#include <iostream>
#include <string>
#include <Runs.hpp>
#include <sstream>

//Going to want to put bounds checks on each of these.
Run& Run::numRuns__(int runs) {
    numRuns_ = runs; return *this;
}    
Run& Run::numPC__(int pc) {
    numPC_ = pc; return *this;
}    
Run& Run::dataByte__(int databyte) {
    dataByte_ = databyte; return *this;
}    
Run& Run::numTries__(int reread) {
    numTries_ = reread; return *this;
}
Run& Run::whichMode__(int mode) {
    whichMode_ = mode; return *this;
} 
Run& Run::logFile__(std::string filename) {
    logFile_ = filename; return *this;
}    

Run::Run()
    : numRuns_      (0)
    , numPC_        (0)
    , dataByte_     (170)
    , numTries_     (100)
    , whichMode_    (3)
    , logFile_      ("logFile")
{ }

enum {data,dataByte,numRuns,noCommand,numTries,whichMode,numPC,nowDone};

int Run::ParseCommands(char command) {
    if (command == 'A') return dataByte;
    if (command == 'B') return numRuns;
    if (command == 'C') return noCommand;
    if (command == 'D') return numTries;
    if (command == 'E') return whichMode;
    if (command == 'F') return numPC;
    if (command == '$') return nowDone;
    return data;
}

int Run::Tui() {
    return 0;   //Not yet written- no interface
}   

Run::Run(std::string inputInfo) 
    : numRuns_      (0)
    , numPC_        (0)
    , dataByte_     (170)
    , numTries_     (100)
    , whichMode_    (3)
    , logFile_      ("logFile")
{ //Read from file and set runtime variables
    bool CommandReceived = false;
    char comm;
    int whichVar = 0; //The contents of this variable determines which variable is set
    int dat; //The contents of this variable are set to the determined variable
    std::stringstream stream;
    stream << inputInfo;
    while (true) { //Reads one line from input file and interprets it as a command or data.
        if (whichVar == nowDone) break;
        if (CommandReceived) {
            if (stream >> dat) {
                switch(whichVar) {
                    case dataByte:
                        dataByte__(dat);
                        break;
                    case numRuns:
                        numRuns__(dat);
                        break;
                    case numTries:
                        numTries__(dat);
                        break;
                    case whichMode:
                        whichMode__(dat);
                        break;
                    case numPC:
                        numPC__(dat);
                        break;
                    default:
                        throw("Data given without valid command");
                }
                CommandReceived = false;
            }
        }
        else if (stream >> comm) {
            whichVar = ParseCommands(comm);
            CommandReceived = true;
        }
    }
#ifndef NDEBUG    
    std::cout<<"A"<<dataByte_<<"\nB"<<numRuns_<<"\nD"<<numTries_<<"\nE"<<whichMode_<<"\nF"<<numPC_<<"\n";
#endif
}    
