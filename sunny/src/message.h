#ifndef _MESSAGE_HEADER_H
#define _MESSAGE_HEADER_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "fifoutil.h"

class Message
{
 public:

   typedef enum 
   {
      TEST,
      I2C,
      WATCHDOG,
      UNKNOWN
   }Sources;

   typedef enum 
   {
      FLUSH,
      PRIME,
      RESET,
      EXIT,
      PLAY_END,
      PLAY_FILE,
      BUTTON,
      HDMI
   }Commands;


   typedef struct 
   {
      long m_type;
      int m_source;
      unsigned m_command;
      unsigned m_subcommand;
      unsigned m_length;
      char m_buffer[256];
   } Msg;   

   static Message *Instance()
   {
      static Message inst;
      return &inst;
   }

   enum Return_code
   {
      OK=0,
      ERR,
      INVALID,
      QUEUE_FAIL
   };
   
   int process();
   int send(Msg *msg);
   int send_command(unsigned cmd, unsigned sub_command = 0, int source = UNKNOWN);

   static const unsigned S_MAX_BUTTON = 16;
     
 private:
   Message();
   virtual ~Message();

   int initialize();

   int m_id;

   static const int S_SERVER_ID = 1;

};

#endif

