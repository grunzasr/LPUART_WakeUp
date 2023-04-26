/**************************************************************************//** 
* @file      console.h
* @brief     Contains code for the console task
*
******************************************************************************/

#include "tx_api.h"
#include "queue.h"

#define         MAX_Qd_CMDS     5

typedef void (*CMD_HANDLER_T)(char *);


typedef struct
{
   const char *      pCmd;
   CMD_HANDLER_T     cmd_handler;
   const char *      descr;
} CMD_INFO_T;

void
   consoleTask(
      ULONG pvParameters );

QUEUE_T *
   consoleGetCmdQ(
      void );

void cmd_help( char     *  );


#ifdef INCLUDE_FAULT_CMD
void cmd_fault( char *);
#endif

void cmd_reset( char *);
void cmd_swver( char *);
void cmd_getTick( char *);


void process_msg( char *   );


//end of file console.h
