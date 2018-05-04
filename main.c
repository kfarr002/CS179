#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/portpins.h>
#include <avr/pgmspace.h>

//FreeRTOS include files
#include "FreeRTOS.h"
#include "task.h"
#include "croutine.h"
#include "nokia.c"

char EKG_Flag = 0;
char Trainer_Flag = 0;
char Run_Flag = 0;
char Rest_Flag = 0;
char Run1_Flag = 0;
char Walk_Flag = 0;
char SC_Flag = 0;
char step_counter = 0;

enum SCPoll_States {SC_Start, SC,SC1} SCPoll_State;
long x = 0;
long y = 0;
long z = 0;
long x1 = 0;
long y2 = 0;
long z1 = 0;
long unit_vector = 0;
int array1[100];

enum Trainer_States {Trainer_Start, Trainer_Wait, Run, Rest, Run_1, Walk} Trainer_State;
	char run_time = 0; //seconds
	char rest_time = 0; //seconds
	char run1_time = 0; //seconds
	char walk_time = 0; //seconds
	char sec_cnt = 0;

enum Menu_States {Menu_Start, Main_Menu, Main_Menu_Wait, Heart_Rate,Heart_Rate_Wait, Step_Count, Step_Count_Wait, Training, Training_Wait} Menu_State;
char  down = 0;
char enter = 0;
char temp = 0;
char menu_select_count = 0;

enum EKGPoll_States {EKG_Poll_Off, Beat_Low, Beat_High_Wait, Beat_High} EKGPoll_State;
char heartRate = 0;
char heartBeat = 0;
char high = 0;
int second_count = 0;
int count_stop = 0; // Poll for 5 seconds counter
char pollSeconds = 2;  // used to be 5
char heartBeatCalc = 0;
int array[10] = {0,0,0,0,0,0,0,0,0,0};
char i = 0;
char sampleflag = 0;
int arrcnt = 0;

void main_menu_display()
{
	lcd_clear();
	lcd_string("Main Menu",1);
	if(menu_select_count == 0)
	{
		lcd_position(1, 10);
		lcd_string("Heart Beat<-----",1);
		lcd_position(1, 20);
		lcd_string("Step Counter",1);
		lcd_position(1,30);
		lcd_string("Trainer",1);
		lcd_position(1,40);
		lcd_string("Calibrate",1);
	}
	else if(menu_select_count == 1)
	{
		lcd_position(1, 10);
		lcd_string("Heart Beat",1);
		lcd_position(1, 20);
		lcd_string("Step Counter<---",1);
		lcd_position(1,30);
		lcd_string("Trainer",1);
		lcd_position(1,40);
		lcd_string("Calibrate",1);
	}
	else if(menu_select_count == 2)
	{
		lcd_position(1, 10);
		lcd_string("Heart Beat",1);
		lcd_position(1, 20);
		lcd_string("Step Counter",1);
		lcd_position(1,30);
		lcd_string("Trainer<---",1);
		lcd_position(1,40);
		lcd_string("Calibrate",1);
	}	

	
	lcd_display();
}
void Heart_Rate_Display()
{
	lcd_clear();
	lcd_string("HeartBeat:",1);
	lcd_position(1,10);
	char hr[5];
	itoa(heartRate,hr,10);
	lcd_string(hr, 3);
	itoa(heartBeat,hr,10);
	lcd_position(1,40);
	lcd_string(hr, 1);
	lcd_display();
	EKG_Flag = 1;
	PORTC = 0x01;
	
}
void Display_Trainer()
{
	lcd_clear();
	lcd_string("Press Enter to Start:",1);
	lcd_display();
	
}
void Display_Time()
{
	lcd_clear();
	if(Run_Flag == 1){
		lcd_string("Run for:",1);
		lcd_position(1,10);
		char rt[5];
		itoa(run_time,rt,10);
		lcd_string(rt, 3);
		lcd_display();
	}
	else if(Rest_Flag == 1){
		lcd_string("Rest for:",1);
		lcd_position(1,10);
		char rst[5];
		itoa(rest_time,rst,10);
		lcd_string(rst, 3);
		lcd_display();
	}
	else if(Run1_Flag == 1){
		lcd_string("Run for:",1);
		lcd_position(1,10);
		char r1t[5];
		itoa(run1_time,r1t,10);
		lcd_string(r1t, 3);
		lcd_display();
	}
	else if(Walk_Flag == 1){
		lcd_string("Walk for:",1);
		lcd_position(1,10);
		char wlk[5];
		itoa(walk_time,wlk,10);
		lcd_string(wlk, 3);
		lcd_display();
	}
}
void Display_SC()
{
	SC_Flag = 1;
	lcd_clear();
	lcd_string("StepCount:",1);
	lcd_position(1,10);
	char sc[5];
	itoa(x,sc,10);
	lcd_string(sc, 1);
	lcd_position(1,20);
	itoa(y,sc,10);
	lcd_string(sc, 1);
	lcd_position(1,30);
	itoa(z,sc,10);
	lcd_string(sc, 1);
	lcd_position(1,40);
	itoa(step_counter,sc,10);
	lcd_string(sc, 1);
	lcd_display();
	PORTC = 0x02;

	
}
void BPM()
{
	heartRate = 30*heartBeatCalc;  //used to be 12
	array[arrcnt] = heartRate;
	heartBeatCalc = 0;
	arrcnt++;
	if(sampleflag == 0)
	{
		array[0] = heartRate;
		array[1] = heartRate;
		array[2] = heartRate;
		array[3] = heartRate;
		array[4] = heartRate;
		array[5] = heartRate;
		array[6] = heartRate;
		array[7] = heartRate;
		array[8] = heartRate;
		array[9] = heartRate;
		
	}
	sampleflag = 1;
	if(arrcnt >= 9) // was 1
	{
		arrcnt = 0;
	}
	heartRate = ((array[0] + array[1]+ array[2]+ array[3]+ array[4] +array[5] + array[6]+ array[7]+ array[8]+ array[9])/10);
	

}
void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	// ADEN: setting this bit enables analog-to-digital conversion. // ADSC: setting this bit starts the first conversion.
	// ADATE: setting this bit enables auto-triggering. Since we are
	//in Free Running Mode, a new conversion will trigger whenever
	//the previous conversion completes.
	ADMUX |=(1<<REFS0);
	ADCSRA |= (1<<ADEN);
	ADCSRA |= (1<<ADSC);
}

uint16_t ADC_Select(char port_Select)
{
	//port_Select &= 0x07;
	//ADMUX = (ADMUX & 0xF8) | port_Select;
	ADMUX &= 0x00;
	if(port_Select == 0)
	{
		ADMUX = 0x01;
		ADCSRA |= (1<<ADSC);
		return ADCW;
	}
	else if(port_Select == 1)
	{
		ADMUX = 0x02;
		ADCSRA |= (1<<ADSC);
		return ADCW;
		
	}
	else if(port_Select == 2)
	{
		ADMUX = 0x03;
		ADCSRA |= (1<<ADSC);
		return ADCW;
			
	}

	
	
	//while(ADCSRA & (1<<ADSC));

	return 0;
}


void Display_Init(){
	
	 lcd_init();
	 lcd_clear();
	  Menu_State = Menu_Start;
}
void EKG_Init()
{
	EKGPoll_State = EKG_Poll_Off;

}
void Trainer_Init()
{

	run_time = 10; //seconds
	rest_time = 5; //seconds
	run1_time = 7; //seconds
	walk_time = 4; //seconds

	Trainer_State = Trainer_Start;

}
void StepCount_Init()
{
	//SC_Flag = 1;
	ADC_init();
	SCPoll_State = SC_Start;

}


void Display_Tick(){
		switch(Menu_State){
			
			case Menu_Start:
			Menu_State = Main_Menu;
			break;
			
			case Main_Menu:
			if(down && !enter)
			{
				Menu_State = Main_Menu_Wait;
			}
			else if(enter && !down)
			{
				Menu_State = Main_Menu_Wait;
			}
			else 
			{
				Menu_State = Main_Menu;
			}
			break;
			
			case Main_Menu_Wait:
			
			if(!down)
			{
				menu_select_count ++;
				Menu_State = Main_Menu;
			}
			else if(!enter)
			{
				if(menu_select_count == 0)
				{
					menu_select_count = 0;
					Menu_State = Heart_Rate;
				}
				else if(menu_select_count == 1)
				{
					menu_select_count = 0;
					Menu_State = Step_Count;
				}
				if(menu_select_count == 2)
				{
					menu_select_count = 0;
					Menu_State = Training;
				}
			}
			else if (down)
			{
				Menu_State = Main_Menu_Wait;
			}
			else if (enter)
			{
				Menu_State = Main_Menu_Wait;
			}
			if(menu_select_count > 2)
			{
				menu_select_count = 0;
			}
			
			break;
			
			case Heart_Rate:
			if(enter)
			{
				Menu_State = Heart_Rate_Wait;
			}
			else
			{
				Menu_State = Heart_Rate;
			}
			break;

			case Heart_Rate_Wait:
			if(!enter)
			{
				Menu_State = Main_Menu;
			}
			else
			{
				Menu_State = Heart_Rate_Wait;
			}
			break;
			
			case Step_Count:
			if(enter)
			{
				Menu_State = Step_Count_Wait;
			}
			else
			{
				Menu_State = Step_Count;
			}
			break;
			
			case Step_Count_Wait:
			if(!enter)
			{
				Menu_State = Main_Menu;
			}
			else
			{
				Menu_State = Step_Count_Wait;
			}
			break;
			
			case Training:
				if(enter)
				{
					Menu_State = Training_Wait;
				}
				else
				{
					Menu_State = Training;
				}
			
			break;
			
			case Training_Wait:
			if(!enter)
			{
				Menu_State = Main_Menu;
			}
			else
			{
				Menu_State = Training_Wait;
			}
			break;
			
			default:
			Menu_State = Menu_Start;
			break;
			
			
		}
		switch(Menu_State){
			
			case Menu_Start:
			break;
			
			case Main_Menu:
				main_menu_display();
			break;

			case Main_Menu_Wait:
			
			break;
			
			case Heart_Rate:
				Heart_Rate_Display();
			break;
			
			case Heart_Rate_Wait:
				EKG_Flag = 0;
			break;
			
			case Step_Count:
				Display_SC();
			break;
			
			case Step_Count_Wait:
			Display_SC();
			break;
			case Training:
			Trainer_Flag = 1;
			PORTC = 0x03;
			break;
			
			default:
			break;
			
			
			
		}
	
}
void EKG_Tick(){
	switch(EKGPoll_State){
		
		case EKG_Poll_Off:
		EKGPoll_State = Beat_Low;
		break;
		
		case Beat_Low:
		if(high)
		{
			EKGPoll_State = Beat_High_Wait;
		}
		else
		{
			EKGPoll_State = Beat_Low;
		}
		break;
		
		case Beat_High_Wait:
		
		if(high)
		{
			EKGPoll_State = Beat_High_Wait;
		}
		else
		{
			EKGPoll_State = Beat_High;
		}
		break;
		case Beat_High:
		
			EKGPoll_State = Beat_Low;
		
		break;
		
		default:
		EKGPoll_State = EKG_Poll_Off;
		break;
		
	}
	switch(EKGPoll_State){
		
		case EKG_Poll_Off:
		heartBeat = 0;
		heartRate = 0;
		
		break;
		
		case Beat_Low:
		//PORTC = 0x00;
		break;
		
		case Beat_High_Wait:
		//heartBeat++;
		//PORTC = 0xFF;
		break;
		
		case Beat_High:
		heartBeat++;
		//PORTC = 0xFF;
		break;
		
		default:
		break;
		
		
		
	}
}
void Trainer_Tick(){
	switch(Trainer_State){
		
		case Trainer_Start:
		if(enter && !down)
		{
			Trainer_State = Trainer_Wait;
		}
		else
		{
			Trainer_State = Trainer_Start;
		}
		break;
		
		case Trainer_Wait:
		if(!enter)
		{
			Trainer_State = Run;
		}
		else
		{
			Trainer_State = Trainer_Wait;
		}
		break;
		
		case Run:
		
		if(run_time == 0)
		{
			Trainer_State = Rest;
		}
		else
		{
			Trainer_State = Run;
		}
			if(sec_cnt ==10)
			{
				run_time--;
				sec_cnt = 0;
			}
			sec_cnt ++;
			
		break;
		
		case Rest:
		
		if(rest_time == 0)
		{
			Trainer_State = Run_1;
		}
		else
		{
			Trainer_State = Rest;
		}
		if(sec_cnt ==10)
		{
			rest_time--;
			sec_cnt = 0;
		}
		sec_cnt ++;
		
		break;
		
		case Run_1:
		
		if(run1_time == 0)
		{
			Trainer_State = Walk;
		}
		else
		{
			Trainer_State = Run_1;
		}
		if(sec_cnt ==10)
		{
			run1_time--;
			sec_cnt = 0;
		}
		sec_cnt ++;
		
		break;
		case Walk:
		
		if(walk_time == 0)
		{
			Trainer_State = Trainer_Start;
		}
		else
		{
			Trainer_State = Walk;
		}
		if(sec_cnt ==10)
		{
			walk_time--;
			sec_cnt = 0;
		}
		sec_cnt ++;
		
		break;
				
		
		default:
		Trainer_State = Trainer_Start;
		break;
		
	}
	switch(Trainer_State){
		
		case Trainer_Start:
		Run_Flag = 0;
		Rest_Flag = 0;
		Run1_Flag = 0;
		Walk_Flag = 0;
		Display_Trainer();
		break;
		
		case Trainer_Wait:
		break;
		
		case Run:
		Run_Flag = 1;
		Display_Time();
		break;

		case Rest:
		Run_Flag = 0;
		Rest_Flag = 1;
		Display_Time();
		break;
		
		case Run_1:
		Run_Flag = 0;
		Rest_Flag = 0;
		Run1_Flag = 1;		
		Display_Time();
		break;
				
		case Walk:
		Run_Flag = 0;
		Rest_Flag = 0;
		Run1_Flag = 0;
		Walk_Flag = 1;
		Display_Time();
		break;
			
		
		default:
		break;
		
		
		
	}
}
void StepCount_Tick(){
	switch(SCPoll_State){
		
		case SC_Start:
		SC_Flag = 1;
		if(SC_Flag == 1)
		{
			SCPoll_State = SC;
		}
		else
		{
			SCPoll_State = SC_Start;
		}
		break;
		
		case SC:
			SCPoll_State = SC1;
		break;
		
		case SC1:
			SCPoll_State = SC;
		break;
				
		
		default:
		SCPoll_State = SC_Start;
		break;
		
	}
	switch(SCPoll_State){
		
		case SC_Start:
		
		break;
		
		case SC:
		//PORTC = 0xFF;
		x = ADC_Select(0);
		y = ADC_Select(1);
		z = ADC_Select(2);
		x1 = (x >> 8);	
		y2 = (y >> 8);
		z1 = (z >> 8);			
		break;
		
		//PORTC = 0x00;
		case SC1:
		unit_vector = sqrt((x*x) + (y*y) + (z*z));
		if(x1 > 2)
		{
				_delay_ms(20);
			if(x1 > 2)
			{
			
			step_counter ++;
			}
		}
		_delay_ms(10);
		break;
		
		default:
		break;
		
		
		
	}
}


void DisplaySecTask()
{
	Display_Init();
	for(;;)
	{
		if(Trainer_Flag == 0)
		{
		temp = ~PIND;
		down = temp & 0x01;
		enter = temp & 0x02;
		Display_Tick();
		
		}
		vTaskDelay(100);
		
	}
}
void EKGSecTask()
{
	EKG_Init();
	for(;;)
	{
		
		count_stop = 200; // used to be 500
		high = PINA & 0x01;
		if(second_count == count_stop)
		{
			second_count = 0;
			heartBeatCalc = heartBeat;
			heartBeat = 0;
			BPM();
			//heartBeat = 0;
			sampleflag = 1;
		}
		EKG_Tick();
		second_count++;
		vTaskDelay(10);
	}
}
void TrainerSecTask()
{
	Trainer_Init();
	for(;;)
	{
		//Trainer_Flag = 1;
		if(Trainer_Flag == 1)
		{
			temp = ~PIND;
			down = temp & 0x01;
			enter = temp & 0x02;
			Trainer_Tick();
			//second_count++;
			
		}
	vTaskDelay(100);
	}
}
void StepCountSecTask()
{
	StepCount_Init();
	for(;;)
	{
		StepCount_Tick();
		vTaskDelay(100);
	}
}


void StartSecPulse(unsigned portBASE_TYPE Priority)
{
	xTaskCreate(DisplaySecTask, (signed portCHAR *)"LedSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
	xTaskCreate(EKGSecTask, (signed portCHAR *)"LedSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
	xTaskCreate(TrainerSecTask, (signed portCHAR *)"LedSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
	xTaskCreate(StepCountSecTask, (signed portCHAR *)"LedSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );

}

int main(void)
{
 PORTA = 0XFF; DDRA = 0x00;
 PORTD = 0xFF; DDRD = 0x00;
 PORTC = 0x00; DDRC = 0xFF;
	StartSecPulse(1);
	vTaskStartScheduler();

	return 0;
}
