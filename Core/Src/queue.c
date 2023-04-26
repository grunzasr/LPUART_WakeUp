/*
 * queue.c
 *
 *  Created on: Sep 7, 2022
 *      Author: SGrunza
 */

#include "main.h"
#include "queue.h"
#include "usart.h"
#include <string.h>


// Defined in main.c
extern char		DBG_msg_buf[];

//*********************************************************************
void
   fifo_init(
      QUEUE_T 		*queue )
{

   if( queue != NULL )
   {
      memset( queue, 0, sizeof(QUEUE_T) );
   }

   return;
} // end of fifo_init()

//*********************************************************************
uint8_t
   fifo_spaceLeft(
      QUEUE_T 		*queue )
{
   uint8_t	spaceLeft;


   if( queue->entries < MAX_Q_ITEMS )
   {
      spaceLeft = 1;
   }
   else
   {
      spaceLeft = 0;
   }


   return( spaceLeft );
} // end of fifo_init()

//*********************************************************************
int8_t
   fifo_push(
      QUEUE_T 		*queue,
      char 		*msg )
{

   int8_t	result;
   uint16_t	dataLen;
   uint16_t	nextIndex;

   // It works until it doesn't
   result = QUEUE_SUCCESS;
   dataLen = MAX_Q_ENTRY_LEN + 1;

   if( queue == NULL )
   {
      result = QUEUE_FAILURE;
   }
   else
   {
      if( msg == NULL )
      {
	 result = QUEUE_FAILURE;
      }
      else
      {
	 dataLen = strlen( msg );
	 if( dataLen > MAX_Q_ENTRY_LEN )
	 {
	    result = QUEUE_FAILURE;
	 }
	 else
	 {
	    if( !fifo_spaceLeft( queue ) )
	    {
	       result = QUEUE_FAILURE;
	    }
	 }
      }
   }

   if( result == QUEUE_SUCCESS )
   {
      // If there is space left then the head ptr is ok for writing

      // Clear the entry
      memset( queue->item[ queue->head ], 0, MAX_Q_ENTRY_LEN );

      // Put data into entry
      memcpy( queue->item[ queue->head ], msg, dataLen );
      queue->entries++;

      nextIndex = (queue->head + 1) % MAX_Q_ITEMS;

      queue->head = nextIndex;
   }

   return( result );
} // end of fifo_push()

//*********************************************************************
int8_t
   fifo_pop(
      QUEUE_T 		*queue,
      char 		*dst )
{
   int8_t	result;
   uint16_t	dataLen;
   char		*src;

   // It works until it doesn't
   result = QUEUE_SUCCESS;

   if( queue == NULL )
   {
      result = QUEUE_FAILURE;
   }
   else
   {
      if( dst == NULL )
      {
	 result = QUEUE_FAILURE;
      }
      if( queue->entries == 0 )
      {
	 // Nothing to pop
	 result = QUEUE_FAILURE;
      }
   }

   if( result == QUEUE_SUCCESS )
   {
      src = queue->item[ queue->tail ];
      dataLen = strlen( src );
      memcpy( dst, src, dataLen );
      queue->entries--;
      queue->tail = (queue->tail + 1) % MAX_Q_ITEMS;
   }

   return( result );
} // end of fifo_pop()



// end of file queue.c
