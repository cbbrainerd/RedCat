#include <iostream>
#include <string>

void Manual() {
    std::cout << "\nInvalid Input.\nNote that either the -x or -f flags are required.\nCommand line options are the following:\n\n-x: Ask for input from command line.\n-f [filename]: Read script from [filename].\n-m: Just make script to be read from file later. This switch implies -x.\n-s [filename]: Save log to [filename]. By default, this appends to 'logFile'.\n\n";
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
    std::string File;
    if (argc == 1) { //Echo back options
        Manual();
        return -1;
    }
    for (int i = 1; i < argc; ++i) {
        if (((argv[i])[0] != '-') && ((argv[i])[0] != '/')) {
            Manual();
            return -1;
        }
        if (std::char_traits<char>::length(argv[i]) != 2) {
            Manual();
            return -1;
        }
        int switchy = parse((argv[i])[1]);
        switch (switchy) {
            case x:
                ReadFromFile = false;
                break;
            case f:
                if (i + 1 != argc) File = argv[++i];
                else {Manual();return -1;}
                break;
            case s:
                if (i + 1 != argc) LogFile = argv[++i];
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
}    
