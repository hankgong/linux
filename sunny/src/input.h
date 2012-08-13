#ifndef _INPUT_H
#define _INPUT_H

#include "noncopyable.h"

class AVI_input:public Noncopyable
{
public:

   enum Button_code
   {
      BUTTON_NULL = 0,
      BUTTON_1 = 1<<0,
      BUTTON_2 = 1<<1,
      BUTTON_3 = 1<<2,
      BUTTON_4 = 1<<3,
      BUTTON_5 = 1<<4,
   };
   
   static AVI_input *Instance()
   {
      static AVI_input inst;
      return &inst;
   }

   enum Return_code
   {
      OK=0,
      ERR,
      INVALID
   };

   int dispatch(int code);
   
     
 private:
   AVI_input();
   virtual ~AVI_input(){}

   typedef int (*button_func)(void *param,int attribute);

   static int play_video_file(void *file,int attribute);
   struct Button_map
   {
      int m_code;
      const char *m_name;
      button_func m_action;
      void *m_param;
      int m_attribute;
   };

   static Button_map m_bmap_array[];
};

#endif

