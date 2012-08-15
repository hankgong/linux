#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>      
#include <sys/ioctl.h>

#include "gpiod.h"

static const char dev_name[] = "/dev/gpiod";

static int dev_fd = -1;


int main(int argc, char **argv)
{   
   fd_set fdset,fdcopy;   
   ssize_t recv_bytes;
   FD_ZERO (&fdcopy);
   char buf[100];
   
   printf("===Device query===\n");         

   dev_fd = open( dev_name, O_RDWR );
   if(dev_fd<=0)
   {
      printf("Can not open EWDT driver\n");
      return -1;
   }

   printf("dev fd[%d]\n", dev_fd);
   FD_SET(dev_fd, &fdcopy);

   while(1)
   {
      fdset = fdcopy;
      select(FD_SETSIZE, &fdset, NULL, NULL, NULL);
      
      if (FD_ISSET (dev_fd, &fdset) > 0) 
      {
         printf("device active\n");
         //sleep(1);
         read(dev_fd,buf,100);      
      }
   }

   close(dev_fd);
  
   return 0;
}
    
