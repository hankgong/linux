#ifndef _INTERFACE_I2C_HEADER_H
#define _INTERFACE_I2C_HEADER_H

#include "jthread.h"
#include "noncopyable.h"
#include <fcntl.h>      

class Interface_i2c:public JThread,Noncopyable
{
 public:

   static Interface_i2c *Instance()
   {
      static Interface_i2c inst;
      return &inst;
   }

   enum Return_code
   {
      OK=0,
      ERR,
      INVALID
   };
   
   int Start();
   int Stop();     
   int switch_hdmi_port(unsigned port);
   unsigned get_current_hdmi_port(void)const {return m_current_hdmi_port;}
   int hdmi_discovery(void);
   void set_old_button(int button){m_button_old = button;}
   
   static const unsigned HDMI_AVI_PORT = 2;
   static const unsigned HDMI_DVD_PORT = 1;

     
 private:
   Interface_i2c();
   virtual ~Interface_i2c(){}
   
   /**
   * the thread function 
   */
   virtual void *Thread();     

   void run();

   int init();

   int get(const char *cmd);
   int set(const char *cmd);

   int get_pressed_button(void);

   int send_cec_code(int code);
   
   unsigned m_current_hdmi_port;
   int m_button_old;
   int m_button_dev_fd;
   fd_set m_fdcopy;

   static const char *GET_BUTTON_CMD;
   static const char *RESET_BUTTON_CMD;
   static const char *GET_HDMI_CMD;
   static const char *SET_HDMI_CMD;
   static const char *GET_A6_REGISTER_CMD;
   static const char *GET_A7_REGISTER_CMD;
   static const char *SET_CEC_CMD;
   static const char *SWITCH_HDMI_CMD1;
   static const char *SWITCH_HDMI_CMD2;
   static const char *SWITCH_HDMI_CMD3;
   static const char *SWITCH_HDMI_CMD4;
   static const char *SWITCH_HDMI_CMD5;
   static const char *SWITCH_HDMI_CMD6;
   static const char S_BUTTON_DEV_NAME[];
   
};

#endif

