#include <iostream>
#include <string>
#include <Runs.hpp>

void Manual() {
    std::cout << "\nInvalid Input.\nCommand line options are the following:\n\n-x: Ask for input from command line.\n-f [filename]: Read script from [filename]. This defaults to \"DefaultFilename\"\n-m: Just make script to be read from file later. This switch implies -x.\n-s [filename]: Save log to [filename]. By default, this appends to 'logFile'.\n\n";
}

enum{nul,x,f,s,m};

int parse(const char argument) {
    if (argument == 'x') return x;
    if (argument == 'f') return f;
    if (argument == 's') return s;
    if (argument == 'm') return m;
    return nul;
}

int main(int argc, char* argv[]) {
    bool ReadFromFile = true;
    bool doRun = true;
    std::string LogFile = "LogFile";
    std::string File = "DefaultScript";
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
            int switchy = parse((argv[i])[j]);
            switch (switchy) {
                case x:
                    ReadFromFile = false;
                    break;
                case f:
                    if ((i + 1 != argc) && (j + 1 == lenarg)) File = argv[++i];
                    else {Manual();return -1;}
                    break;
                case s:
                    if ((i + 1 != argc) && (j + 1 == lenarg)) LogFile = argv[++i];
                    else {Manual();return -1;}
                    break;
                case m:
                    doRun = false;
                    break;
                default:
                    Manual();
                    return -1;
            }
        }
    } //End of parsing loop
    Run run;
    int Finished = 0;
    while (!Finished) {
        Finished = (ReadFromFile ? run.ReadFromFile(File) : run.Tui());
    }
}    
