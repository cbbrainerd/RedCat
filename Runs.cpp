#include <iostream>
#include <string>
#include <Runs.hpp>
#include <fstream>
#include <sstream>

Run::Run()
    : numRuns_      (0)
    , numPC_        (0)
    , dataByte_     (170)
    , numTries_     (100)
    , whichMode_    (3)
    , logFile_      ("logFile")
{ }

enum {data,dataByte,numRuns,numTries,whichMode,numPC,nowDone};

int Run::ParseCommands(char command) {
    if (command == 'A') return dataByte;
    if (command == 'B') return numRuns;
    if (command == 'C') return numTries;
    if (command == 'D') return whichMode;
    if (command == 'E') return numPC;
    if (command == '$') return nowDone;
    return data;
}

int Run::Tui() {
    return 0;   //Not yet written- no interface
}   

int Run::ReadFromFile(std::string filename) {
    std::ifstream file;
    file.open(filename,std::ifstream::in);
    std::string inputInfo;
    std::stringstream stream;
    char comm;
    int whichVar = 0;
    int dat;
    while (std::getline(file,inputInfo)) { 
        stream << inputInfo.c_str();
        if (stream >> comm) {
            if (!whichVar) whichVar = ParseCommands(comm);
            else continue;
        }
        if (whichVar == nowDone) break;
        if (stream >> dat) {
            switch(whichVar) {
                case dataByte:
                    dataByte_ = dat;
                    break;
                case numRuns:
                    numRuns_ = dat;
                    break;
                case numTries:
                    numTries_ = dat;
                    break;
                case whichMode:
                    whichMode_ = dat;
                    break;
                case numPC:
                    numPC_ = dat;
                    break;
                default:
                    throw("Data given without command");
            }
        }    
    }
}    
