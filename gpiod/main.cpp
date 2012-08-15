#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>      
#include <sys/ioctl.h>

#include "sam.h"

static const char dev_name[] = "/dev/sam";

static int dev_fd = -1;


static void dev_set_mode(unsigned long command)
{
   if(dev_fd>0)
   {
      printf("Set device mode:%ld\n", command);
      ioctl(dev_fd, SAM_SET_MODE, &command);
   }
}

int main(int argc, char **argv)
{   
   int ch = 0;
   unsigned long command = SAM_MODE_NONE;
   
   printf("===Device control===\n");         

   dev_fd = open( dev_name, O_RDWR );
   if(dev_fd<=0)
   {
      printf("Can not open EWDT driver\n");
      return -1;
   }
   
   while ((ch =getopt(argc, argv, "BEDS")) != -1) 
   {
      switch(ch) {
        case 'B':
           printf("===>Starting device...\n");
           command = SAM_MODE_START;
           break;
        case 'E':
           printf("===>Stopping device...\n");
           command = SAM_MODE_STOP;
           break;
        case 'D':
           printf("===>Debugging EWDT...\n");
           command = SAM_MODE_DEBUG;
           break;
        case 'S':
           printf("===>Servicing EWDT...\n");
           command = SAM_MODE_SERVICE;
           ioctl(dev_fd, SAM_SERVICE, &command);
           close(dev_fd);
           return 0;
        default:
           break;
      }    
   }

   if(command!=SAM_MODE_NONE)
   {
      sleep(1);
      dev_set_mode(command);
   }
   else
   {
      printf("===>SAM COMMAND: B/E/D/S\n");
   }

   close(dev_fd);
  
   return 0;
}
    
