
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/watchdog.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "input.h"
#include "main.h"
#include "message.h"
#include "player.h"

//#define DEBUG_AVI_INPUT

#ifdef DEBUG_AVI_INPUT
#define debug_printf(X...) { printf("----->%s: ",__FUNCTION__); printf (X); printf ("\n\r"); }
#else
#define debug_printf(X...)
#endif


// define video files to be played with button pressed
AVI_input::Button_map AVI_input::m_bmap_array[] = 
{
      {AVI_input::BUTTON_1, (const char *)"Button 1", AVI_input::play_video_file, (void *)"1.ts", Player::VF_2D},
      {AVI_input::BUTTON_2, (const char *)"Button 2", AVI_input::play_video_file, (void *)"2.ts", Player::VF_2D},
      {AVI_input::BUTTON_3, (const char *)"Button 3", AVI_input::play_video_file, (void *)"3.ts", Player::VF_2D},
      {AVI_input::BUTTON_4, (const char *)"Button 4", AVI_input::play_video_file, (void *)"4.ts", Player::VF_2D},
      {AVI_input::BUTTON_5, (const char *)"Button 5", AVI_input::play_video_file, (void *)"2.ts", Player::VF_2D},
      NULL
};

AVI_input::AVI_input()
{
   
}

int AVI_input::play_video_file(void *file,int attribute)
{
   int ret = OK;

   if(!file) return INVALID;

   Player::Instance()->play_video((char *)file,(Player::Video_format)attribute);
   
   return ret;
}

int AVI_input::dispatch(int code)
{
   int ret = OK;
   int i = 0;
   Button_map *pmap = &m_bmap_array[i];
   while(pmap->m_code!=BUTTON_NULL)
   {
      if(pmap->m_code==code)
      {
         debug_printf("Input dispatch[%d]: button[%s], code[0x%x], attribute[%d]\n",i, pmap->m_name,code,pmap->m_attribute);
         ret = pmap->m_action(pmap->m_param,pmap->m_attribute);
         break;
      }
      pmap = &m_bmap_array[++i];
   }

   return ret;
}


