

#ifndef DEV_GPIOD_H
#define DEV_GPIOD_H

#include <linux/ioctl.h>

enum Gpiod_mode
{
   GPIOD_MODE_NONE = 0,
   GPIOD_MODE_START,
   GPIOD_MODE_PAUSE,
   GPIOD_MODE_STOP,
   GPIOD_MODE_SERVICE,
   GPIOD_MODE_DEBUG
};


#define GPIOD_SET_MODE _IOW('g', 0, unsigned long)
#define GPIOD_SERVICE _IOW('g', 1, unsigned long)

#endif

