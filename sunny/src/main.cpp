
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>      

#include "main.h"
#include "watchdog.h"
#include "interface_i2c.h"
#include "message.h"
#include "player.h"

static bool exit_system = false;

bool set_black_screen = false;

bool is_exit_system(void)
{
   return exit_system;
}

void do_exit_system(void)
{
   exit_system = true;
}

static void system_init(void)
{
   //Watchdog::Instance()->Start();
   Interface_i2c::Instance()->Start();
   Interface_i2c::Instance()->hdmi_discovery();
   //Interface_i2c::Instance()->switch_hdmi_port(Interface_i2c::HDMI_DVD_PORT);
   //Player::Instance()->do_video_loop();
}

static void system_close(void)
{
   Watchdog::Instance()->Stop();
   Interface_i2c::Instance()->Stop();
}


void test_thread(void)
{
   pid_t child;
 printf("%s:\n",__FUNCTION__);
   child = fork ();
   if (child != 0)
   {
      printf("%s:parent\n",__FUNCTION__);

    }
    else
    {
      printf("child process\n");
         }
    
}


int main(int argc, char **argv)
{   
   //unsigned counter = 0;
   //unsigned step = 1;
   int ch = 0;

   //long before = get_host_time_ms();
   //long diff = 0;



   printf("=== Application: AVI  ===\n");

   while ((ch =getopt(argc, argv, "CB")) != -1) 
   {
      switch(ch) 
      {
        case 'C':
           break;        
        case 'B':
            set_black_screen = true;
        default:
           break;
      }    
   }


   system_init();

   
   // main loop
   while(!exit_system)
   {
      //sleep(step);

      
      if(Message::Instance()->process()==Message::ERR)break;

#if 0
      counter +=step;
      if(!(counter%10))
      {
         diff = get_host_time_diff_ms(before);
         //printf("Running main loop....(%u) seconds so far, time diff[%ld]\n", counter,diff);
      }
#endif

   }

   system_close();   
   sleep(1);
   printf("main function exiting\n");
   return 0;
}
    


