#ifndef __CREATERUNH_INCLUDED__
#define __CREATERUNH_INCLUDED__

class Run;

class CreateRun {
public:
    CreateRun();
    Run& runsPerPowerCycle(unsigned int runs);
    Run& numberPowerCycles(unsigned int pc);
    Run& data(unsigned short databyte);
    Run& tryReread(unsigned int reread);
    Run& inputMode(unsigned short mode);
    Run& saveTo(std::string filename);
private:
    friend class Run;
    unsigned int numRuns_;
    unsigned int numPC_;
    unsigned short data_;
    unsigned int numTries_;
    unsigned int whichMode_;
    std::string logFile_;
};
