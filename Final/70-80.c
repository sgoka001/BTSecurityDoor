/*  Name & E-mail: Srisri Gokanapudy & sgoka001@ucr.edu
*   Assignment: Custom Project (Smart Door System)
*
*   I acknowledge all content contained herein, excluding template or example
*   code, is my own original work.
*/

#define F_CPU 1000000UL
#include <util/delay.h>
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
#include "keypad&nokia.h"
//FreeRTOS include files 
#include "FreeRTOS.h" 
#include "task.h" 
#include "croutine.h" 

//////////////////////////////////////////////////////////////////////// LOCAL VARIABLES ////////////////////////////////////////////////////////////////////////
unsigned char loc[8] = {0x01, 0x03, 0x02, 0x06, 0x04, 0x0C, 0x08, 0x09}; //how much the motors should turn
unsigned char temp = 0; //loc of stepper motor atm
unsigned long numPhases;
unsigned char cnt = 0;

char key; //first entry on keypad
char key2; //second entry on keypad

unsigned char checker = 0; // 1 = valid entry, 0 = no key entered
unsigned char checker2 = 0; // 1 = valid entry, 0 = no key entered

unsigned char lr = 0; // unlock = 2, lock = 1
unsigned char prev = 0; // unlock = 2, lock = 1

//////////////////////////////////////////////////////////////////////// LOCAL VARIABLES ////////////////////////////////////////////////////////////////////////

enum MotorState {READ_MOTION, READ_KEY_INIT, READ_KEY_WAIT, READ_KEY_INIT_2, READ_KEY, UNLOCK, LOCK} motor_state;

void Motor_Init(){
	motor_state = READ_MOTION;
}

void Motor_Tick()
{
	char garage_button = (GetBit(~PIND, 3)); //if button is pressed then = 8
	char motion_sensor = (GetBit(~PIND, 2)); //if motion is sensed then = 4
	
	switch(motor_state)
	{	
		case READ_MOTION:
			if(motion_sensor == 4){motor_state = READ_KEY_INIT;}
			else if(motion_sensor == 0){motor_state = READ_MOTION;}
			break;
		
		case READ_KEY_INIT:
			if(checker == 1){motor_state = READ_KEY_WAIT;}
			else if (checker == 0){motor_state = READ_KEY_INIT;}
			break;
			
		case READ_KEY_WAIT:
			if(cnt > 100){motor_state = READ_KEY_INIT_2; cnt = 0;}
			else{motor_state = READ_KEY_WAIT; cnt = cnt + 1;}
			break;
			
		case READ_KEY_INIT_2:
			if(checker2 == 1){motor_state = READ_KEY;}
			else if (checker2 == 0){motor_state = READ_KEY_INIT_2;}
			break;
			
		case READ_KEY:
			if (lr == 1){motor_state = LOCK;} // if passcode entered is = 1 then unlock
			else if (lr == 2){motor_state = UNLOCK;} // if passcode entered is = 3 then lock
			else {motor_state = READ_KEY;}
			break;
			
		case UNLOCK:
			if(cnt > 200) {motor_state = READ_MOTION; cnt = 0; lr = 0; prev = 2; motion_sensor = 0;}
			else {motor_state = UNLOCK; cnt = cnt + 1;}
			break;
			
		case LOCK:
			if(cnt > 200) {motor_state = READ_MOTION; cnt = 0; lr = 0; prev = 1; motion_sensor = 0;}
			else {motor_state = LOCK; cnt = cnt + 1;}
			break;
			
		default:
			motor_state = READ_MOTION;
			break;
	}
			
			
	switch(motor_state)
	{
		case READ_MOTION:
			PORTD = SetBit(PORTD,4,0); //GARAGE LIGHT OFF
			if (motion_sensor == 0){
				nokia_lcd_clear();
				nokia_lcd_set_cursor(35, 10);
				nokia_lcd_write_string("No", 2);
				nokia_lcd_set_cursor(15, 30);
				nokia_lcd_write_string("Motion", 2);
				nokia_lcd_set_cursor(50, 40);
				//nokia_lcd_write_char(motion_sensor + '0', 1);
				//nokia_lcd_write_char(garage_button + '0', 1);
			}
			nokia_lcd_render();
			break;
			
		case READ_KEY_INIT:
			key = GetKeypadKey();
			if (key != '\0')
			{
				checker = 1;
			}
			else{
				checker = 0;
			}
			
			nokia_lcd_clear();
			nokia_lcd_set_cursor(10, 0);
			nokia_lcd_write_string("PASSCODE?", 1);
			
			nokia_lcd_set_cursor(10, 40);
			nokia_lcd_write_string("Entry: ", 1);
			nokia_lcd_set_cursor(50, 40);
			//nokia_lcd_write_char(motion_sensor + '0', 1);
			nokia_lcd_write_char(key, 1);
			nokia_lcd_render();
			break;
			
		case READ_KEY_WAIT:
			break;
			
		case READ_KEY_INIT_2:
			key2 = GetKeypadKey();
			if (key2 != '\0')
			{
				checker2 = 1;
			}
			else{
				checker2 = 0;
			}
			
			nokia_lcd_clear();
			nokia_lcd_set_cursor(10, 0);
			nokia_lcd_write_string("PASSCODE?", 1);
		
			nokia_lcd_set_cursor(10, 40);
			nokia_lcd_write_string("Entry: ", 1);
			nokia_lcd_set_cursor(50, 40);
			nokia_lcd_write_char(key, 1);
			nokia_lcd_set_cursor(55, 40);
			//nokia_lcd_write_char(motion_sensor + '0', 1);
			nokia_lcd_write_char(key2, 1);
			nokia_lcd_render();
			break;
			
			
		case READ_KEY:
			if((key == '1') && (key2 == '2')){ //LOCK = 1 2
				lr = 1;
			}
			else if((key == '4') && (key2 == '5')){ //UNLOCK = 4 5
				lr = 2;
			}
			else{
				nokia_lcd_clear();
				nokia_lcd_set_cursor(0, 10);
				nokia_lcd_write_string("WRONG PASSWORD", 1);
				nokia_lcd_set_cursor(0, 30);
				nokia_lcd_write_string("RESTART BOARD", 1);
				nokia_lcd_set_cursor(10, 40);
				nokia_lcd_write_string("Entry: ", 1);
				nokia_lcd_set_cursor(50, 40);
				nokia_lcd_write_char(key, 1);
				nokia_lcd_set_cursor(55, 40);
				//nokia_lcd_write_char(motion_sensor + '0', 1);
				nokia_lcd_write_char(key2, 1);
				nokia_lcd_render();
			}
			numPhases = 1024; //(90 / 5.625) * 64
			break;
		
		case UNLOCK:
			if(lr){
				if (temp < 7) {temp = temp + 1;}
				else {temp = 0;}
			
				if((numPhases - 1) == 0)
				{
					lr = 0;
				}
			} 
			
			if(garage_button == 8){PORTA = (loc[temp] << 4); PORTD = ((0x01 << 4) | PORTD);}
			else{PORTA = loc[temp]; PORTD = SetBit(PORTD,4,0);}
				
			nokia_lcd_clear();
			nokia_lcd_set_cursor(10, 20);
			nokia_lcd_write_string("UNLOCKING!", 1);
			nokia_lcd_set_cursor(10, 40);
			nokia_lcd_write_string("Entry: ", 1);
			nokia_lcd_set_cursor(50, 40);
			nokia_lcd_write_char(key, 1); //changed key to garage_button
			nokia_lcd_set_cursor(55, 40);
			nokia_lcd_write_char(key2, 1);
			nokia_lcd_render();
			break;
			
		case LOCK:
			if(lr){
				if (temp > 0) {temp = temp - 1;}
				else {temp = 7;}
			
				if((numPhases - 1) == 0)
				{
					lr = 0;
				}
			}
			
			if(garage_button == 8){PORTA = (loc[temp] << 4); PORTD = ((0x01 << 4) | PORTD);}
			else{PORTA = loc[temp]; PORTD = SetBit(PORTD,4,0);}
				
			nokia_lcd_clear();
			nokia_lcd_set_cursor(10, 30);
			nokia_lcd_write_string("LOCKING!", 1);
			nokia_lcd_set_cursor(10, 40);
			nokia_lcd_write_string("Entry: ", 1);
			nokia_lcd_set_cursor(50, 40);
			nokia_lcd_write_char(key, 1);
			nokia_lcd_set_cursor(55, 40);
			nokia_lcd_write_char(key2, 1);
			nokia_lcd_render();
			break;
		
		default:
			break;
	}
}

void MotorTask()
{
	Motor_Init();
   for(;;) 
   { 	
	Motor_Tick();
	vTaskDelay(3); 
   } 
}

void StartSecPulse(unsigned portBASE_TYPE Priority)
{
	xTaskCreate(MotorTask, (signed portCHAR *)"MotorTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}	
 
int main(void) 
{
   DDRA = 0xFF; PORTA = 0x00; //stepper
   DDRB = 0xFF; PORTB = 0x00; //NOKIA 5110
   DDRC = 0xF0, PORTC = 0x0F; //calculator
   DDRD = 0xF2; PORTD = 0x0D; //D0 (Rx of Bluetooth), D2 (IR sensor), and D3 (button) inputs and rest outputs for lightbulbs and Tx of Bluetooth
   
   nokia_lcd_init();
   nokia_lcd_power(1);
   
   //Start Tasks  
   StartSecPulse(1);
    //RunSchedular 
   vTaskStartScheduler(); 
 
   return 0; 
}

/* To DO

*/