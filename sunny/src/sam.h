

#ifndef DEV_SAM_H
#define DEV_SAM_H

#include <linux/ioctl.h>

enum Device_video_format
{
   VMD_HDMIFORMAT_CEA_VIC = 0,
   VMD_HDMIFORMAT_HDMI_VIC,
   VMD_HDMIFORMAT_3D,
   VMD_HDMIFORMAT_PC,
   VMD_HDMIFORMAT_DEBUG,
   VMD_HDMIFORMAT_UNKNOWN   
};

#define SAM_SET_MODE _IOW('s', 0, unsigned long)
#define SAM_SERVICE _IOW('s', 1, unsigned long)

#endif

