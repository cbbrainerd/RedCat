#ifndef __RUNH_INCLUDED__
#define __RUNH_INCLUDED__
#include <string>

class Run {
public:
    Run();
    int ReadFromFile(std::string);
    int Tui();
private: 
    Run& numRuns__(int);
    Run& numPC__(int);
    Run& dataByte__(int);
    Run& numTries__(int);
    Run& whichMode__(int);
    Run& logFile__(std::string);
    int ParseCommands(char);
    int numRuns_;
    int numPC_;
    int dataByte_;
    int numTries_;
    int whichMode_;
    std::string logFile_;
};

#endif
