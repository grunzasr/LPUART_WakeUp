/*
 * queue.h
 *
 *  Created on: Sep 7, 2022
 *      Author: SGrunza
 */

#ifndef INC_QUEUE_H_
#define INC_QUEUE_H_

// Define a queue that can hold 10 entries of 80 characters each
#define MAX_Q_ENTRY_LEN		80
#define MAX_Q_ITEMS		10


#define QUEUE_FAILURE		-1
#define QUEUE_SUCCESS		1

typedef char	QUEUE_ENTRY_T[MAX_Q_ENTRY_LEN];

typedef struct QUEUE_T
{
   uint16_t		head;
   uint16_t		tail;
   uint16_t		entries;
   QUEUE_ENTRY_T	item[ MAX_Q_ITEMS ];
} QUEUE_T;


int8_t	fifo_push( QUEUE_T *queue, char *msg );
int8_t	fifo_pop( QUEUE_T *queue, char *msg );
uint8_t fifo_spaceLeft( QUEUE_T *q );
void	fifo_init( QUEUE_T *queue );




#endif /* INC_QUEUE_H_ */
