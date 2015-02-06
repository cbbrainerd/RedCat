#ifndef __RUNH_INCLUDED__
#define __RUNH_INCLUDED__
#include <string>

class Run {
public:
    Run();
    Run& runsPerPowerCycle(unsigned int runs);
    Run& numberPowerCycles(unsigned int pc);
    Run& data(unsigned short databyte);
    Run& tryReread(unsigned int reread);
    Run& inputMode(unsigned short mode);
    Run& saveTo(std::string filename);
private:
    unsigned int numRuns_;
    unsigned int numPC_;
    unsigned short data_;
    unsigned int numTries_;
    unsigned int whichMode_;
    std::string logFile_;
};
