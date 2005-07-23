#ifndef CAN_H
#define CAN_H

/**
 * Simple CAN Library
 *
 * #define CAN_INTERRUPT 1	//set this to enable interrupt driven and buffering version
 * #define CAN_RX_BUFFER_SIZE 2	//only used for Interrupt
 * #define CAN_TX_BUFFER_SIZE 2	//only used for Interrupt
 */

typedef struct{
	unsigned char addr_src;
	unsigned char addr_dest;
	unsigned char port_src;
	unsigned char port_dest;
	unsigned char dlc;
	unsigned char data[8];
}can_message;
typedef enum { normal, sleep, loopback, listenonly, config } can_mode_t ;

/* CAN */ 
void  can_init();

can_message * can_buffer_get();
void can_transmit( can_message *msg );

can_message *can_get();
can_message *can_get_nb();
void can_setfilter();
void can_setmode(can_mode_t);

#endif
