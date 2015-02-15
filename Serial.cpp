#include <iostream>
#include <string>
#include <Runs.hpp>
#include <fstream>
//#include <CSerial>

constexpr int Baud = 115200;
std::string Port = "COM20";

void Manual() {
    std::cout << "\nInvalid Input.\nCommand line options are the following:\n\n-x: Ask for input from command line.\n-f [filename]: Read script from [filename]. This defaults to \"DefaultFilename\"\n-m: Just make script to be read from file later. This switch implies -x.\n-s [filename]: Save log to [filename]. By default, this appends to 'logFile'.\n-p [Port]: Select the serial port to use.\n\n";
}

int main(int argc, char* argv[]) {
    bool ReadFromFile = true;
    bool doRun = true;
    std::string LogFile = "LogFile";
    std::string filename = "DefaultScript";
    if (argc == 1) { //Echo back options
        Manual();
        return -1;
    }
    for (int i = 1; i < argc; ++i) {
        if (((argv[i])[0] != '-') && ((argv[i])[0] != '/')) {
            Manual();
            return -1;
        }
        int lenarg = std::char_traits<char>::length(argv[i]);
        if (lenarg < 2) {
            Manual();
            return -1;
        }
        for (int j = 1; j < lenarg; ++j) {
            switch ((argv[i])[j]) {
                case 'x':
                    ReadFromFile = false;
                    break;
                case 'f':
                    if ((i + 1 != argc) && (j + 1 == lenarg)) filename = argv[++i];
                    else {Manual();return -1;}
                    break;
                case 's':
                    if ((i + 1 != argc) && (j + 1 == lenarg)) LogFile = argv[++i];
                    else {Manual();return -1;}
                    break;
                case 'm':
                    doRun = false;
                    break;
                case 'p':
                    if ((i + 1 != argc) && (j + 1 == lenarg)) Port = argv[++i];
                    else {Manual();return -1;}
                    break;
                default:
                    Manual();
                    return -1;
            }
        }
    } //End of parsing loop
    int Finished = 0;
    if(ReadFromFile) {
        std::ifstream file;
        file.open(filename,std::ifstream::in);
        std::string inputInfo;
        while(std::getline(file,inputInfo)) {
            Run run = Run(inputInfo);
//            CSerial serial;
//            serial.Open(Port,Baud);
        }
    }
}    

