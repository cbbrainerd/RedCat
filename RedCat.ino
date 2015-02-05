//Various pins on the RedCat chip - Note: Ground and Vcc not included
//No ribbon cable: no additional pauses- just on the breadboard

const byte CS = 52; // Chip select
const byte OE = 2; // Output enable''
const byte WR = 53; // Write enable
const byte LED = 22; // Testing LED
const byte POWER = 23; // Power pin

const unsigned int REGB_CS = 1U << 21; //"REGB" means on the B register. CS is Chip Select
const unsigned int REGB_OE = 1U << 25; //Output Enable
const unsigned int REGB_WR = 1U << 14; //Write Enable 
const unsigned int REGB_LED = 1U << 26; //Testing LED
const unsigned int REGA_POWER = 1U << 14; //Similarly, "REGA" means this is on the A register. POWER Pin
/* Use a command like: 
REG_PIOB_ODSR |= REGB_CS;
to pull the CS pin high, or
REG_PIOB_ODSR &= ~REGB_CS;
to pull the CS pin low
*/
const int NUM_ADDRESSES = 65536;

const unsigned int NUM_SET_CODES = 6;
const char TERMINATION = '$';
const unsigned int ADDRESS_MASK = 1022; //Used in the address increment function to jump when necessary
const unsigned int ADD_MASK = 0b1111111001111111110; //Used to mask out the address bits more explicitly. Address bits are on the C register
const unsigned int DATA_MASK = 0b1111001111; //Masks out data bits on the D register

//const byte DELAY_AFTER_ADDRESS = 0; //in microseconds, 10 is more than enough, but probably depends on cable setup (NOT USED)
const bool VERBOSE = false;
bool wrongData = false;

//Below variables are for testing.
/*
int breakAddr = -1; //Used in testing code to simulate a single event upset once per write cycle
bool broken = false; //Used to restore data back to correct state after simulated single event upset
int whichBitBreaks = 0; //Single event upset in LSB followed by next-to-LSB, etc. Checks whether certain pins on the Arduino or RedCat are malfunctioning.
*/

typedef enum {NONE, GOT_DATA, GOT_NUM_RUNS, GOT_DELAY, GOT_REPEAT_READS, GOT_MODE, GOT_POWER_CYCLES} states;
typedef enum {ALL0, ALL1, ALTERNATING, NORMAL, RANDOM, TESTING} mode;

states currentState = NONE;
mode currentMode = NORMAL;

unsigned int currentValue = 0;
byte output1, output2, output3;
//const char SET_CODES[] = {'a', 'b', 'c', 'd', 'e', 'f'}; //Characters the computer sends associated with the 'mode'

// Data and address integers
unsigned int correctData = 170;
unsigned int correctDataStart = 170;
unsigned int startMessed = 0;
unsigned int numPowerCycles = 5;
unsigned int divider = 0;
unsigned int numRuns = 0; // Number of runs per power cycle. 0 means run forever

unsigned int delayAfterWrite = 10; // Delay after writing (in milliseconds)
unsigned int readsAfterFailure = 5; // Number of reads if there is a bad read

int i = 0;

void getData() { //Gets data from the PC about what I should do
//  int mod = 0;
  bool done = false;
  while (!done) {
    if (Serial.available()) {
      done = processIncomingByte (Serial.read());
    }
  }

  correctData = messedData(correctData);
  correctDataStart = messedData(correctDataStart);
  startMessed = correctDataStart;
}

void setAddress(const int& addrIn) { //Takes a simple sequential address: addr, in the range 0-65535.
  unsigned int addressInto = findAddress(addrIn);
  if (((addressInto) & (~ADD_MASK)) != 0) {
    Serial.println("Addressing issues.");
    Serial.println(addressInto,BIN);
  }  
  while (true) { 
    REG_PIOC_ODSR &= ~ADD_MASK;
    __asm__("nop\n\t");
    __asm__("nop\n\t");
    __asm__("nop\n\t");
    REG_PIOC_ODSR |= addressInto;
    __asm__("nop\n\t");
    __asm__("nop\n\t");  
    __asm__("nop\n\t");
    if ((REG_PIOC_ODSR & ADD_MASK) == addressInto) break;
    Serial.println("Something went badly wrong in setAddress().");
    Serial.println(REG_PIOC_PDSR,HEX);
    break;
  }
}

unsigned int findAddress(const int& addr) { //Turns the nice "addr" form of addresses (0-65535) into the nasty form required by the arduino register.
  unsigned int addr9 = addr % 512;
  unsigned int addr12 = ((addr/512)*4096);
  return (addr12 + (addr9*2));
}

void handlePreviousState() {
  switch (currentState) {
    case GOT_DATA:
      correctData = currentValue;
      correctDataStart = currentValue;
      Serial.println("Y");
      break;
    case GOT_NUM_RUNS:
      numRuns = currentValue;
      Serial.println("Y");
      break;
    case GOT_DELAY:
      delayAfterWrite = currentValue;
      Serial.println("Y");
      break;
    case GOT_REPEAT_READS:
      readsAfterFailure = currentValue;
      Serial.println("Y");
      break;
    case GOT_MODE:
       if(currentValue==0) {
         currentMode = ALL0;
       }
       else if(currentValue==1) {
         currentMode = ALL1;
       }
       else if (currentValue == 2) {
         currentMode = ALTERNATING;
       }
       else if (currentValue == 3) {
         currentMode = NORMAL;
       }
       else if (currentValue == 4) {
         currentMode = RANDOM;
       }
       else if (currentValue ==5) {
         currentMode = TESTING;
       }
       Serial.println("Y");
       break; 
    case GOT_POWER_CYCLES:
      numPowerCycles = currentValue;
      Serial.println("Y");
      break;
  }
  currentValue = 0;
}



/* Character codes:
 A = data value
 B = number of runs
 C = delay between runs
 D = number of times to re-read address after failure
 E = mode
 F = power cycle
*/
bool processIncomingByte (const byte& c) {
  if (isdigit(c)) {
    currentValue = currentValue * 10;
    currentValue = currentValue + c - '0';
    return false;
  }
  else {
    handlePreviousState();
    switch (c) {
      case 'A':
        currentState = GOT_DATA;
        return false;
      case 'B':
        currentState = GOT_NUM_RUNS;
        return false;
      case 'C':
        currentState = GOT_DELAY;
        return false;
      case 'D':
        currentState = GOT_REPEAT_READS;
        return false;
      case 'E':
        currentState = GOT_MODE;
        return false;
      case 'F':
        currentState = GOT_POWER_CYCLES;
        return false;
      case TERMINATION:
        return true;
      default:
        currentState = NONE;
        return false;
    }
  }
}



void establishContact() {
  char holder[3];
  bool done = false;
  char input = ' ';

  do {
    Serial.print('$');
    Serial.print('\n');
    delay(10);
    REG_PIOB_ODSR |= REGB_LED; //digitalWrite(LED, HIGH);
  }
  while (Serial.read() != 'A');
}

void prepareToWrite() {
  REG_PIOD_IDR = DATA_MASK;
  REG_PIOB_ODSR |= REGB_OE + REGB_CS + REGB_WR; //Pulls all control pins high
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  for (i = 25; i < 31; i++) {
    pinMode(i, OUTPUT);
  } //Sets DATA pins to output
  delayMicroseconds(10);
}

void prepareToRead() {
  REG_PIOD_IER = DATA_MASK;
  REG_PIOB_ODSR |= REGB_OE + REGB_CS + REGB_WR; //Pulls all control pins high
  pinMode(11, INPUT);
  pinMode(12, INPUT);
  for (i = 25; i < 31; i++) {
    pinMode(i, INPUT);
  } //Sets DATA pins to output
  delayMicroseconds(10);
}

void exitReady() { //Pulls all pins high- brings the RedCat chip out of ready mode for all operations
  REG_PIOB_ODSR |= REGB_OE + REGB_CS + REGB_WR; //Pulls all control pins high
}
int readData() { //Reading data doesn't require any particular pins to be set- just change the address.
  
//  REG_PIOB_ODSR |= REGB_CS; //digitalWrite(CS, HIGH);
//  REG_PIOB_ODSR |= REGB_OE; //digitalWrite(OE, HIGH);
//  __asm__("nop\n\t");   
//  __asm__("nop\n\t");  
/*
  REG_PIOC_ODSR = (REG_PIOC_ODSR & (~ADD_MASK)) ^ (address & ADD_MASK); //writes the address bits, keeps the others unchanged. By construction masking the address is unnecessary, but this keeps dedicated pins from being set accidentally if the address is corrupted in memory or something goes wrong.
//new address not valid for 15ns- put 2 NOP. (1 NOP ~= 10ns delay)
  __asm__("nop\n\t");   
  __asm__("nop\n\t");  
  */
  REG_PIOB_ODSR &= ~REGB_CS; //digitalWrite(CS, LOW);
  REG_PIOB_ODSR &= ~REGB_OE; //digitalWrite(OE, LOW);
  
  REG_PIOD_ODSR |= 0; //This forces the PDSR to update? Anyway, it helps...
  __asm__("nop\n\t");
  __asm__("nop\n\t"); 
  __asm__("nop\n\t");     
  REG_PIOD_ODSR |= 0; //This forces the PDSR to update?
  __asm__("nop\n\t");
  __asm__("nop\n\t"); 
  __asm__("nop\n\t");     
  unsigned int data = REG_PIOD_PDSR & DATA_MASK;

  REG_PIOB_ODSR |= REGB_OE; //digitalWrite(OE, HIGH);
  REG_PIOB_ODSR |= REGB_CS; //digitalWrite(CS, HIGH); 
/*  __asm__("nop\n\t");
  __asm__("nop\n\t");
  __asm__("nop\n\t");
*/
  return data;
}



// Read a byte of data given an address and an array of data
/* int readDataAddr(const long& address) {
  //function assumes that OE and CS have already been drawn low
 
  REG_PIOC_ODSR = (REG_PIOC_ODSR & (~ADD_MASK)) ^ (address & ADD_MASK); //writes the address bits, keeps the others unchanged

  __asm__("nop\n\t");
  __asm__("nop\n\t");

  return (REG_PIOD_PDSR & 0b01111001111);
}
*/


// Write a byte of data
void writeData() { //Pulls the correct pins to write whatever is in the D register to the RedCat chip

//  REG_PIOC_ODSR = (REG_PIOC_ODSR & (~ADD_MASK)) ^ (address & ADD_MASK); //writes the address bits, keeps the others unchanged
//  REG_PIOD_ODSR = (REG_PIOD_ODSR & (~DATA_MASK)) ^ (data & DATA_MASK); //ditto for data
  //new address not valid for 15 ns
  __asm__("nop\n\t");   
  __asm__("nop\n\t");
  REG_PIOB_ODSR &= ~REGB_CS;  
  REG_PIOB_ODSR &= ~REGB_WR;
  //should be low for 15 ns before pulling back high
  __asm__("nop\n\t");   
  __asm__("nop\n\t");
  __asm__("nop\n\t");   
  __asm__("nop\n\t");
  REG_PIOB_ODSR |= REGB_WR;
  REG_PIOB_ODSR |= REGB_CS;
}

void setData(const unsigned int& data) { //Sets the D register to whatever data is input to this function- in the "messed up" format
  REG_PIOD_ODSR &= ~DATA_MASK;;   
  __asm__("nop\n\t");
  __asm__("nop\n\t");   
  __asm__("nop\n\t");
  while(true) {
    REG_PIOD_ODSR |= (data & DATA_MASK);   
    __asm__("nop\n\t");
    __asm__("nop\n\t");   
    __asm__("nop\n\t");
    if ((REG_PIOD_ODSR & DATA_MASK) == data) break;
    Serial.println("Something went badly wrong in setData().");
    break;
  }
}  


String toBinary(const int& val, const int& length) {
  String holder = "";
  int mask = 1 << (length - 1);

  for (i = 0; i < length; i++) {
    if ((val & mask) == 0) {
      holder += "0";
    }
    else {
      holder += "1";
    }
    // Bitshift the mask over one character
    mask >>= 1;
  }
  return holder;
}



/*void reread(const long& address, const int& times, int (*fread)(const long&)) {
  // If the read-back data is incorrect read the address more
  int reads[times];
  int netFalseReads = 0;
  long resetaddr = incrementAddress(address);

  for (i = 0; i < times; i++) {
    (*fread)(resetaddr);
    reads[i] = (*fread)(address);
    if (correctData != reads[i]) {
      netFalseReads++;
    }
  }

  if (VERBOSE) {
    Serial.print("CURRENT MODE: ");
    Serial.println(currentMode);
    Serial.print("Total number of invalid reads: ");
    Serial.println(netFalseReads + 1);
    Serial.print("Read back: ");
    for (i = 0; i < times; i++) {
      Serial.print(reads[i]);
      Serial.print("\t");
    }
    Serial.println();
  }
  else {
    Serial.print("BR ");
    Serial.println(netFalseReads + 1, HEX);
    Serial.print("NR ");
    for (i = 0; i < times; i++) {
      Serial.print(reads[i], HEX);
      Serial.print("\t");
    }
    Serial.println();
  }
}
*/

/*long incrementAddress(const long& address) {
  // Strange addition is necessary to ensure that dedicated pins do not get set. In this case, pins 33 through 41 are being used and then pins 45 through 51. Thus, when the following situation is met:
  // 0000000 00 111111111 0 -> 0000001 00 000000000 0
  if ((address % 1024) == (ADDRESS_MASK)) {
    return (address + 3074);
  }
  else {
    return (address + 2);
  }
}
*/

int messedData(int ret) { //Inserts zeroes to line up the registers
  int mod;
  // Using PD0 through PD9 without PD4 and PD5. Thus, need to split the data at PD4 and PD5;
  mod = ret & 15; // Get the last 4 digits
  ret >>= 4; // Bit shift up to where 0's should start
  ret <<= 6; // Add the two extra zeros
  ret += mod; // Re-add mod
  return ret;
}


void setCorrectData(const int& addr){
  if(currentMode == ALL0) {
    correctData = 0; //Sends all 0's, though this could be implemented a little more efficiently?
  }
  else if (currentMode == ALL1) {
    correctData = DATA_MASK; //Sends all 1's. Like ALL0, pretty redundant.
  }
  else if (currentMode == ALTERNATING) {
    if ((addr % 2) == 1) {
      correctData = startMessed ^ DATA_MASK; //This flips every bit each successive byte. For instance, it writes 00001111 followed by 11110000 or 01010101 -> 10101010, or any other pattern like that.
    }
    else {
      correctData = startMessed;
    }  
  }
  else if (currentMode == NORMAL) {
/*    if ((addr == breakAddr) || broken) {
      correctData = correctData ^ (0b1 << whichBitBreaks);
      broken = !broken;
    } //FOR TESTING ONLY */ 
      ;//Doesn't do anything. Just keeps the data as it is. Writes the same data to each byte successively.
  }
  else if (currentMode == RANDOM) {
    correctData = messedData(random(256)); //DOES NOT WORK
  }
}



void setup() {
  correctData=messedData(correctData);
  Serial.begin(115200);
  // Set modes of IO 
//  REG_PIOB_OWER |= (REGB_LED + REGB_OE + REGB_CS + REGB_WR);
//  __asm__("nop\n\t");
  delayMicroseconds(50); //Seems reasonable to wait for a bit after setting the bits to output before changing them? At any rate, the performance hit is truly negligible.
// The REG_PIOB_OWER register controls input and output of the B register pins, but doesn't work for some reason
  pinMode(CS, OUTPUT);
  pinMode(OE, OUTPUT);
  pinMode(WR, OUTPUT);
  pinMode(LED, OUTPUT);
  REG_PIOB_ODSR |= (REGB_LED + REGB_OE + REGB_CS + REGB_WR);
/*
  The above line is equivalent to:
  digitalWrite(LED, HIGH);
  digitalWrite(OE, HIGH);
  digitalWrite(CS, HIGH);
  digitalWrite(WR, HIGH);
*/
  //These pins are to be set high after all operations to reduce safeguard overhead
  // Address pins
//    REG_PIOC_OWER |= ADD_MASK; //sets all address pins to output
  for (i = 33; i < 52; i++) {
    pinMode(i, OUTPUT);
    if (i == 41) {
        i = 44;
    }
  }
}

void loop() {
  establishContact();
  getData();

/*  Serial.print("Mode:");
  Serial.println(currentMode);  
  Serial.print("Data:");
  Serial.println((correctData%16)+((correctData>>6)<<4),HEX);
  Serial.print("Runs:");
  Serial.println(numRuns);
  Serial.print("Power cycles:");
  Serial.println(numPowerCycles);
  Serial.print("Rereads after failure:");
  Serial.println(readsAfterFailure);
*/  
  for (int pwrc = 0; pwrc < numPowerCycles; pwrc++) {
    for (int rn = 0; (rn < numRuns) && (numRuns != 0); rn++) { //main loop for the tests
      int addr = 0;
      long addressInt = 0;/
      int dataInt = 0;
      int seed = analogRead(0);
//      REG_PIOC_OWER |= DATA_MASK; //Sets data pins to output

//      REG_PIOC_ODSR &= ~ADD_MASK;
      correctData = (messedData(correctDataStart)) & DATA_MASK; //=0?
      randomSeed(seed);
//      breakAddr = random(65534); //Simulates a single event upset at one address per write cycle.
      addressInt = 0;
      prepareToWrite(); //Prepare RedCat for writing operations.
      if (currentMode == RANDOM) {
        Serial.print(seed,HEX);
        Serial.println(seed,HEX);
      }
      for (addr = 0; addr < NUM_ADDRESSES; addr++) { //write loop
        setCorrectData(addr);
        setAddress(addr);
 /*       Serial.print("I am about to write the byte ");
        Serial.print((correctData%16)+((correctData>>6)<<4),HEX);
        Serial.print(" to the address ");
        Serial.print(addr,HEX);
        Serial.println(".");
*/
        setData(correctData);
        writeData(); //Pulls the correct pins to write whatever is in the D register to the RedCat chip
      }
//      REG_PIOC_OWER &= ~DATA_MASK; //Sets data pins to input

      addressInt = 0; //reset the address between our run and read cycles back to 0
      correctData = (messedData(correctDataStart)) & DATA_MASK; //Resets data back to correctData for address 0
//      breakAddr = -1;
      randomSeed(seed);
      
 /*     whichBitBreaks += 1;
      if (whichBitBreaks == 4) whichBitBreaks = 6;
      if (whichBitBreaks == 10) whichBitBreaks = 0;
 */  //For testing purposes
      prepareToRead(); //Prepares RedCat chip for reading operations
      
      for (addr = 0; addr < NUM_ADDRESSES; addr++) { //read loop
        setCorrectData(addr);     
        setAddress(addr);
        wrongData = false;
        
        for (int numloop = 0; numloop < (readsAfterFailure + 1); numloop++) {
//          readDataAddr(addressInt); Duplicate line?
          dataInt = readData();
          // Print out the address and received data if bad data read
          if ((correctData != dataInt)) {
            wrongData = true;
            if (VERBOSE) {
              Serial.print("\nAddress:\t");
              Serial.println(addr);
              Serial.print("Correct data:\t");
              Serial.println(correctData);
              Serial.print("Read data:\t");
              Serial.println(dataInt);
              Serial.print("Difference of the data:\t");
              Serial.println(correctData - dataInt); //toBinary(dataInt ^ correctData, 10)
            }
            else {
              Serial.print(".");
              Serial.println(numloop);
 //             Serial.print("AD ");
              Serial.println(addr,HEX);
 //             Serial.print("CD ");
              Serial.println((correctData%16)+((correctData>>6)<<4),HEX);
 //             Serial.print("DI ");
              Serial.println((dataInt%16)+((dataInt>>6)<<4),HEX);
            }
            // If the read-back data is incorrect reread n times, n = readsAfterFailure
//          reread(addressInt, readsAfterFailure, readDataAddr);
          }
          if (!wrongData) break;
          if (addr == 0) setAddress(100); //Fixes an issue with address 0.
          setAddress(addr);
        }
      }
      
      exitReady();
      //Bring the necessary pins up at the end of the read cycle

//      REG_PIOB_ODSR |= REGB_OE; //digitalWrite(OE, HIGH);
//      REG_PIOB_ODSR |= REGB_CS; //digitalWrite(CS, HIGH);

      if (VERBOSE) {
        Serial.print("Done with run ");
        Serial.print(rn + 1);
        Serial.print('/');
        Serial.println(numRuns);
      }
      else {
        Serial.println("--");
      }
    }
    if (VERBOSE) {
      Serial.print("Done with all runs, power cycle number ");
      Serial.print(pwrc+1);
      Serial.print(" of ");
      Serial.println(numPowerCycles);
    }
    else {
      Serial.println("----\n");
    }

    REG_PIOA_ODSR &= ~REGA_POWER; //digitalWrite(POWER, LOW);
    delay(2000);
    REG_PIOA_ODSR |= REGA_POWER; //digitalWrite(POWER, HIGH);
  }
  Serial.println("------");
}
