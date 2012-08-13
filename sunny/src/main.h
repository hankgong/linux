#ifndef _MAIN_HEADER_H
#define _MAIN_HEADER_H

#include <time.h>

bool is_exit_system(void);
void do_exit_system(void);


inline unsigned long get_host_time_ms()
{
    struct timespec tp;

    if(clock_gettime(CLOCK_MONOTONIC, &tp) == 0) {
        unsigned long long real_host_time =
            ((unsigned long)tp.tv_sec * 1000) +
            ((unsigned long)(tp.tv_nsec/1000000));
         return real_host_time;
    }
    return 0UL;
}


inline unsigned get_host_time_diff_ms(unsigned before)
{
   unsigned now = (unsigned)get_host_time_ms();

   return (now>=before)?(now-before):((~((unsigned)0)-before)+now);
   
}

#endif

