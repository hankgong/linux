
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/watchdog.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "watchdog.h"
#include "main.h"

//#define DEBUG_WATCHDOG

#ifdef DEBUG_WATCHDOG
#define debug_printf(X...) { printf("----->%s: ",__FUNCTION__); printf (X); printf ("\n\r"); }
#else
#define debug_printf(X...)
#endif


Watchdog::Watchdog():m_dog(-1)
{
}

int Watchdog::init()
{
   int ret = OK;
   int interval = 0;

   m_dog = open("/dev/watchdog",O_NONBLOCK,O_RDWR);
   if(m_dog<0) 
   {
      printf("Failed to enable watchdog\n");
      ret = ERR;
   }
   else
   {
      if(ioctl(m_dog, WDIOC_GETTIMEOUT, &interval)==0)
      {
         debug_printf("Failed to get current watchdog intervale[%d]\n", interval);
      }

      debug_printf("current watchdog intervale[%d]\n", interval);
      interval = M_KEEP_ALIVE_INTERVAL+M_KEEP_ALIVE_MARGIN;
      debug_printf("New watchdog intervale[%d]\n", interval);
#if 0      
      if(ioctl(m_dog, WDIOC_SETTIMEOUT, &interval)!=0)
      {
         ret = ERR;
         printf("Failed to set the watchdog interval\n");
      }
#endif      
   }
   return ret;
}

int Watchdog::Start()
{
   int ret = OK;

   debug_printf("Starting the Watchdog thread\n");

   // do initialization
   if((ret=init())!=OK)
   {
      printf("Failed to init watchdog timer\n");
      return ret;
   }

   return JThread::Start();   
}

int Watchdog::Stop()   
{
   debug_printf("Stopping the Watchdog thread\n");   
   //  clean up   
   
   return JThread::Kill();
}

void *Watchdog::Thread()
{
   JThread::ThreadStarted();
   run();

   return 0;

}

void Watchdog::run()
{
   unsigned counter = 0;
   int dummy;
   const static unsigned step = M_KEEP_ALIVE_INTERVAL;

   while(!is_exit_system())
   {

      sleep(step);
     
      ioctl(m_dog, WDIOC_KEEPALIVE, &dummy);

      counter +=step;
      if(!(counter%20))debug_printf("Running watchdog....(%u) seconds so far\n", counter);
   }

   debug_printf("watchdog exiting\n");
}


