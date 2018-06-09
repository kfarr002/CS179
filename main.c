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
#include <avr/sleep.h>

//FreeRTOS include files
#include "FreeRTOS.h"
#include "task.h"
#include "croutine.h"
#include "nokia.c"
#include "usart_ATmega1284.h"

char EKG_Flag = 0;
char Trainer_Flag = 0;
char Run_Flag = 0;
char Rest_Flag = 0;
char Run1_Flag = 0;
char Walk_Flag = 0;
char SC_Flag = 0;
char cal_SC = 0;
char step_counter = 0;
char Calibrate_Flag = 0;
int temp1 = 0;
long to = 0;
long bo= 1310;
int walk = 0;
int rest = 0;
int run = 0;
int run1 = 0;
char integ = 0;
char c = 0;
char hbeat = 0;
//char count_state = 0

enum sleep_States {S_Off, S_On} sleep_State;
char SCount = 0;
char s = 0;
char s1 = 0;

enum bluetooth_States {B_Off, B_On} bluetooth_State;

enum Calibrate_States {Calibrate_Start, Calibrate_Run, Calibrate_Min, Calibrate_Min_Wait,Calibrate_Second, Calibrate_Second_Wait, Calibrate_Wait} Calibrate_State;
	long temp2 = 0;
	long x2 = 0;
	long y2 = 0;
	long z2 = 0;
	long b = 0;
	long t = 0;
	long temp3 = 0;
	char count_state  = 0;
	char min = 0;
	char sec = 0;

enum SCPoll_States {SC_Start, SC,SC1} SCPoll_State;
	long x1 = 0;
	long z1 = 0;
	long unit_vector = 0;
	int array1[100];
	int upperbound = 0;
	int lowerbound = 0;
	int up = 0;
	int low = 0;

enum Trainer_States {Trainer_Start, Trainer_Wait, Run, Rest, Run_1, Walk, menu} Trainer_State;
	int run_time = 0; //seconds
	int rest_time = 0; //seconds
	int run1_time = 0; //seconds
	int walk_time = 0; //seconds
	int sec_cnt = 0;

enum Menu_States {Menu_Start, Main_Menu, Main_Menu_Wait, Heart_Rate,Heart_Rate_Wait, Step_Count, Step_Count_Wait, Training, Training_Wait, calibration,calibration_Wait} Menu_State;
	char  down = 0;
	char enter = 0;
	char temp = 0;
	char menu_select_count = 0;
	char enterflag = 0;
	char downflag = 0;

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
	else if(menu_select_count == 3)
	{
		lcd_position(1, 10);
		lcd_string("Heart Beat",1);
		lcd_position(1, 20);
		lcd_string("Step Counter",1);
		lcd_position(1,30);
		lcd_string("Trainer",1);
		lcd_position(1,40);
		lcd_string("Calibrate<--",1);
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
	//PORTC = 0x01;
	
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
void Display_Calibrate1()
{
	lcd_clear();
	lcd_string("Take 10 Steps:",1);
	//lcd_position(1,40);
	char sb[5];
	lcd_string(sb, 1);
	lcd_position(1,40);
	itoa(bo,sb,10);
	lcd_string(sb, 1);
	lcd_display();
}
void Display_SC()
{
	SC_Flag = 1;
	lcd_clear();
 	char sc[5];
	lcd_string("StepCount:",1);
	lcd_position(1, 10);
	
	itoa(step_counter,sc,10);
	lcd_string(sc, 1);
	lcd_position(1, 20);
	 	
 	lcd_string("Unit Vector: ",1);
 	lcd_position(1,35);

 	lcd_position(1,40);
 	itoa(unit_vector,sc,10);
 	lcd_string(sc, 1);
	lcd_display();
	//PORTC = 0x02;

	
}
void display_min()
{
	lcd_clear();
	if(count_state == 1)
	{
		lcd_string("Run Minutes:",1);
	}
	else if(count_state == 2)
	{
		lcd_string("Rest Minutes:",1);
	}
	else if(count_state == 3)
	{
		lcd_string("Run Minutes:",1);
	}
	else
	{
		lcd_string("Walk Minutes:",1);
	}
	
	//lcd_position(1,40);
	char mb[5];
	lcd_string(mb, 1);
	lcd_position(1,40);
	itoa(min,mb,10);
	lcd_string(mb, 1);
	lcd_display();
}
void display_second()
{
	lcd_clear();
	//lcd_string("Run Seconds:",1);
	if(count_state == 1)
	{
		lcd_string("Run Seconds:",1);
	}
	else if(count_state == 2)
	{
		lcd_string("Rest Seconds:",1);
	}
	else if(count_state == 3)
	{
		lcd_string("Run Seconds:",1);
	}
	else
	{
		lcd_string("Walk Seconds:",1);
	}
	//lcd_position(1,40);
	char ms[5];
	lcd_string(ms, 1);
	lcd_position(1,40);
	itoa(sec,ms,10);
	lcd_string(ms, 1);
	lcd_display();
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
	hbeat = heartRate;

}
void ADC_init() {
	 ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	 // ADEN: setting this bit enables analog-to-digital conversion. // ADSC: setting this bit starts the first conversion.
	 // ADATE: setting this bit enables auto-triggering. Since we are
	 //in Free Running Mode, a new conversion will trigger whenever
	 //the previous conversion completes.

 }
void ADC_Select(uint8_t port_Select)
 {
	 ADMUX = (port_Select <= 0x07) ? port_Select :ADMUX;
	 for(int i = 0; i < 15; i++){asm("nop");}

 }
uint16_t read()
 {
	 return ADC;
 }


long unitvector()
{
	long x = 0;
	long y = 0;
	long z = 0;
	//long uvector = 0;
	
	ADC_Select(1);
	x = read();
	ADC_Select(2);
	y = read();
	ADC_Select(3);
	z = read();
	x2 = x;
	y2 = y;
	
	return sqrt((x*x) + (y*y) + (z*z));

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
void Calibrate_Init()
{
	//SC_Flag = 1;
	ADC_init();
	
		Calibrate_State = Calibrate_Start;

	//Calibrate_State = Calibrate_Start;

}
void bluetooth_Init()
{
	//SC_Flag = 1;
	//ADC_init();
	initUSART(1);
	bluetooth_State = B_Off;

	//Calibrate_State = Calibrate_Start;

}
void sleep_Init()
{
	//SC_Flag = 1;
	ADC_init();
	sleep_State = S_Off;

	//Calibrate_State = Calibrate_Start;

}

void Display_Tick(){
		switch(Menu_State){
			
			case Menu_Start:
			Menu_State = Main_Menu;
			break;
			
			case Main_Menu:
			if(down && !enter)
			{	
				downflag = 1;
				//menu_select_count ++;
				Menu_State = Main_Menu_Wait;
			}
			else if(enter && !down)
			{
				enterflag = 1;
				Menu_State = Main_Menu_Wait;
			}
			else 
			{
				Menu_State = Main_Menu;
			}
			break;
			
			case Main_Menu_Wait:
			
			if(!down && !enter && downflag == 1)
			{
				downflag = 0;
				menu_select_count ++;
				Menu_State = Main_Menu;
			}
			else if(!enter && !down &&  enterflag == 1)
			{
				enterflag = 0;
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
				else if(menu_select_count == 2)
				{
					menu_select_count = 0;
					Menu_State = Training;
				}
				else if(menu_select_count == 3)
				{
					menu_select_count = 0;
					Menu_State = calibration;
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
			if(menu_select_count > 3)
			{
				menu_select_count = 0;
			}
			
			break;
			
			case Heart_Rate:
			if(enter && !down)
			{
				Menu_State = Heart_Rate_Wait;
			}
			else
			{
				Menu_State = Heart_Rate;
			}
			break;

			case Heart_Rate_Wait:
			if(!enter && !down)
			{
				Menu_State = Main_Menu;
			}
			else
			{
				Menu_State = Heart_Rate_Wait;
			}
			break;
			
			case Step_Count:
			if(enter && !down)
			{
				Menu_State = Step_Count_Wait;
			}
			else
			{
				Menu_State = Step_Count;
			}
			break;
			
			case Step_Count_Wait:
			if(!enter && !down)
			{
				Menu_State = Main_Menu;
			}
			else
			{
				Menu_State = Step_Count_Wait;
			}
			break;
			
			case Training:
				if(enter && !down)
				{
					Menu_State = Training_Wait;
				}
				else
				{
					Menu_State = Training;
				}
			
			break;
			
			case Training_Wait:
			if(!enter && !down)
			{
				Menu_State = Main_Menu;
			}
			else
			{
				Menu_State = Training_Wait;
			}
			break;
			
			case calibration:
			if(enter && !down)
			{
				Menu_State = calibration_Wait;
			}
			else
			{
				Menu_State = calibration;
			}
			
			break;
			
			case calibration_Wait:
			if(!enter && !down)
			{
				Menu_State = calibration_Wait;
			}
			else
			{
				Menu_State = Menu_Start;
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
			//PORTC = 0x03;
			break;
			case Training_Wait:
			//Trainer_Flag = 0;
			//PORTC = 0x03;
			break;
			case calibration:
			 Calibrate_Flag = 1;
			 //PORTC = 0x00;
			 //Display_Calibrate();
			 break;
			case calibration_Wait:
			//Display_Calibrate();
			// Calibrate_Flag = 0;
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
			if(sec_cnt == 10)
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
		if(sec_cnt == 10)
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
		if(sec_cnt == 10)
		{
			run1_time--;
			sec_cnt = 0;
		}
		sec_cnt ++;
		
		break;
		case Walk:
		
		if(walk_time == 0)
		{
			Trainer_State = menu;
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
		
		case menu:
		 Trainer_State = menu;
		 
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
		run_time = run;
		walk_time = walk;
		run1_time = run1;
		rest_time = rest;
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
		
		case menu:
		Walk_Flag = 0;
		Trainer_Flag = 0;
		//sec_cnt = 0;
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
		break;
		
		case SC1:
		if(down)
		{
			step_counter == 0;
		}
		unit_vector = unitvector();//sqrt((x*x) + (y*y) + (z*z));
		if(unit_vector >(to-20) || upperbound == 1)
		{ //1300 1275
			upperbound = 1;
			if(unit_vector <= (bo + 10))
			{
				lowerbound = 1;
			}
		}
		if(upperbound == 1 && lowerbound == 1)
		{			
			if(cal_SC == 1){
			step_counter ++;
			}
			upperbound = 0;
			lowerbound = 0;
		
		}
		break;
		
		default:
		break;
		
	}
}
void Calibrate_Tick(){
	switch(Calibrate_State){
		
		case Calibrate_Start:
			//Calibrate_Flag =1;
			if(Calibrate_Flag == 1)
			{
				Calibrate_State = Calibrate_Run;	
			}
			else
			{
				Calibrate_State = Calibrate_Start;	
			}
			
	
		break;
		
		case Calibrate_Run:
		if (!down && !enter && count_state > 5)
		{
			Calibrate_State = Calibrate_Wait;
		}
		
		else if(!down && !enter)
		{
			Calibrate_State = Calibrate_Min;
		}
		else
		{
			Calibrate_State = Calibrate_Run;
		}
		break;
		
		case Calibrate_Min:
		if(enter && !down)
		{
			
			Calibrate_State = Calibrate_Second;
		}
		else if(down && !enter)
		{	
			min++;
			Calibrate_State = Calibrate_Min_Wait;
		}
		else
		{
			Calibrate_State = Calibrate_Min;
		}
		break;
		case Calibrate_Min_Wait:
		if(!enter && !down)
		{
			Calibrate_State = Calibrate_Min;
		}
		else
		{
			Calibrate_State = Calibrate_Min_Wait;
		}
		break;

		case Calibrate_Second:
		if(enter && !down)
		{
			
			Calibrate_State = Calibrate_Run;
		}
		else if(down && !enter)
		{
			sec++;
			Calibrate_State = Calibrate_Second_Wait;
		}
		else
		{
			Calibrate_State = Calibrate_Second;
		}
		break;
		case Calibrate_Second_Wait:
		if(!enter && !down)
		{
			Calibrate_State = Calibrate_Second_Wait;
		}
		else
		{
			Calibrate_State = Calibrate_Second;
		}
		break;
		
		
		case Calibrate_Wait:
			if(enter)
			{
				Calibrate_Flag = 0;	
			}
			else{
			Calibrate_State = Calibrate_Wait;
			}
		break;
		
		default:
		Calibrate_State = Calibrate_Start;
		break;
		
	}
	switch(Calibrate_State){
		
		case Calibrate_Start:
		b = 1300;
		//bo = 1300;
		t = unitvector();
		to = t;
		break;
		
		case Calibrate_Run:
			if(count_state == 1)
			{
				run = ((min*60) + sec);
			}
			else if(count_state == 2)
			{
				rest = ((min*60) + sec);
			}
			else if(count_state == 3)
			{
				run1 = ((min*60) + sec);
			}
			else if(count_state == 4)
			{
				walk = ((min*60) + sec);
			}
			
			min = 0;
			sec = 0;
			count_state ++;
			//display_min();
		break;
		
		case Calibrate_Min:
			//min++;
			display_min();
		break;
		
		case Calibrate_Min_Wait:
			
			display_min();
		break;
		case Calibrate_Second:
		//min++;
		display_second();
		break;
		
		case Calibrate_Second_Wait:
		
		display_second();
		break;
		
		case Calibrate_Wait:
		count_state = 0;
		temp2 = unitvector();
		cal_SC = 1;
		if(temp2 > 1290)
		{
			if(temp2 < bo)
			{
				//PORTC = 0xFF;
				//b = temp2;
				bo = temp2;
			}
				
		}
		if(temp2 > t && temp2 < 1400)
		{
			t = temp2;
			to = temp2;		
		}
		Display_Calibrate1();
		break;
		
		
		default:
		break;
	}
}
void bluetooth_Tick(){
	switch(bluetooth_State){
		
		case B_Off:
		
		bluetooth_State = B_On;
		break;
		case B_On:
		
		//integ = 0x00;
		//PORTC = 0xFF;
		if(USART_IsSendReady(1))
		{
			PORTC = 0xFF;
			if(hbeat < 100){
			USART_Send(0x00,1);
			}
			else
			{
				USART_Send(0xFF,1);
			}
			
			//PORTC = 0x00;
			while(!(USART_HasTransmitted(1))){//PORTC = 0xFF;};
			//PORTC = 0xFF;
			bluetooth_State = B_On;
		}
		
		}
		default:
		bluetooth_State = B_Off;
		break;
		
	}
	switch(bluetooth_State){
		
		case B_Off:
		//if(c == 0)
// 		{
// 			integ = 0x00;
// 		}
// 		else
// 		{
// 			integ = 0xFF;
// 		}
		
		break;
		
		case B_On:
		break;
		default:
		break;
		
		
		
	}
}
void sleep_Tick(){
	switch(sleep_State){
		
		case S_Off:
		
		sleep_State = S_On;
		break;
		case S_On:
// 		if( (s- 10) < s <  (s1 + 10))
// 		{
// 			SCount++;
// 		}
// 		
// 		if(SCount > 2)
// 		{
// 			//set_sleep_mode(<mode>);
			cli();
			sleep_enable();
			sleep_disable();
			SCount = 0;
			
//		}
		sei();
		sleep_State = S_Off;
		
	
	default:
	sleep_State = S_Off;
	break;
	}

switch(sleep_State){
	
	case S_Off:
	s = unitvector();
	s1 = unitvector();
	SCount++;
	
	break;
	
	case S_On:
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
		//Calibrate_Flag = 1;
		if(Trainer_Flag == 0 && Calibrate_Flag == 0 )
		{
			
			//PORTC = 0xFF;
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
		if(EKG_Flag == 1){

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
		}
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
	if(SC_Flag == 1){

		StepCount_Tick();
	}
		vTaskDelay(100);
	}
}
void CalibrateSecTask()
{
	Calibrate_Init();
	for(;;)
	{
		if(Calibrate_Flag == 1){
			temp = ~PIND;
			down = temp & 0x01;
			enter = temp & 0x02;
		Calibrate_Tick();
		}
		vTaskDelay(300);
		
	}
}
void bluetoothSecTask()
{
	bluetooth_Init();
	for(;;)
	{
		//if(Calibrate_Flag == 1){
			//temp = ~PIND;
			//down = temp & 0x01;
			//enter = temp & 0x02;
			bluetooth_Tick();
		//}
		vTaskDelay(300);
		
	}
}
void SleepSecTask()
{
	sleep_Init();
	for(;;)
	{
		bluetooth_Tick();
		vTaskDelay(1000);
		
	}
}
void StartSecPulse(unsigned portBASE_TYPE Priority)
{
	xTaskCreate(DisplaySecTask, (signed portCHAR *)"LedSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
	xTaskCreate(EKGSecTask, (signed portCHAR *)"LedSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
	xTaskCreate(TrainerSecTask, (signed portCHAR *)"LedSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
	xTaskCreate(StepCountSecTask, (signed portCHAR *)"LedSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
	xTaskCreate(CalibrateSecTask, (signed portCHAR *)"LedSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
	xTaskCreate(bluetoothSecTask, (signed portCHAR *)"LedSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
	//xTaskCreate(SleepSecTask, (signed portCHAR *)"LedSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
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
