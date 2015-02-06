inline Run::Run()
    : numRuns_      (0)
    , numPC_        (0)
    , dataByte_     (170)
    , numTries_     (100)
    , whichMode_    (3)
    , logFile_      ("logFile")
{ }

inline Run& Run::runsPerPowerCycle(unsigned int runs)
{ numRuns_ = runs; return *this; }

inline Run& Run::numberPowerCycles(unsigned int pc)
{ numPC_ = pc; return *this; }

inline Run& Run::data(unsigned short dat)
{ dataByte_ = dat; return *this; }

inline Run& inputMode(unsigned short mode)
{ whichMode_ = mode; return *this; }

inline Run& tryReread(unsigned int reread)
{ numTries_ = reread; return *this }

inline Run& saveTo(std::string filename)
{ logFile_ = filename; return *this }
