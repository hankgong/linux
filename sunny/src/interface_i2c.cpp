
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include "interface_i2c.h"
#include "main.h"
#include "message.h"
#include "player.h"

//#define DEBUG_INTERFACE_I2C
extern bool set_black_screen;
#ifdef DEBUG_INTERFACE_I2C
#define debug_printf(X...) { printf("----->%s: ",__FUNCTION__); printf (X); printf ("\n\r"); }
#else
#define debug_printf(X...)
#endif

const char *Interface_i2c::GET_BUTTON_CMD = "2 0x55";
const char *Interface_i2c::RESET_BUTTON_CMD = "2 0x55 0xFF";
const char *Interface_i2c::GET_HDMI_CMD = "1 0x60 0xE0";
const char *Interface_i2c::SET_HDMI_CMD = "1 0x60 0xE0 0x01";
const char *Interface_i2c::GET_A6_REGISTER_CMD = "1 0x60 0xA6";
const char *Interface_i2c::GET_A7_REGISTER_CMD = "1 0x60 0xA7";
const char *Interface_i2c::SET_CEC_CMD = "1 0x60 0x9F ";
const char *Interface_i2c::SWITCH_HDMI_CMD1 = "1 0x60 0x88 0x0E";
const char *Interface_i2c::SWITCH_HDMI_CMD2 = "1 0x60 0x89 0x0F";
const char *Interface_i2c::SWITCH_HDMI_CMD3 = "1 0x60 0x8F 0x82";
const char *Interface_i2c::SWITCH_HDMI_CMD4 = "1 0x60 0x90 ";
const char *Interface_i2c::SWITCH_HDMI_CMD5 = "1 0x60 0x91 0x00";
const char *Interface_i2c::SWITCH_HDMI_CMD6 = "1 0x60 0x8F 0x04";


const char Interface_i2c::S_BUTTON_DEV_NAME[] = "/dev/gpiod";

Interface_i2c::Interface_i2c():m_current_hdmi_port(0xFFFF),m_button_old(-1),m_button_dev_fd(-1)
{
    FD_ZERO (&m_fdcopy);
}

int Interface_i2c::init()
{
   int ret = OK;

   m_button_dev_fd = open( S_BUTTON_DEV_NAME, O_RDWR );
   if(m_button_dev_fd<=0)
   {
      printf("Can not open driver[%s]\n",S_BUTTON_DEV_NAME);
      //return ERR;
   }
   else
   {
      FD_SET(m_button_dev_fd, &m_fdcopy);
   }
   
   set(RESET_BUTTON_CMD);
   hdmi_discovery();
   //switch_hdmi_port(Interface_i2c::HDMI_DVD_PORT);
   Player::Instance()->do_video_loop();

   return ret;
}

int Interface_i2c::Start()
{
   int ret = OK;

   debug_printf("Starting the Interface_i2c thread\n");

   // do initialization
   if((ret=init())!=OK)
   {
      printf("Failed to init application\n");
      return ret;
   }
   
   return JThread::Start();   
}

int Interface_i2c::Stop()   
{
   debug_printf("Stopping the Interface_i2c thread\n");   
   //  clean up   
   if(m_button_dev_fd>=0)close(m_button_dev_fd);
   return JThread::Kill();
}

void *Interface_i2c::Thread()
{
   JThread::ThreadStarted();
   run();

   return 0;

}

void Interface_i2c::run()
{
   unsigned counter = 0;
   const static unsigned step = 1;
   long before = get_host_time_ms();
   long diff = 0;
   int button;

   while(!is_exit_system())
   {
      //sleep(step);
      //usleep(1000);

      counter +=step;
      //if(!(counter%20))debug_printf("Running i2c interface....(%u) seconds so far\n", counter);

      //button = get(GET_BUTTON_CMD);
      button = get_pressed_button();
      diff = get_host_time_diff_ms(before);

      //if(button_old!=button) debug_
      printf("button input: 0x%x <= 0x%x, diff[%ld]\n", button,m_button_old,diff);
      if((m_button_old!=button)&&(diff>5000))
      {
         debug_printf("button input: 0x%x <= 0x%x, diff[%ld]\n", button,m_button_old,diff);
         if(button==0)
         {
         }
         else if(button<=(int)Message::S_MAX_BUTTON)
         {
            if(set_black_screen)Player::Instance()->kill_current_video();
            Message::Instance()->send_command(Message::BUTTON,button,Message::I2C);
         }
#if 0         
         else if(button<=16)
         {
            Message::Instance()->send_command(Message::HDMI,button/8,Message::I2C);
         }
#endif         
         before = get_host_time_ms();
         m_button_old = button;
      }
      else if(m_button_old==button)
      {
          if(m_button_dev_fd>=0)before = get_host_time_ms();
      }
      
   }

   Stop();
   debug_printf("interface_i2c exiting\n");
}


int Interface_i2c::get_pressed_button(void)
{
   int ret = 0;
   fd_set fdset = m_fdcopy;
   char buf[10];

   if(m_button_dev_fd>=0)
   {
      select(FD_SETSIZE, &fdset, NULL, NULL, NULL);
      
      if (FD_ISSET (m_button_dev_fd, &fdset) > 0) 
      {
         debug_printf("button pressed\n");
         ret = get(GET_BUTTON_CMD);
         read(m_button_dev_fd,buf,10);
      }
   }
   else
   {
      usleep(1000);
      sleep(1);
      ret = get(GET_BUTTON_CMD);
   }
   
   return ret;
}

int Interface_i2c::get(const char *cmd)
{
   int ret = 0;
   assert(cmd);
   char cmd_buffer[200];
   char local_buffer[200];
   assert(strlen(cmd)<150);

   strcpy(cmd_buffer,"i2cget -y ");
   strcat(cmd_buffer,cmd);
   //debug_printf("do get cmd[%s]\n", cmd_buffer);
   FILE *p = popen(cmd_buffer, "r");
   
   if(p != NULL) 
   {
      while(fgets(local_buffer, sizeof(local_buffer), p) != NULL) 
      {
         //debug_printf("output:\n %s\n", local_buffer);
         //ret = atoi(local_buffer+2);
         ret = strtol(local_buffer, (char**) NULL, 16);
      }
   }
      
   return ret;
}

int Interface_i2c::set(const char *cmd)
{
   int ret = OK;
   assert(cmd);
   char cmd_buffer[200];
   assert(strlen(cmd)<150);

   strcpy(cmd_buffer,"i2cset -y ");
   strcat(cmd_buffer,cmd);
   debug_printf("do set cmd[%s]\n", cmd_buffer);

   system(cmd_buffer);
   return ret;
}

int Interface_i2c::hdmi_discovery(void)
{
   int ret = OK;
   int counter = 0;

   int hdmi_value = get(GET_HDMI_CMD);
   debug_printf("hdmi discovery: 0x%x\n",hdmi_value);
   if(hdmi_value!=0x80)
   {
      debug_printf("------------HDMIDiscovery------------");
      set(SET_HDMI_CMD);

      while((get(GET_HDMI_CMD)!=0x80)&&(counter<=10))
      {
         hdmi_value = get(GET_HDMI_CMD);
         debug_printf("hdmi discovery: 0x%x, counter[%d]\n",hdmi_value,counter);
         //usleep(500000);
         sleep(1);
         ++counter;       
      }

      debug_printf("_________HDMIDiscovery DONE__________");
   }

   return ret;
}

int Interface_i2c::send_cec_code(int code)
{
   int ret = OK;
   unsigned counter = 0;
   int reg_a6 = get(GET_A6_REGISTER_CMD);
   int reg_a7 = get(GET_A7_REGISTER_CMD);
   char local_buffer[100];
   
   while((reg_a6==0xF7)||(reg_a6==0x77))
   {
      usleep(50000);
      reg_a6 = get(GET_A6_REGISTER_CMD);
      reg_a7 = get(GET_A7_REGISTER_CMD);
      debug_printf("reg: a6[0x%x], a7[0x%x]\n", reg_a6,reg_a7);
      ++counter;
      if(counter>10) break;
   }

   reg_a6 = get(GET_A6_REGISTER_CMD);
   reg_a7 = get(GET_A7_REGISTER_CMD);
   debug_printf("reg: a6[0x%x], a7[0x%x]\n", reg_a6,reg_a7);

   sprintf(local_buffer,"%s 0x%x",SET_CEC_CMD,code);
   set(local_buffer);

   return ret;
}

int Interface_i2c::switch_hdmi_port(unsigned port)
{
   int ret = OK;
   char local_buffer[100];

   printf("------------SWITCHING TO HDMI %d------------\n",port);

   //if(port==m_current_hdmi_port) return INVALID;

   m_current_hdmi_port = port;
   hdmi_discovery();
   set(SWITCH_HDMI_CMD1);
   set(SWITCH_HDMI_CMD2);
   set(SWITCH_HDMI_CMD3);

   sprintf(local_buffer,"%s 0x%x",SWITCH_HDMI_CMD4, 16 * port);
   set(local_buffer);
   set(SWITCH_HDMI_CMD5);

   debug_printf("Sending active source\n");
   send_cec_code(0x12);
   set(SWITCH_HDMI_CMD6);

   debug_printf("Sending Image view on\n");
   send_cec_code(0x20);

   return ret;
}

