
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <assert.h>

#include "message.h"
#include "fifoutil.h"
#include "main.h"
#include "interface_i2c.h"
#include "input.h"
#include "player.h"

#define DEBUG_MESSAGE

#ifdef DEBUG_MESSAGE
#define debug_printf(X...) { sleep(1);printf("----->%s: ",__FUNCTION__); printf (X); printf ("\n\r"); }
#else
#define debug_printf(X...)
#endif

int Message::initialize()
{
   key_t key =1234;

   if ((m_id = msgget(key, 0666 | IPC_CREAT)) < 0)
   {
      printf("%s: msgget failed.\n",__FUNCTION__);
      return ERR;
   }
   
   return OK;
}

Message::Message():m_id(-1)
{
   assert(initialize()==OK);
}

Message::~Message()
{
   if(m_id!=-1)
   {
      if(msgctl(m_id, IPC_RMID, NULL) == -1)
      {
         printf("%s: msgctl failed to destroy message queue.\n", __FUNCTION__);
      }
   }
      
}

int Message::send(Msg *msg)
{
   int ret = OK;
   int err;
   if((err=msgsnd(m_id, (struct msgbuf *)msg, sizeof(*msg), 0))<0)
   {
      printf("%s: failed to send message to queue.err[0x%x]\n", __FUNCTION__,err);
      return ERR;
   }

   return ret;
}
extern void test_thread(void);
int Message::process()
{
   int ret = OK;
   Msg msg_in;
   int recv_bytes;
   long before = get_host_time_ms();
   long diff = 0;

   if((recv_bytes=msgrcv(m_id, (struct msgbuf *)&msg_in, sizeof(msg_in), S_SERVER_ID, 0))!=-1) 
   {
      debug_printf("%s: message queue receive size[%d]\n",__FUNCTION__, recv_bytes);
      if (recv_bytes < (int)sizeof(Msg)) 
      {            
         return QUEUE_FAIL;
      }

      debug_printf("%s: message receive, type[%ld],source[%d],cmd[%d],sub[%d],length[%d]\n", __FUNCTION__,
         msg_in.m_type,msg_in.m_source,msg_in.m_command,msg_in.m_subcommand,msg_in.m_length);
      switch(msg_in.m_command)
      {
         case FLUSH:
            debug_printf("%s: Reading flush message\n",__FUNCTION__);
            break;
         case PRIME:
            debug_printf("%s: Reading prime message\n",__FUNCTION__);
            break;
         case EXIT:
            debug_printf("%s: Reading EXIT message\n",__FUNCTION__);
            ret = ERR;
            break;      
         case PLAY_END:
            diff = get_host_time_diff_ms(before);
            //if(diff>5000)
            {
               Player::Instance()->reset();
               Interface_i2c::Instance()->set_old_button(-1);
               //Interface_i2c::Instance()->switch_hdmi_port(Interface_i2c::HDMI_DVD_PORT);
               Player::Instance()->do_video_loop();
            }
            break;
         case BUTTON:
            debug_printf("%s: button pressed[%d]\n",__FUNCTION__,msg_in.m_subcommand);
            assert(msg_in.m_subcommand<=S_MAX_BUTTON);

            before = get_host_time_ms();

            AVI_input::Instance()->dispatch((int)msg_in.m_subcommand);
           
#if 0            
            if(msg_in.m_subcommand==1)
            {
               Player::Instance()->play_video((char *)"1.ts");
            }
            else if(msg_in.m_subcommand==2)
            {
               Player::Instance()->play_video((char *)"2.ts");
            }
            else if(msg_in.m_subcommand==4)
            {
               Player::Instance()->play_video((char *)"3.ts");
            } 
#endif

            break;
          case HDMI:
            debug_printf("HDMI switch signal[%d]\n", msg_in.m_subcommand);
            assert(msg_in.m_subcommand<=2);
            if((msg_in.m_subcommand==1)||(msg_in.m_subcommand==2))
            {
               Interface_i2c::Instance()->switch_hdmi_port(msg_in.m_subcommand);
               //Player::Instance()->do_video_loop();
            }            
            break;
         default:
            debug_printf("%s: Reading packet command[%d]\n",__FUNCTION__,msg_in.m_command);
            break;
      }

   }

   debug_printf("message done\n");
   
   return ret;
}


int Message::send_command(unsigned cmd, unsigned sub_command, int source)
{
   int ret = OK;
   Message::Msg msg;

   //sleep(1);
   debug_printf("%s: cmd[%d]\n", __FUNCTION__,cmd);
   memset((void *)&msg, 0, sizeof(msg));
   msg.m_type = 1;
   msg.m_source = source;
   msg.m_command = cmd;
   msg.m_subcommand =sub_command;
   send(&msg);   
   debug_printf("%s: cmd[%d] done\n", __FUNCTION__,cmd);
   return ret;
}



