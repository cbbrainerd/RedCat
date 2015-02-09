#ifndef __RUNH_INCLUDED__
#define __RUNH_INCLUDED__
#include <string>

class Run {
public:
    Run();
    int ReadFromFile(std::string);
    int Tui();
private: 
    int ParseCommands(char);
    int numRuns_;
    int numPC_;
    int dataByte_;
    int numTries_;
    int whichMode_;
    std::string logFile_;
};

#endif
