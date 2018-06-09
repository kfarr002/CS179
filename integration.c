/*
 * integration.c
 *
 * Created: 6/4/2018 8:26:01 AM
 * Author : K
*/
#include <avr/io.h>
#include "usart_ATmega1284.h"

int main(void)
{
	DDRA = 0xFF; PORTA = 0x00;
	DDRD = 0x00; PORTD = 0xFF;
	unsigned char motion = 0x00;
	unsigned char heartbeat = 0x00;
	
	initUSART(0);
	initUSART(1);
	
    while (1) 
    {
		if (USART_HasReceived(0)) {
			heartbeat = USART_Receive(0);
				if (heartbeat > 0)
				{
					heartbeat = 0x00;
					PORTA = heartbeat;
				}
				else
				{
					heartbeat = 0x02;
					PORTA = heartbeat;
				}	
			USART_Flush(0);
			//PORTA &= 0x00;
		}	
		//PORTA = heartbeat;	
	if (USART_HasReceived(1)) {
		motion = USART_Receive(1);
		//PORTA = 0xFF;
		if (motion > 0)
		{	
			motion = 0x00;
			PORTA = motion;
		}
		else
		{
			motion = 0x04;
			PORTA = motion;
		}
		USART_Flush(1);
		//PORTA &= 0x00;
	}
	
}
}
