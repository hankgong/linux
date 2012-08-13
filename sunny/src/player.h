#ifndef _PLAYER_H
#define _PLAYER_H

class Player
{
public:

   static Player *Instance()
   {
      static Player inst;
      return &inst;
   }

   enum Return_code
   {
      OK=0,
      ERR,
      INVALID
   };

   enum Video_format
   {
      VF_2D,
      VF_3D
   };
   
   int play_video(char *file_name, Video_format format = VF_2D);
   int play_video_done(char *file_name);
   int reset(void);   
   
   int do_video_loop(void);
   void set_pid(pid_t pid){m_pid = pid;}
   int  kill_current_video(void);
   
private:  
   Player();
   virtual ~Player(){}

   int set_video_format(Video_format format);

   enum Video_loop_type
   {
      VLT_DVD,
      VLT_VIDEO_FILE,
      VLT_UNKNOWN
   };
   
   typedef int (*video_loop_func)(void *param,int attribute);
   struct Video_loop_t
   {
      Video_loop_type m_type;
      const char *m_name;
      video_loop_func m_action;
      void *m_param;
      int m_attribute;
   };

   static int video_loop_file(void *file, int attribute);
   static int video_loop_dvd(void *param, int attribute);

   Video_format m_current_video_format;
   char m_current_video[100];
   static Video_loop_t s_video_loop_file;
   static Video_loop_t s_video_loop_dvd;
   Video_loop_t *m_video_loop;
   int m_video_fd;
   pid_t m_pid;
   static const char s_video_dev_name[100];
};

#endif

