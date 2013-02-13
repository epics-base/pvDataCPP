#include "mb.h"

#include <time.h>
#include <map>
#include <math.h>
#include <cstdlib>

#include <fstream>
#include <sstream>
#include <unistd.h>

#include <iomanip>

#include <epicsMutex.h>

#if defined(__APPLE__)
#include <mach/mach_time.h>
uint64_t MBTime()
{
    return mach_absolute_time();
}
#else
uint64_t MBTime()
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return static_cast<uint64_t>(ts.tv_sec) * 1000000000 + static_cast<uint64_t>(ts.tv_nsec);
}
#endif


void MBPointAdd(MBEntity &e, intptr_t id, uint8_t stage)
{
    // no copy and no MBPoint init solution
    const std::size_t ix = ATOMIC_GET_AND_INCREMENT(e.pos);
    MBPoint& p = e.points[ix];
    p.id = id; p.stage = stage;
    p.time = MBTime();
}

void MBCSVExport(MBEntity &e, std::ostream &o)
{
    const std::size_t len = ATOMIC_GET(e.pos); 
    for (std::size_t i = 0; i < len; i++)
    {
        MBPoint& p = e.points[i];
        o << p.id << ',' << static_cast<uint32_t>(p.stage) << ',' << p.time << std::endl;
    }
}

// NOTE: this method is not thread-safe
void MBCSVImport(MBEntity &e, std::istream &i)
{
    std::string line;
    char c;    

    e.points.resize(0);
    size_t lc = 0;
    while (getline(i,line))
    {
        lc++;
        std::istringstream is(line);
        MBPoint p;
        is >> p.id >> c;
        uint32_t s; is >> s >> c; p.stage = s;
        is >> p.time;
        
        if (is.good() || is.eof())        
            e.points.push_back(p);
        else
        {
            std::cerr << "failed to parse line " << lc << ": \"" << line << "\"" << std::endl;
        }
    }
    e.pos = e.points.size();
}

void MBNormalize(MBEntity &e)
{
    std::map<intptr_t, uint64_t> lastTime;
    
    const std::size_t len = ATOMIC_GET(e.pos); 
    for (std::size_t i = 0; i < len; i++)
    {
        MBPoint& p = e.points[i];
        if (p.stage == 0)
            lastTime[p.id] = p.time;
            
        std::map<intptr_t, uint64_t>::iterator last = lastTime.find(p.id);
        if (last == lastTime.end())
        {
            std::cerr << "no 0 stage for " << e.name << ", id = " << p.id << std::endl;
            p.time = -1;    // TODO error?!
        }
        else
        {
            uint64_t lt = last->second;
            last->second = p.time;
            p.time -= lt;
        }
    }
}

struct MBStatistics
{
    std::size_t count;
    uint64_t min;
    uint64_t max;
    uint64_t rms;
    
    MBStatistics() :
        count(0),
        min(-1),
        max(0),
        rms(0.0)
    {}
    
    MBStatistics(uint64_t sample) :
        count(1),
        min(sample),
        max(sample),
        rms(sample*sample)
    {
    };
    
    void addSample(uint64_t sample)
    {
        count++;
        if (sample < min) min = sample;
        if (sample > max) max = sample;
        rms += sample*sample;
    };
};

typedef std::map<uint8_t, MBStatistics> StatsMapPerStage;

void MBStats(MBEntity &e, std::ostream &o)
{
    MBNormalize(e);
    
    StatsMapPerStage stats;
    
    const std::size_t len = ATOMIC_GET(e.pos); 
    for (std::size_t i = 0; i < len; i++)
    {
        MBPoint& p = e.points[i];
        
        // first stage is start time, skip
        if (p.stage == 0)
            continue;
            
        StatsMapPerStage::iterator s = stats.find(p.stage);
        if (s == stats.end())
            stats[p.stage] = MBStatistics(p.time);
        else
            s->second.addSample(p.time);
    }
    
    uint64_t smin = 0;
    uint64_t smax = 0;
    double srms = 0;

    for (StatsMapPerStage::iterator i = stats.begin();
         i != stats.end();
         i++)
    {
        smin += i->second.min;
        smax += i->second.max;
        double rrms = sqrt(i->second.rms/(double)i->second.count);
        srms += rrms;
        
        o << "stage " << std::setw(4) << static_cast<uint32_t>(i->first)
                      << ": min = " << std::setw(16) << i->second.min 
                      << ", max = " << std::setw(16) << i->second.max
                      << ", rms = " << std::setw(16) << static_cast<uint64_t>(rrms) << std::endl;
    }
    
    o << std::string(82,'-') << std::endl;
    
    o << "stage " << std::setw(4) << "sum"
                    << ": min = " << std::setw(16) << smin 
                    << ", max = " << std::setw(16) << smax
                    << ", rms = " << std::setw(16) << static_cast<uint64_t>(srms) << std::endl;
}

typedef std::vector<MBEntity*> EntitiesVector;

static int nifty_counter;
static epicsMutex* MBMutex;

// The counter is initialized at load-time, i.e., before any of the static objects are initialized.
MBMutexInitializer::MBMutexInitializer ()
{
    if (0 == nifty_counter++)
    {
        // Initialize static members.
        MBMutex = new epicsMutex();
    }
}

MBMutexInitializer::~MBMutexInitializer ()
{
    if (0 == --nifty_counter)
    {
        // Clean-up.
        delete MBMutex;
    }
}

class MutexLock {
public:

    explicit MutexLock(epicsMutex &m) :
        mutexPtr(m),
        locked(true)
    {
        mutexPtr.lock();
    }

    ~MutexLock()
    {
        unlock();
    }

    void lock()
    {
        if (!locked)
        {
            mutexPtr.lock();
            locked = true;
        }
    }
    void unlock()
    {
        if (locked)
        {
            mutexPtr.unlock();
            locked = false;
        }
    }

private:
    epicsMutex &mutexPtr;
    bool locked;
};


void MBEntityRegister(MBEntity *e)
{
    MutexLock lock(*MBMutex);
    static EntitiesVector MBEntities;

    if (e)
    {
        MBEntities.push_back(e);
    }
    else
    {
        for(EntitiesVector::const_iterator i = MBEntities.begin();
            i != MBEntities.end();
            i++)
        {
            // skip empty entities
            if ((*i)->pos)
            {
                char fileName[128];
                char* path = getenv("MB_OUTPUT_DIR");
                if (path == 0) path = const_cast<char*>(".");
                snprintf(fileName, 128, "%s/mb_%s_%d.csv", path, (*i)->name.c_str(), getpid());
                std::ofstream out(fileName);
                if (out.is_open())
                {
                    MBCSVExport(*(*i), out);
                    out.close();
                }
                else
                {
                    std::cerr << "failed to create a file " << fileName << ", skipping..." << std::endl;
                }
            }
        }
    }
}

void MBAtExit()
{
    MBEntityRegister(0);
}


void MBInit()
{
    MutexLock lock(*MBMutex);
    static bool inited = false;
    if (!inited)
    {
        inited = true;
        atexit(MBAtExit);
    }
}

