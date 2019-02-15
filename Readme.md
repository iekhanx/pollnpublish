                                 README 
 
  The following is a thread function for a low-priority thread. It uses
  time-out of the thread-flag wait function, osThreadFlagsWait(),
  to regulate periodic monitoring performed by PollAndPublish().
 
  At what frequency will the system tick count roll over?
 
  What are the risks, if any, with the above code relating to:
    A. System tick count roll over.
    B. Periodic monitoring occasionally taking longer than mPollPeriodms.
 
  Modify the code to protect against any risks identified above.
 
  During development testing I wish to temporarily reduce the frequency
  with which periodic monitoring is performed slightly, so I add a small
  fixed offset to the waitTimeoutms calculation. Line 10 becomes:
 
  uint32_t waitTimeoutms = (nextSampleStartTickCount - nowTickCount) + 10;
 
  Does this have the desired effect? If not, why not?
 
  Why do we set ClientThreadFlag thread-flags on completion of the processing
  associated with WorkerThreadFlag::Stop and WorkerThreadFlag::Start
  thread-flags being set?
 
      3 Timer Wrapper.
  Attempt this question if you have experience with the C++ programming
  language. Write a C++ wrapper class for the timer management functionality
  described by: (try link below)
 
  http://www.keil.com/pack/doc/CMSIS/RTOS2/html/group__CMSIS__RTOS__TimerMgmt.html
 
  In addition to your class containing an appropriate number of constructors
  accepting appropriate parameters, the public interface of your class should
  include:
 
   osStatus_t  start(uint32_t ticks);
   osStatus_t  stop(void);
   bool        isRunning(void);
 
  You'll need to choose and implement a suitable approach for notifying
  objects that use instances of your class when timers expire/elapse.

