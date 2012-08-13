#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>

char *arg_list[] = {
  (char *)"gplay",    // argv[0], the name of the program
  (char *) "2.ts",
  NULL
};


static void *play_file_function (void *ptr )
{
   pid_t child;
   char *argv[3];
   int status;
   static char *program = (char *)"gplay";
   argv[0] = (char *)program;
   argv[1] = (char *)ptr;
   argv[2] = 0;

printf("%s\n",__FUNCTION__);
   child = fork ();
   if (child != 0)
   {
      //Player::Instance()->set_pid(child);
      //waitpid(child,&status,0);
    if (wait(&status) == -1)
    {
      perror("wait()");
    }
    else
    {
      /* did the child terminate normally? */
      if(WIFEXITED(status))
      {
        printf("%ld exited with return code %d\n", (long)child, WEXITSTATUS(status));
      }
      /* was the child terminated by a signal? */
      else if (WIFSIGNALED(status))
      {
        printf("%ld terminated because it didn't catch signal number %d\n", (long)child, WTERMSIG(status));
      }
    }

      
      printf("play video file, ending==============\n");
      //Message::Instance()->send_command(Message::PLAY_END);
      
   }
   else 
   {
      execvp (program, argv);
      printf("failed execvp\n");
      exit(0);
   }

   return 0;
}


int
main (void)
{ 

pthread_t thread_play_file;
   //play_file_function((char *)"2.ts");
   pthread_create (&thread_play_file, NULL, play_file_function, (void *)"2.ts");


#if 0   
  pid_t pid = fork();

  if (pid == -1)
  {
    perror("fork()");
  }
  /* parent */
  else if (pid > 0)
  {
    int status;

    printf("Child has pid %ld\n", (long)pid);

    if (wait(&status) == -1)
    {
      perror("wait()");
    }
    else
    {
      /* did the child terminate normally? */
      if(WIFEXITED(status))
      {
        printf("%ld exited with return code %d\n",
               (long)pid, WEXITSTATUS(status));
      }
      /* was the child terminated by a signal? */
      else if (WIFSIGNALED(status))
      {
        printf("%ld terminated because it didn't catch signal number %d\n",
               (long)pid, WTERMSIG(status));
      }
    }
  }
  /* child */
  else
  {
    //sleep(10);
    execvp (arg_list[0], arg_list);
    printf("Child exiting...\n");
    exit(0);
  }
#endif

  return 0;
}
