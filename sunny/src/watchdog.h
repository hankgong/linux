#ifndef _WATCHDOG_HEADER_H
#define _WATCHDOG_HEADER_H

#include "jthread.h"
#include "noncopyable.h"


class Watchdog:public JThread,Noncopyable
{
 public:

   static Watchdog *Instance()
   {
      static Watchdog inst;
      return &inst;
   }

   enum Return_code
   {
      OK=0,
      ERR,
      INVALID
   };
   
   int Start();
   int Stop();     

     
 private:
   Watchdog();
   virtual ~Watchdog(){}
   
   /**
   * the thread function 
   */
   virtual void *Thread();     

   void run();

   int init();

   int m_dog;
   static const unsigned M_KEEP_ALIVE_INTERVAL =  50;
   static const unsigned M_KEEP_ALIVE_MARGIN =  30;
};

#endif

