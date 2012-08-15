#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>      
#include <sys/ioctl.h>
#include <sys/wait.h>


#include "player.h"
#include "message.h"
#include "interface_i2c.h"
#include "sam.h"

#define DEBUG_PLAYER
#ifdef DEBUG_PLAYER
#define debug_printf(X...) { printf("----->%s: ",__FUNCTION__); printf (X); printf ("\n\r"); }
#else
#define debug_printf(X...)
#endif


#if 0
static void *play_file_function (void *ptr )
{
   char local_buffer[200];
   debug_printf("play video file[%s], starting ==============\n",(char *)ptr);

   assert(strlen((char *)ptr)<180);
   sprintf(local_buffer,"gplay %s", (char *)ptr);
   system(local_buffer);
   printf("play video file, ending==============\n");
   Message::Instance()->send_command(Message::PLAY_END);
   return 0;
}
#endif

#if 0
static void *play_file_function (void *ptr )
{
   pid_t child;
   char *argv[3];
   int status;
   static char program[30];
   argv[0] = program;
   argv[1] = (char *)ptr;
   argv[2] = 0;

   strcpy(program,"gplay");
   child = fork ();
   if (child != 0)
   {
      Player::Instance()->set_pid(child);
      waitpid(child,&status,0);
      printf("play video file, ending==============\n");
      Message::Instance()->send_command(Message::PLAY_END);
      
   }
   else 
   {
      execvp (program, argv);
      printf("failed execvp\n");
      exit(0);
   }

   return 0;
}
#endif

#if 1
static void *play_file_function (void *ptr )
{
   pid_t child;
   char *argv[3];
   int status;
   static char *program = (char *)"gplay";
   argv[0] = (char *)program;
   argv[1] = (char *)ptr;
   argv[2] = 0;

   child = fork ();
   if (child != 0)
   {
      printf("child[%d]\n", child);

      Player::Instance()->set_pid(child);
      waitpid(child,&status,0);
      Player::Instance()->play_video_done((char *)ptr);
   }
   else 
   {
      execvp (program, argv);
      //sleep(10);
      printf("failed execvp\n");
      exit(0);
   }

   return 0;
}

#endif

Player::Video_loop_t Player::s_video_loop_dvd = 
{
   Player::VLT_DVD,
   "DVD",
   Player::video_loop_dvd,
   0,
   0
};

Player::Video_loop_t Player::s_video_loop_file= 
{
   Player::VLT_VIDEO_FILE,
   (const char *)"Video File",
   Player::video_loop_file,
   (void *)"loop.ts",
   Player::VF_3D
};

const char Player::s_video_dev_name[] = "/dev/sam";
 
Player::Player():
   m_current_video_format(Player::VF_2D),
   m_video_loop(&Player::s_video_loop_file),                // <== set the default video loop
   m_video_fd(-1),
   m_pid(-1)
{
   m_video_fd = open( s_video_dev_name, O_RDWR );
   if(m_video_fd<=0)
   {
      printf("Can not open video device driver\n");
   }

   reset();
}

int Player::video_loop_file(void *file, int attribute)
{
   int ret = OK;

   debug_printf("Video loop, video file==============\n");
   if(!file) return INVALID;

   Player::Instance()->play_video((char *)file,(Player::Video_format)attribute);

   return ret;
}

int Player::video_loop_dvd(void *param, int attribute)
{
   int ret = OK;
   debug_printf("Video loop, dvd==============\n");
   Interface_i2c::Instance()->switch_hdmi_port(Interface_i2c::HDMI_DVD_PORT);
   return ret;
}

int Player::do_video_loop(void)
{
   int ret = OK;
   
   if(m_video_loop)
   {
      m_video_loop->m_action(m_video_loop->m_param,m_video_loop->m_attribute);
   }
   return ret;
}


int Player::set_video_format(Video_format format)
{
   int ret = OK;
   unsigned long command;

   if(m_current_video_format==format) return ret;
   switch(format)
   {
    case VF_3D:
      if(m_video_fd>0)
      {
         debug_printf("%s: setting video to 3D format\n", __FUNCTION__);
         command = VMD_HDMIFORMAT_3D;
         ioctl(m_video_fd, SAM_SET_MODE, &command);
         m_current_video_format = format;
      }
      break;
    case VF_2D:
      if(m_video_fd>0)
      {
         debug_printf("%s: setting video to 2D format\n", __FUNCTION__);
         command = VMD_HDMIFORMAT_CEA_VIC;
         ioctl(m_video_fd, SAM_SET_MODE, &command);
         m_current_video_format = format;
      }      
      break;
    default:
      ret = INVALID;
      break;
   }
   
   return ret;
}

int Player::play_video_done(char *file_name)
{
   int ret = OK;
   printf("video file[%s], ending==============current file[%s]\n",(char *)file_name,m_current_video);

   if(!strcmp(file_name,m_current_video))
   {
      Player::Instance()->set_pid(-1);
      Message::Instance()->send_command(Message::PLAY_END);
      //sleep(5);
   }
   return ret;
}

int  Player::kill_current_video(void)
{
   int ret = OK;
   char buffer[50];
   debug_printf("kill_current_video: %s\n", m_current_video);
   if(m_pid!=-1)
   {
      reset();
      sprintf(buffer,"kill -9 %d",m_pid);
      debug_printf("Kill pid[%d],[%s]\n", m_pid,buffer);
      m_pid = -1;      
      system(buffer);
      //sleep(2);
   }   
   return ret;
}

int Player::play_video(char *file_name,Video_format format)
{
   int ret = OK;
   pthread_t thread_play_file;
   char buffer[50];


   debug_printf("Play video: %s\n", file_name);
   if((!strcmp(file_name,m_current_video))&&
      (Interface_i2c::Instance()->get_current_hdmi_port()==Interface_i2c::HDMI_AVI_PORT))
   {
      printf("Player is playing this file[%s] already\n",file_name);
      return INVALID;
   }

   //system("killall gplay");
   strcpy(m_current_video, file_name);

   if(m_pid!=-1)
   {
      sprintf(buffer,"kill -9 %d",m_pid);
      debug_printf("Kill pid[%d],[%s]\n", m_pid,buffer);
      system(buffer);
      m_pid = -1;
      
      //sleep(2);
   }
   //system(buffer);
   

   set_video_format(format);
   
   Interface_i2c::Instance()->switch_hdmi_port(Interface_i2c::HDMI_AVI_PORT);
   pthread_create (&thread_play_file, NULL, play_file_function, (void *)file_name);
   //play_file_function((void *)file_name);
   
   return ret;
}

int Player::reset(void)
{
   int ret = OK;
   memset((void *)m_current_video,0, sizeof(m_current_video));
   return ret;
}

