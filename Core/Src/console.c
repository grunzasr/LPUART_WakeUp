/**************************************************************************//** 
 * @file      console.c
 * @brief     Contains code for the console task
 *
 ******************************************************************************/
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "main.h"
#include "app_threadx.h"

#include "usart.h"

//#define	INCLUDE_FAULT_CMD

#include "console.h"
#include "gpio.h"
#include "queue.h"


// Command queue
QUEUE_T    	consoleCmdQ;

// Defined in main.c
extern char DBG_msg_buf[ DBG_MSG_LEN ];

#define MAX_CONSOLE_OUT_MSG_LEN		80

extern TX_THREAD		consoleThread;
extern TX_THREAD		uartThread;



const CMD_INFO_T        gCommands[] =
{
		{ "?",              	cmd_help,	(const char *)"Show available commands" },
#ifdef INCLUDE_FAULT_CMD
		{ "fault",		cmd_fault,	(const char *)"Cause a hard fault"},
#endif
		{ "getTick",		cmd_getTick,	(const char *)"Show current system tick" },
		{ "help",           	cmd_help,	(const char *)"Show available commands" },
		{ "reset",				cmd_reset,	(const char *)"Reset board"},
		{ "swver",				cmd_swver,	(const char *)"Get software version information"},
		{ NULL, NULL, NULL }
};

#define	TICKS_BEFORE_SLEEP	100
//*********************************************************************
// console task
//
// Console task has a message Q that gets sent ASCII text message
// commands straight from the receiving UART.
//
//*********************************************************************
void
consoleTask(
		ULONG   ulParameters )
{

	UINT		ret;
	uint8_t		cmd[ MAX_MSG_LEN + 2 ];	// room for \r and \0
	uint8_t		loops;
	uint32_t	currentTime;
	uint32_t	timeToSleep;

	// Initialize console command queue
	fifo_init( &consoleCmdQ );


	// Clear screen
	for( loops = 0; loops < 20; loops++ )
	{
		vSerialPutString( "\n" );
	}

	vSerialPutString(" STM32U5xx LPUART STOP2 Demo " );
	vSerialPutString( BuildInfoString );
	vSerialPutString( "\r\n" );

#ifdef ENABLE_SLEEPING
	vSerialPutString( "Calls to tx_thread_sleep() are enabled\r\n");
#else
	vSerialPutString( "Calls to tx_thread_sleep() are disabled\r\n");
#endif
	vSerialPutString( "\r\n\r\n" );

	cmd_help( "help" );

	vSerialPutString( "\r\n> " );


	while( 1 )
	{
		if( consoleCmdQ.entries > 0 )
		{
			// Found something so restart the idle count
			timeToSleep = HAL_GetTick() + TICKS_BEFORE_SLEEP;


			ret = fifo_pop( &consoleCmdQ,	// ptr to queue
					        (char *)cmd);	// ptr into which to put extracted command
			if( ret == QUEUE_SUCCESS )
			{
				// There is a message to process so there is a moment to check the
				 // system clock and see if it needs to be restored

				 SystemCoreClock = HAL_RCC_GetSysClockFreq() >> AHBPrescTable[(RCC->CFGR2 & RCC_CFGR2_HPRE) >> RCC_CFGR2_HPRE_Pos];
				 if( SystemCoreClock != SYSCLK_FREQ )
				 {
				    // Restore the SYSCLK to full speed
				    SystemClock_Restore();
				 }

				if( cmd[0] != '\0' )
				{
					vSerialPutString( "\r\n" );
					// Process command if there is a command to process
					process_msg( (char *)cmd );

					memset( cmd, 0, MAX_MSG_LEN );

					vSerialPutString( "\r\n> " );
				}
			}
		}
		else
		{
			currentTime = HAL_GetTick();
			if( currentTime > timeToSleep )
			{
#ifdef	ENABLE_SLEEPING
				tx_thread_suspend( &consoleThread );	// need to restart with tx_thread_resume()
#endif
			}
		}
	}

	return;
} // end of consoleTask()


//*********************************************************************
//
// Return the address of the command queue
//
//*********************************************************************
QUEUE_T *
consoleGetCmdQ(
		void )
{
	return &consoleCmdQ;
} // consoleGetCmdQ()


//*********************************************************************
void cmd_help( char     *msg )
{

	const CMD_INFO_T     *pCmdInfo;
	char *c;

	if( msg == NULL )
	{
		return;
	}

	// make the whole string lower case so later processing is easier
	c = msg;
	while( *c != '\0' )
	{
		*c = tolower(*c);
		c++;
	}

	c = msg;

	// might be pointing at a ? or an h

	// move to the end of the current token
	while( (*c != '\0') && (*c != ' ') )
	{
		c++;
	}

	// Check if "help" or "?" is the only thing on the line
	if( *c == '\0' )
	{
		// No more arguments so display first level help
		pCmdInfo = gCommands;

		while( pCmdInfo->cmd_handler != NULL )
		{
			vSerialPutString( (char *)pCmdInfo->pCmd );
			vSerialPutString( "\t - " );
			vSerialPutString( (char *)pCmdInfo->descr );
			vSerialPutString( "\r\n" );
			pCmdInfo++;
		}
		vSerialPutString( "\r\n\r\n" );
	}

	return;
}

//*********************************************************************
void process_msg( char *msg )
{
	const CMD_INFO_T     *pCmdInfo;
	char                 *pFound;


	// Search the list of known commands to find this message's command
	pCmdInfo = gCommands;

	while( pCmdInfo->cmd_handler != NULL )
	{
		pFound = strstr( msg, pCmdInfo->pCmd );
		if( pFound == msg )
		{
			// This command was found at the beginning of the msg so process it
			vSerialPutString( "\r\n" );
			pCmdInfo->cmd_handler( msg );
			break;
		}
		else
		{
			pCmdInfo++;
		}
	}

	if( pCmdInfo->cmd_handler == NULL )
	{
		// Got to the end
		vSerialPutString( "Unknown command: " );
		vSerialPutString( msg );
		vSerialPutString( "\r\n" );
	}

	return;
} // end of process_msg()

#ifdef INCLUDE_FAULT_CMD
//*********************************************************************
void cmd_fault( char     *msg )
{
	CMD_INFO_T		badCmd;

	badCmd.descr = "bad command";
	badCmd.pCmd  = "fubar";
	badCmd.cmd_handler = (CMD_HANDLER_T *)0xFFFFFFF0;

	badCmd.cmd_handler( (char *) badCmd.descr );

	return;
} // end of cmd_fault()
#endif	// INCLUDE_FAULT_CMD

//*********************************************************************
void cmd_reset( char     *msg )
{

	NVIC_SystemReset();

	return;
} // end of cmd_reset()

//*********************************************************************
void cmd_swver( char     *msg )
{

	vSerialPutString( SoftwareNameString );
	vSerialPutString( "\r\n" );

	vSerialPutString( "Version: " );
	vSerialPutString( SoftwareVersionString );
	vSerialPutString( "\r\n" );

	vSerialPutString( "ICD Version: " );
	vSerialPutString( ICDversionString );
	vSerialPutString( "\r\n" );

	vSerialPutString( BuildInfoString );
	vSerialPutString( "\r\n" );

#ifdef ENABLE_SLEEPING
	vSerialPutString( "Calls to tx_thread_sleep() are enabled\r\n");
#else
	vSerialPutString( "Calls to tx_thread_sleep() are disabled\r\n");
#endif


	return;
} // end of cmd_swver()

//*********************************************************************
void cmd_getTick( char     *msg )
{
	uint32_t		current_tick;

	current_tick = HAL_GetTick();

	snprintf( DBG_msg_buf, DBG_MSG_LEN,
			"Current tick is %ld (0x%8.8lx)\r\n",
			current_tick,
			current_tick );
	vSerialPutString( DBG_msg_buf );
	return;
} // end of cmd_getTick()



//end of file console.c
