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
