/* -*- Mode: C; tab-width: 2 -*- */
#include <avr/io.h>

#include <avrx-io.h>
#include "avrx.h"               // AvrX System calls/data structures

#include "xcan.h"

#include "twi_master/twi_master.h"
#include "queue.h"
#include "PowerCommander.h"

TimerControlBlock switchtimer;
TimerControlBlock switch_timer_vortrag;
TimerControlBlock switch_timer_lounge;


AVRX_GCC_TASKDEF(switch_vortrag, 60, 7)
{
	uint8_t stat_vortrag = 0; // nur an oder aus 
	static t_i2cMessage_out i2c_outdata;
	static t_canMessage_out can_outdata;

	while (1)
	{
		if ((!(PINA & _BV(PA2))) && (stat_vortrag == 1))
		{
					i2c_outdata.outdata[0]=C_VIRT;
					i2c_outdata.outdata[1]=VIRT_VORTRAG;
					i2c_outdata.outdata[2]=F_SW_OFF;
					i2c_outdata.outdata[3]=0x00;

					/*
						hier muss das rein das nachrichten nochmal ueber 
						CAN rausgesendet werden
					*/

					can_outdata.outdata[0]=C_VIRT;
					can_outdata.outdata[1]=VIRT_VORTRAG;
					can_outdata.outdata[2]=F_SW_OFF;
					can_outdata.outdata[3]=0x00;

					AvrXSendMessage(&canQueue_out_info, &can_outdata.mcb);
					AvrXWaitMessageAck(&can_outdata.mcb);

					AvrXSendMessage(&i2cQueue_out, &i2c_outdata.mcb);
					AvrXWaitMessageAck(&i2c_outdata.mcb);

					AvrXDelay(&switch_timer_vortrag, 3000);
					stat_vortrag=0;
				}
			if ((!(PINA & _BV(PA2))) && stat_vortrag == 0)
				{
					i2c_outdata.outdata[0]=C_VIRT;
					i2c_outdata.outdata[1]=VIRT_VORTRAG;
					i2c_outdata.outdata[2]=F_SW_ON;
					i2c_outdata.outdata[3]=0x00;

					/*
						hier muss das rein das nachrichten nochmal ueber 
						CAN rausgesendet werden
					*/

					can_outdata.outdata[0]=C_VIRT;
					can_outdata.outdata[1]=VIRT_VORTRAG;
					can_outdata.outdata[2]=F_SW_ON;
					can_outdata.outdata[3]=0x00;

					AvrXSendMessage(&canQueue_out_info, &can_outdata.mcb);
					AvrXWaitMessageAck(&can_outdata.mcb);

					AvrXSendMessage(&i2cQueue_out, &i2c_outdata.mcb);
					AvrXWaitMessageAck(&i2c_outdata.mcb);

					AvrXDelay(&switch_timer_vortrag, 3000);
					stat_vortrag=1;
				}
				AvrXDelay(&switch_timer_vortrag, 1);
		}

}

AVRX_GCC_TASKDEF(switch_lounge, 60, 7)
{
	uint8_t stat_lounge=0; // nur an oder aus 
	static t_i2cMessage_out i2c_outdata;
	static t_canMessage_out can_outdata;
	while(1)
		{
			if ((!(PINB & _BV(PB0))) && stat_lounge == 1)
				{
					i2c_outdata.outdata[0]=C_SW;
					i2c_outdata.outdata[1]=SWL_LOUNGE;
					i2c_outdata.outdata[2]=F_SW_OFF;
					i2c_outdata.outdata[3]=0x00;

					/*
						hier muss das rein das nachrichten nochmal ueber 
						CAN rausgesendet werden
					*/

					can_outdata.outdata[0]=C_SW;
					can_outdata.outdata[1]=SWL_LOUNGE;
					can_outdata.outdata[2]=F_SW_OFF;
					can_outdata.outdata[3]=0x00;

					AvrXSendMessage(&canQueue_out_info, &can_outdata.mcb);
					AvrXWaitMessageAck(&can_outdata.mcb);

					AvrXSendMessage(&i2cQueue_out, &i2c_outdata.mcb);
					AvrXWaitMessageAck(&i2c_outdata.mcb);

					AvrXDelay(&switch_timer_lounge, 3000);
					stat_lounge=0;
				}
			if ((!(PINB & _BV(PB0))) && stat_lounge == 0)
				{
					i2c_outdata.outdata[0]=C_SW;
					i2c_outdata.outdata[1]=SWL_LOUNGE;
					i2c_outdata.outdata[2]=F_SW_ON;
					i2c_outdata.outdata[3]=0x00;

					/*
						hier muss das rein das nachrichten nochmal ueber 
						CAN rausgesendet werden
					*/

					can_outdata.outdata[0]=C_SW;
					can_outdata.outdata[1]=SWL_LOUNGE;
					can_outdata.outdata[2]=F_SW_ON;
					can_outdata.outdata[3]=0x00;

					AvrXSendMessage(&canQueue_out_info, &can_outdata.mcb);
					AvrXWaitMessageAck(&can_outdata.mcb);

					AvrXSendMessage(&i2cQueue_out, &i2c_outdata.mcb);
					AvrXWaitMessageAck(&i2c_outdata.mcb);

					AvrXDelay(&switch_timer_lounge, 3000);
					
					stat_lounge=1;
				}
				AvrXDelay(&switch_timer_lounge, 1);
		}
	
}



AVRX_GCC_TASKDEF(switchtask, 60, 7)
{
	static t_i2cMessage_out i2c_outdata;

	static t_canMessage_out can_outdata;

	uint8_t stat_haupt = 0;

	while (1)
  	{
			if (!(PINA & _BV(PA0)) && stat_haupt == 1)
				{
					
					i2c_outdata.outdata[0]=C_VIRT;
					i2c_outdata.outdata[1]=VIRT_POWER;
					i2c_outdata.outdata[2]=F_SW_OFF;
					i2c_outdata.outdata[3]=0x00;

					stat_haupt = 0;

					/*
						hier muss das rein das nachrichten nochmal ueber 
						CAN rausgesendet werden
					*/

					can_outdata.outdata[0]=C_VIRT;
					can_outdata.outdata[1]=VIRT_POWER;
					can_outdata.outdata[2]=F_SW_OFF;
					can_outdata.outdata[3]=0x00;

					AvrXSendMessage(&canQueue_out_info, &can_outdata.mcb);
					AvrXWaitMessageAck(&can_outdata.mcb);
					/*
						es sollte da auch noch ein sleep oder so rein,
						damit die geraete evt drauf reagieren koennen.
					*/
					AvrXDelay(&switchtimer, 1000);

					
					AvrXSendMessage(&i2cQueue_out, &i2c_outdata.mcb);
					AvrXWaitMessageAck(&i2c_outdata.mcb);

					AvrXDelay(&switchtimer, 1000);
				}
			if ((PINA & _BV(PA0)) && stat_haupt == 0)
				{
					i2c_outdata.outdata[0]=C_VIRT;
					i2c_outdata.outdata[1]=VIRT_POWER;
					i2c_outdata.outdata[2]=F_SW_ON;
					i2c_outdata.outdata[3]=0x00;

					stat_haupt = 1;

					/*
						hier muss das rein das nachrichten nochmal ueber 
						CAN rausgesendet werden
					*/
					can_outdata.outdata[0]=C_VIRT;
					can_outdata.outdata[1]=VIRT_POWER;
					can_outdata.outdata[2]=F_SW_ON;
					can_outdata.outdata[3]=0x00;

					AvrXSendMessage(&canQueue_out_info, &can_outdata.mcb);
					AvrXWaitMessageAck(&can_outdata.mcb);
					/*
						es sollte da auch noch ein sleep oder so rein,
						damit die geraete evt drauf reagieren koennen.
					*/
					AvrXDelay(&switchtimer, 1000);


					AvrXSendMessage(&i2cQueue_out, &i2c_outdata.mcb);
					AvrXWaitMessageAck(&i2c_outdata.mcb);

					AvrXDelay(&switchtimer, 1000);

				}
			AvrXDelay(&switchtimer, 1);
  	}
}