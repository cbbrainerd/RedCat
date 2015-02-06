#ifndef __RUNH_INCLUDED__
#define __RUNH_INCLUDED__

class Run;

class CreateRun {
public:
    Run();
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

inline CreateRun::CreateRun()
    : numRuns_      (0)
    , numPC_        (0)
    , dataByte_     (170)
    , numTries_     (100)
    , whichMode_    (3)
    , logFile_      ("logFile")
{ }

inline CreateRun& CreateRun::runsPerPowerCycle(unsigned int runs)
{ numRuns_ = runs; return *this; }

inline CreateRun& CreateRun::numberPowerCycles(unsigned int pc)
{ numPC_ = pc; return *this; }

inline CreateRun& CreateRun::data(unsigned short dat)
{ dataByte_ = dat; return *this; }

inline CreateRun& inputMode(unsigned short mode)
{ whichMode_ = mode; return *this; }

inline CreateRun& tryReread(unsigned int reread)
{ numTries_ = reread; return *this }

inline CreateRun& saveTo(std::string filename)
{ logFile_ = filename; return *this }

class Run {
Public: 
    Run(CreateRun const& params);
}

Run::Run(CreateRun const& params) {

}
