/*
 * ========================================================================
 *
 *       Filename:  pollnpublish.cpp
 *
 *    Description:  Sample question for thread pollAndPublish()
 *
 *        Version:  1.0
 *        Created:  13/02/19 23:54:43
 *       Revision:  none
 *       Compiler:  -std=gnu++17   (g++)
 *
 *         Author:  Imran Khan (C), imrank340@gmail.com
 *   Organization:  Onami Systems Ltd
 *
 * =========================================================================
 * Attempt this question if you have experience with the C programming language
 * The following is a thread function for a low-priority thread. It uses
 * time-out of the thread-flag wait function, osThreadFlagsWait(),
 * to regulate periodic monitoring performed by PollAndPublish().
 *
 * At what frequency will the system tick count roll over?
 *
 * What are the risks, if any, with the above code relating to:
 *   A. System tick count roll over.
 *   B. Periodic monitoring occasionally taking longer than mPollPeriodms.
 *
 * Modify the code to protect against any risks identified above.
 *
 * During development testing I wish to temporarily reduce the frequency
 * with which periodic monitoring is performed slightly, so I add a small
 * fixed offset to the waitTimeoutms calculation. Line 10 becomes:
 *
 * uint32_t waitTimeoutms = (nextSampleStartTickCount - nowTickCount) + 10;
 *
 * Does this have the desired effect? If not, why not?
 *
 * Why do we set ClientThreadFlag thread-flags on completion of the processing
 * associated with WorkerThreadFlag::Stop and WorkerThreadFlag::Start
 * thread-flags being set?
 *
 *     3 Timer Wrapper.
 * Attempt this question if you have experience with the C++ programming
 * language. Write a C++ wrapper class for the timer management functionality
 * described by: (try link below)
 *
 * http://www.keil.com/pack/doc/CMSIS/RTOS2/html/group__CMSIS__RTOS__TimerMgmt.html
 *
 * In addition to your class containing an appropriate number of constructors
 * accepting appropriate parameters, the public interface of your class should
 * include:
 *
 *  osStatus_t  start(uint32_t ticks);
 *  osStatus_t  stop(void);
 *  bool        isRunning(void);
 *
 * You'll need to choose and implement a suitable approach for notifying
 * objects that use instances of your class when timers expire/elapse.
 *
 */
#include <iostream>
#include <thread>
#define Q_UNUSED(x) (void)x;

#define MYMONITOR           1

#define red        "\033[1;31m"     /* 0 -> normal ;  31 -> red */
#define cyan       "\033[1;36m"     /* 1 -> bold ;    36 -> cyan */
#define green      "\033[1;32m"     /* 4 -> underline;32 -> green */
#define blue       "\033[1;34m"     /* 9 -> strike ;  34 -> blue */

#define cls        "\033[2J\033[1;1H"   /* Clear the screen */
#define none       "\033[0m"            /* to flush the previous property */

#define osFlagsError          0x80000000U
#define osWaitForever         0xFFFFFFFFU
#define osFlagsWaitAny        0x00000000U
#define osWaitForAll          0x00000001U
#define osFlagsNoClear        0x00000002U
#define osFlagsError          0x80000000U
#define osFlagsErrorUnkonwn   0xFFFFFFFFU
#define osFlagsErrorTimeout   0xFFFFFFFEU

using namespace std;

#ifndef MYMONITOR
    void operator()() const {
        for (int i=0; i<10; i++)
            cout << "Display Thread Executing.." << endl;
    }
#endif

class Monitor
{
public:
    // default constructor  -- any one choice could be suffice.
    Monitor() : nextSampleStartTickCount(0.0),
                mPollPeriodms(0.0) { }         
    // Copy constructor
    Monitor (const Monitor &m) { 
        nextSampleStartTickCount = m.nextSampleStartTickCount;
        mPollPeriodms = m.mPollPeriodms; }
    // constructor with default trailing arguments
    Monitor (double x, double y = 0.0) { 
        nextSampleStartTickCount = x;
        mPollPeriodms = y; }

    virtual ~Monitor() { };
    void DoWork(void);
    //void PollnPublish(void);
    void PollnPublish(double y, double x, bool flags, bool flagsValid);
private:
    double osKernelGetTickCount();
    uint32_t osThreadFlagsWait(uint32_t flags, uint32_t option, uint32_t timeout);
    uint32_t osThreadFlagsSet(uint32_t tId, uint32_t flags);
    double nextSampleStartTickCount;
    double mPollPeriodms;
    bool   mRunning;
    enum WorkerThreadFlag {Start, Stop, Terminate, Suspend};
    enum ClientThreadFlag {Started, Stopped};
};
double Monitor::osKernelGetTickCount()
{
    struct timespec now;
    if (clock_gettime(CLOCK_MONOTONIC, &now))
        return 0;
    return now.tv_sec * 1000.0 + now.tv_nsec / 1000000.0;
}
uint32_t Monitor::osThreadFlagsWait(uint32_t flags, uint32_t option, uint32_t timeout)
{
    if (option == osFlagsWaitAny && timeout == 0)
        return flags = mRunning;
    else
        return flags = osFlagsError;
}

uint32_t Monitor::osThreadFlagsSet (uint32_t tId, uint32_t flags)
{
    // Get the current thread id: under Linux OS
    pthread_t tidB;
    tidB = pthread_self();
    if (pthread_equal(tId, tidB))
        return flags = WorkerThreadFlag::Start;
    else
       return flags = WorkerThreadFlag::Stop;
}
void Monitor::DoWork(void)
{
        bool keepGoing = true;
        nextSampleStartTickCount = osKernelGetTickCount();

        while (keepGoing)
        {
            nextSampleStartTickCount += mPollPeriodms;
            double nowTickCount = osKernelGetTickCount();
            double waitTimeoutms = nextSampleStartTickCount - nowTickCount;

            uint32_t flags = osThreadFlagsWait(
                WorkerThreadFlag::Start |
                WorkerThreadFlag::Stop |
                WorkerThreadFlag::Terminate,
                osFlagsWaitAny,
                mRunning ? waitTimeoutms : osWaitForever);

            bool flagsValid = (~flags & 0x80000000);

            if (flagsValid && (flags & WorkerThreadFlag::Terminate))
            {
                mRunning = false;
                keepGoing = false;
            }
            else if (flagsValid && (flags & WorkerThreadFlag::Stop))
            {
                uint32_t mClientThreadID = pthread_self();
                mRunning = false;
                osThreadFlagsSet(mClientThreadID, ClientThreadFlag::Stopped);
            }
            else
            {
                if (flagsValid && (flags & WorkerThreadFlag::Start))
                {
                    uint32_t  mClientThreadID = pthread_self();
                    mRunning = true;
                    nextSampleStartTickCount = osKernelGetTickCount();
                    osThreadFlagsSet(mClientThreadID,
                            ClientThreadFlag::Started);
                }
                PollnPublish(nowTickCount, waitTimeoutms, flags, flagsValid);
            }
      }
 }
void Monitor::PollnPublish(double y, double x, bool flags, bool flagsValid)
{
    cout <<cyan<< "\t\tDoing work"<<none<<endl;
    cout <<       "\t\t=========="<<endl << endl;
    cout << "Vars: NextSampleStartTickCount: "<<green
                  <<nextSampleStartTickCount <<none<< endl;
    cout << "Vars: mPollPeriodms: "<<green<<mPollPeriodms<<none<< endl;
    cout << "Vars: NowTickCount "<<green<<y<<none<< endl;
    cout << "Vars: WaitTimeoutms "<<green<<x<<none<< endl;
    cout << endl;
    cout << "Vars: flags "<<green<<flags<< endl;
    cout << "Vars: flagsValid "<<green<<flagsValid<<none<< endl;
    //cout << "Vars: mRunning " <<green<<mRunning<<none<< endl;
    //cout << "Vars: keepGoing "<<green<<keepGoing<<none<< endl;
    //cout << "Vars: mClientThreadID "<<green<<mClientThreadID<<none<< endl;
}

#ifndef MYMONITOR
/* Following function used with Thread object as normal calling method */
void thread_func()
{
    for (int i=0; i<10;  i++)
        cout <<"Thread function executed" <<endl;
}
#endif

int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    Q_UNUSED (argv);

#ifndef MYMONITOR
    std::thread threadObj(thread_func);
#else
    // Making a call to the class object or function object
    Monitor  one(1); // initialise with double,double 
    
    // Initialise with copy of one, using Monitor::Monitor (const Monitor)
    Monitor two = one;

    // construct Monitor(3,4)
    // directly into three
    Monitor three = Monitor (3,4);

    // Initialise with default constructor
    Monitor four;

    // Monitor (double, double) and construct directly vars in to obj
    Monitor five = 5;

    // NOTE:
    //
    // There were five objects which could be nested under the
    // C++ pthread arch to create two or all five object can be 
    // instatiated with added C++ facility of :
    //             Semaphoe, 
    //             Mutex, 
    //             Spinlock
    // to run all these five object simultenously with given priority to an 
    // object -- by ik 15/02/19


    //pthread_t t;

    //pthread_create(&t, NULL, &myobj::DoWork(), &myobj);

    one.DoWork();

    for (int i=0; i<10; i++)
                cout << "Display from Main Thread" << endl;
    cout << "Waiting for thread to complete" << endl;

    //threadObj.join();
    cout << "Exit of main function" << endl;
    return 0;
#endif
}

