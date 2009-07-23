/**** RFM 12 library for Atmel AVR Microcontrollers *******
 * 
 * This software is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA.
 *
 * @author Peter Fuhrmann, Hans-Gert Dahmen, Soeren Heisrath
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

#include "rfm12_hw.h"
#include "rfm12.h"

#if RFM12_UART_DEBUG
	#include "test/uart.h"
#endif

//evil hack
#ifdef RFM12_TRANSMIT_ONLY
#undef RFM12_PWRMGT_ER
#define RFM12_PWRMGT_ER 0
#endif


//default value for powermanagement register
//here it can be selected, if we want the reset generator or the
//oscillator or the wakeup timer to be active
#ifndef PWRMGT_DEFAULT
#define PWRMGT_DEFAULT (RFM12_PWRMGT_DC)
#endif

//default preamble (altrernating 1s and 0s)
#define PREAMBLE 0xAA

//default synchron pattern
#define SYNC_MSB 0x2D
#define SYNC_LSB 0xD4

/*
	Begin of library
*/

#define SS_ASSERT() PORT_SS &= ~(1<<BIT_SS)
#define SS_RELEASE() PORT_SS |= (1<<BIT_SS)

#define RFM12_INT_ON() RFM12_INT_MSK |= (1<<RFM12_INT_BIT)
#define RFM12_INT_OFF() RFM12_INT_MSK &= ~(1<<RFM12_INT_BIT)


//default fiforeset is as follows:
//2 Byte Sync Pattern,
//disable sensitive reset, Fifo filled interrupt at 8 bits
//default fiforeset register value to accept data
#define ACCEPT_DATA (RFM12_CMD_FIFORESET | RFM12_FIFORESET_DR | (8<<4) | RFM12_FIFORESET_FF)
#define ACCEPT_DATA_INLINE (RFM12_FIFORESET_DR | (8<<4) | RFM12_FIFORESET_FF)
//default fiforeset register value to clear fifo
#define CLEAR_FIFO (RFM12_CMD_FIFORESET | RFM12_FIFORESET_DR | (8<<4))
#define CLEAR_FIFO_INLINE (RFM12_FIFORESET_DR | (8<<4))

#if (RFM12_NORETURNS)
	#define TXRETURN(a)
#else
	#define TXRETURN(a) (a)
#endif

//Buffer and status for the message to be transmitted
rf_tx_buffer_t rf_tx_buffer;

//buffer and status for the message to be received
rf_rx_buffer_t rf_rx_buffer;

//mode we are in - rx or tx
#define MODE_RX 0
#define MODE_TX 1
#define MODE_RAW 2
uint8_t rfm12_mode;

/* include spi functions into here */
#include "rfm12_spi.c"

/* include control / init functions into here */
#include "rfm12_ctrl.c"

/* include raw transceiving functions here */
#include "rfm12_raw.c"


void rfm12_reset_fifo()
{
	rf_rx_buffer.status = STATUS_IDLE;
	rfm12_data_inline(RFM12_CMD_FIFORESET>>8, CLEAR_FIFO_INLINE);
	rfm12_data_inline(RFM12_CMD_FIFORESET>>8, ACCEPT_DATA_INLINE);
}


/*
	interrupt handler for interrupts from rfm12
	
	The receiver will generate an interrupt request (IT) for the
	microcontroller - by pulling the nIRQ pin low - on the following events:

	* The TX register is ready to receive the next byte (RGIT)
	* The FIFO has received the preprogrammed amount of bits (FFIT)
	* Power-on reset (POR) *
	* FIFO overflow (FFOV) / TX register underrun (RGUR) *
	* Wake-up timer timeout (WKUP) *
	* Negative pulse on the interrupt input pin nINT (EXT) *
	* Supply voltage below the preprogrammed value is detected (LBD) *
	*these shouldn't occur - we'll just ignore them. These flags are cleared
	 by reading status.
*/


ISR(RFM12_INT_VECT, ISR_NOBLOCK)
{
	RFM12_INT_OFF();
	uint8_t status;

	//first we read the first byte of the status register
	//to get the interrupt flags
	status = rfm12_read_int_flags_inline();

#ifdef RFM12_USE_WAKEUP_TIMER
	if(status & (RFM12_STATUS_WKUP>>8))
	{
		
		//FIXME: crude aproach of using rfm12_mode to decide what to write to 
		//PWRMGT register.
		//should be changed to using a shadow var for the PWRMGT state
		if(rfm12_mode == MODE_RX)
		{
			rfm12_data(RFM12_CMD_PWRMGT | (PWRMGT_DEFAULT & ~RFM12_PWRMGT_EW) | RFM12_PWRMGT_ER );
			rfm12_data(RFM12_CMD_PWRMGT |  PWRMGT_DEFAULT                     | RFM12_PWRMGT_ER );
		}else
		{
			rfm12_data(RFM12_CMD_PWRMGT | (PWRMGT_DEFAULT & ~RFM12_PWRMGT_EW) | RFM12_PWRMGT_ET );
			rfm12_data(RFM12_CMD_PWRMGT |  PWRMGT_DEFAULT                     | RFM12_PWRMGT_ET );
		}
	}
#endif
	
	//check if the fifo interrupt occurred
	if((!status & (RFM12_STATUS_FFIT>>8)))
		goto bail_out;

#if RFM12_UART_DEBUG >= 2
		uart_putc('F');
#endif
	if (rfm12_mode == MODE_TX) /* ==== TX ====*/
	{
		//the fifo interrupt occurred, and we are in TX mode,
		//so the fifo wants the next byte to TX.

#if RFM12_UART_DEBUG >= 2
		uart_putc('T');
#endif
		
		if(rf_tx_buffer.bytecount < rf_tx_buffer.num_bytes)
		{
			//load the next byte from our buffer struct.
			rfm12_data_inline( (RFM12_CMD_TX>>8), rf_tx_buffer.sync[rf_tx_buffer.bytecount++]);
		} else
		{
			//o.k. - so we are finished transmitting the bytes
			
			//flag the buffer as free again
			rf_tx_buffer.status = STATUS_FREE;
			
			//we are now in rx mode again
			rfm12_mode = MODE_RX;
			
			//turn off the transmitter
			//and enable receiver
			rfm12_data(RFM12_CMD_PWRMGT | PWRMGT_DEFAULT | RFM12_PWRMGT_ER);
			
			//load a dummy byte to clear int status.
			rfm12_data_inline( (RFM12_CMD_TX>>8), 0xaa);
			
			rfm12_reset_fifo();
			goto bail_out;
		}
	} else   /* === RX === */ 
	{
		static uint8_t checksum;

#if RFM12_UART_DEBUG >= 2
		uart_putc('R');
		uart_putc(rfm12_read_fifo_inline());
#endif
	
		/* check if we're receiving a new transmission or the next byte of
		 * an active transmission
		 *
		 * FIXME: this could cause problems, b/c we could be ignoring a
		 * transmission if the buffer is in STATUS_COMPLETE
		 *
		 * FIXME: this in turn could lead to a false sync byte recognition
		 * in the middle of a transfer
		 * 
		 * FIXME: we should silently clock in the transmission without
		 * writing it to the buffer OR apply double buffering
		 */
		switch(rf_rx_buffer.status)
		{
			case STATUS_IDLE:
			{
				rf_rx_buffer.bytecount = 1;

				/* read first byte, which is the length byte */
				checksum = rfm12_read_fifo_inline();
				rf_rx_buffer.num_bytes = checksum + 3;
				
				if(rf_rx_buffer.rf_buffer_in->status == STATUS_FREE)
				{
					//the current receive buffer is empty, so we start receiving
					rf_rx_buffer.status = STATUS_RECEIVING;
				
					rf_rx_buffer.rf_buffer_in->len = checksum;//hackhack: checksum holds length
				} else
				{
					//the buffer is full, so we ignore this transmission.
					rf_rx_buffer.status = STATUS_IGNORING;
				}
				break;
			}
			
		//transfer is active, continue receipt
			case STATUS_RECEIVING:
			{
				//check if transmission is complete
				if(rf_rx_buffer.bytecount < rf_rx_buffer.num_bytes)
				{
					uint8_t data;
					data = rfm12_read_fifo_inline();
					checksum ^= data;
					
					//put next byte into buffer, if there is enough space
					if(rf_rx_buffer.bytecount < (RFM12_RX_BUFFER_SIZE + 3))
					{
						//hackhack: begin writing to struct at offsetof len
						(& rf_rx_buffer.rf_buffer_in->len)[rf_rx_buffer.bytecount] = data;
					}
					
					//check crc
					if (rf_rx_buffer.bytecount == 2 && checksum != 0xff)
					{
						rfm12_reset_fifo();
					}

					//increment bytecount
					rf_rx_buffer.bytecount++;
				} else
				{
					uint8_t num;
					
					//indicate that the Buffer is ready to be used
					rf_rx_buffer.rf_buffer_in->status = STATUS_COMPLETE;
					
					//switch to other buffer
					num = rf_rx_buffer.buffer_in_num;
					num = (num+1) % 2;
					rf_rx_buffer.buffer_in_num = num;

					rf_rx_buffer.rf_buffer_in = &rf_rx_buffer.rf_buffers[num];

					rfm12_reset_fifo();
				}
				break;
			}
			case STATUS_IGNORING:
			{
				//FIXME: we don't check the checksum if we ignore a package.
				//that could mean that we ignore a wrong number of bytes, but it shouldn't
				//hurt in practice
				
				//check if transmission is complete
				if(rf_rx_buffer.bytecount < rf_rx_buffer.num_bytes)
				{
					//remove byte from fifo so interrupt condition is cleared.
					rfm12_read_fifo_inline();
				} else
				{
					rfm12_reset_fifo();
				}
				break;
			}
		}
	}
	
	bail_out:
	RFM12_INT_ON();
}


void rfm12_tick()
{
	uint16_t status;
#if !(RFM12_NOCOLISSIONDETECTION)
	static uint8_t channel_free_count = 16;
#endif

#if RFM12_UART_DEBUG
	static uint8_t oldmode;
	uint8_t mode = rfm12_mode;
	if (oldmode != mode)
	{
		uart_putstr ("mode change: ");
		switch (mode)
		{
			case MODE_RX:
				uart_putc ('r');
			break;
			case MODE_TX:
				uart_putc ('t');
			break;
			default:
				uart_putc ('?');
		}
		oldmode = mode;
	}
#endif

	//don't disturb RFM12 if transmitting
	if(rfm12_mode == MODE_TX || rfm12_mode == MODE_RAW)
		return;

#if !(RFM12_NOCOLISSIONDETECTION)
	//disable the interrupt (as we're working directly with the transceiver now)
	RFM12_INT_OFF();
		
	status = rfm12_read(RFM12_CMD_STATUS);
	
	RFM12_INT_ON();

	//check if we see a carrier
	if(status & RFM12_STATUS_RSSI)
	{
		//yes: reset free counter
		channel_free_count = 200;
	}else
	{
		//no: decrement counter
		channel_free_count--;
		//is the channel free long enough ?
		if(channel_free_count == 0)
		{
			channel_free_count = 1;
#endif

			//do we have something to transmit?
			if(rf_tx_buffer.status == STATUS_OCCUPIED)
			{
				//yes: start transmitting
				
				//disable the interrupt (as we're working directly with the transceiver now)
				RFM12_INT_OFF();
				
				//disable receiver - is this needed?
				rfm12_data(RFM12_CMD_PWRMGT | PWRMGT_DEFAULT);
			
				//set mode for interrupt handler
				rfm12_mode = MODE_TX;
				
				//fill 2byte 0xAA preamble into data register
				//this is explicitly done, just to be sure
				//(hint: the tx FIFO [if el is enabled] is two staged, so we can safely write 2 bytes before starting)
				rfm12_data(RFM12_CMD_TX | PREAMBLE);
				rfm12_data(RFM12_CMD_TX | PREAMBLE);
				
				//set ET in power register to enable transmission
				//(hint: TX starts now)
				rfm12_data(RFM12_CMD_PWRMGT | PWRMGT_DEFAULT | RFM12_PWRMGT_ET);
			
				RFM12_INT_ON();
			}
#if !(RFM12_NOCOLISSIONDETECTION)
		}
	}
#endif

}


/* @description ask the rfm12 to transmit a packet when possible (carrier sense)
 * the data should be written to the tx buffer first after asking if
 * it is empty.
 *
 * @return see rfm12.h for possible return values.
 */
#if (RFM12_NORETURNS)
void 
#else
uint8_t
#endif
rfm12_start_tx(uint8_t type, uint8_t length)
{
	//exit if the buffer isn't free
	if(rf_tx_buffer.status != STATUS_FREE)
		return TXRETURN(RFM12_TX_OCCUPIED);
	
	//calculate number of bytes to be sent by ISR
	//2 sync bytes + len byte + type byte + checksum + message length + 1 dummy byte
	rf_tx_buffer.num_bytes = length + 6;
	
	//write airlab header to buffer
	rf_tx_buffer.len = length;
	rf_tx_buffer.type = type;
	rf_tx_buffer.checksum = length ^ type ^ 0xff;
	
	//reset counter
	rf_tx_buffer.bytecount = 0;
	
	//schedule packet for transmission
	rf_tx_buffer.status = STATUS_OCCUPIED;
	
	return TXRETURN(RFM12_TX_ENQUEUED);
}

/* @description send data out
 * @return see rfm12.h for possible return values.
 */
#if (RFM12_NORETURNS)
void
#else
uint8_t 
#endif
rfm12_tx ( uint8_t len, uint8_t type, uint8_t *data )
{
	#if RFM12_UART_DEBUG
		uart_putstr ("sending: ");
		uart_putstr ((char*)data);
		uart_putstr ("\r\n");
	#endif
	if (len > RFM12_TX_BUFFER_SIZE) return TXRETURN(RFM12_TX_ERROR);

	//exit if the buffer isn't free
	if(rf_tx_buffer.status != STATUS_FREE)
		return TXRETURN(RFM12_TX_OCCUPIED);
	
	memcpy ( rf_tx_buffer.buffer, data, len );

#if (!(RFM12_NORETURNS))
	return rfm12_start_tx (type, len);
#else
	rfm12_start_tx (type, len);
#endif
}

void spi_init()
{
	DDR_MOSI   |= (_BV(BIT_MOSI));
	DDR_SCK    |= (_BV(BIT_SCK));
	DDR_SPI_SS |= (_BV(BIT_SPI_SS));
	DDR_MISO   &= ~(_BV(BIT_MISO));


#ifndef SPI_SOFTWARE	
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);//SPI Master, clk/16
#endif
}


#ifdef RFM12_USE_WAKEUP_TIMER
void rfm12_set_wakeup_timer(uint16_t val){
	rfm12_data (RFM12_CMD_WAKEUP | val);
}
#endif

#ifdef RFM12_LOW_POWER
void rfm12_powerDown()
{
	RFM12_INT_OFF();
	rfm12_data(RFM12_CMD_PWRMGT | PWRMGT_DEFAULT);	
}

uint8_t rfm12_lowPowerTx( uint8_t len, uint8_t type, uint8_t *data )
{
	uint8_t retVal;
	
	//enable whole receiver chain
	rfm12_data(RFM12_CMD_PWRMGT | PWRMGT_DEFAULT | RFM12_PWRMGT_ER);
	
	//tick once (reactivates rfm12 int)
	rfm12_tick();
	
	//tx
	retVal = rfm12_tx(len, type, data);
	
	//tx packet
	while(rfm12_mode != RFM12_TX)	
	{
		rfm12_tick();
	}
	
	//wait for tx complete
	while(rf_tx_buffer.status != STATUS_FREE);
	
	//powerdown rfm12
	rfm12_powerDown();
	
	return retVal;
}
#endif


