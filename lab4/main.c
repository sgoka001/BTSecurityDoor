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
#include "usart_ATmega1284.h"
//FreeRTOS include files 
#include "FreeRTOS.h" 
#include "task.h" 
#include "croutine.h" 

//////////////////////////////////////////////////////////////////////// LOCAL VARIABLES ////////////////////////////////////////////////////////////////////////
unsigned char loc[8] = {0x01, 0x03, 0x02, 0x06, 0x04, 0x0C, 0x08, 0x09}; //how much the motors should turn
unsigned char temp = 0; //loc of stepper motor atm
unsigned long numPhases = 1024; //(90 / 5.625) * 64;
unsigned char cnt = 0;

unsigned char menu_key;
unsigned char key; //first entry on keypad
unsigned char key2; //second entry on keypad
unsigned char key3;

unsigned char bt_check = 0;
unsigned char menu_check = 0;
unsigned char checker = 0; // 1 = valid entry, 0 = no key entered
unsigned char checker2 = 0; // 1 = valid entry, 0 = no key entered
unsigned char checker3 = 0; // 1 = valid entry, 0 = no key entered

unsigned char passwordarray[3] = {'1', '2', '3'};

unsigned char lr = 0; // unlock = 2
unsigned char room = 0; // bed = 1 garage = 2
//////////////////////////////////////////////////////////////////////// LOCAL VARIABLES ////////////////////////////////////////////////////////////////////////

enum MotorState {READ_MOTION, MENU, READ_KEY_WAIT, READ_KEY_INIT, CURRENT_PASS, CHANGE_PASS, READ_KEY_WAIT_2, READ_KEY_INIT_2, READ_KEY_WAIT_3, READ_KEY_INIT_3, READ_KEY, LOCK, UNLOCK} motor_state;

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
			if(motion_sensor == 4){motor_state = MENU;}
			else if(motion_sensor == 0){motor_state = READ_MOTION;}
			break;
		
		case MENU:
			if(menu_check == 1){motor_state = READ_KEY_WAIT;}
			else if(menu_check == 2){motor_state = CHANGE_PASS;}
			else if(menu_check == 3){motor_state = CURRENT_PASS;}
			else if(menu_check == 0){motor_state = MENU;}
			break;
			
		case READ_KEY_WAIT:
			if(cnt > 100){motor_state = READ_KEY_INIT; cnt = 0;}
			else{motor_state = READ_KEY_WAIT; cnt = cnt + 1;}
			break;
			
		case READ_KEY_INIT:
			if((checker == 1) && (bt_check == 0)){motor_state = READ_KEY_WAIT_2;}
			else if((checker == 0) && (bt_check == 1)){motor_state = UNLOCK; lr = 2;}
			else if((checker == 0) && (bt_check == 0)){motor_state = READ_KEY_INIT;}
			break;
			
		case CURRENT_PASS:
			if(cnt > 100){motor_state = MENU; cnt = 0;}
			else{motor_state = CURRENT_PASS; cnt = cnt + 1;}
			break;
			
		case CHANGE_PASS:
			if(cnt > 100){motor_state = MENU; cnt = 0;}
			else{motor_state = CHANGE_PASS; cnt = cnt + 1;}
			break;
			
		case READ_KEY_WAIT_2:
			if(cnt > 200){motor_state = READ_KEY_INIT_2; cnt = 0;}
			else{motor_state = READ_KEY_WAIT_2; cnt = cnt + 1;}
			break;
			
		case READ_KEY_INIT_2:
			if((checker2 == 1) && (bt_check == 0)){motor_state = READ_KEY_WAIT_3;}
			else if((checker2 == 0) && (bt_check == 1)){motor_state = UNLOCK; lr = 2;}
			else if((checker2 == 0) && (bt_check == 0)){motor_state = READ_KEY_INIT_2;}
			break;
			
		case READ_KEY_WAIT_3:
			if(cnt > 200){motor_state = READ_KEY_INIT_3; cnt = 0;}
			else{motor_state = READ_KEY_WAIT_3; cnt = cnt + 1;}
			break;
			
		case READ_KEY_INIT_3:
			if((checker3 == 1) && (bt_check == 0)){motor_state = READ_KEY;}
			else if((checker3 == 0) && (bt_check == 1)){motor_state = UNLOCK; lr = 2;}
			else if((checker3 == 0) && (bt_check == 0)){motor_state = READ_KEY_INIT_3;}
			break;
			
		case READ_KEY:
			if (lr == 2){motor_state = UNLOCK;}
			else {motor_state = READ_KEY;}
			break;
		
		case UNLOCK:
			if(cnt > 200) {motor_state = LOCK; cnt = 0; motion_sensor = 0;}
			else {motor_state = UNLOCK; cnt = cnt + 1;}
			break;
			
		case LOCK:
			if(cnt > 200) {motor_state = READ_MOTION; cnt = 0; lr = 0; motion_sensor = 0;}
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
				nokia_lcd_set_cursor(35, 0);
				nokia_lcd_write_string("No", 2);
				nokia_lcd_set_cursor(15, 20);
				nokia_lcd_write_string("Motion", 2);
			}
			nokia_lcd_render();
			break;
			
		case MENU:
			menu_key = GetKeypadKey();
			if ((menu_key == 'A')){
				menu_check = 1;
			}
			else if ((menu_key == 'B')){
				menu_check = 2;
			}
			else if ((menu_key == 'C')){
				menu_check = 3;
			}
			else{
				menu_check = 0;
			}
			nokia_lcd_clear();
			nokia_lcd_set_cursor(0, 10);
			nokia_lcd_write_string("A-enter code", 1);
			nokia_lcd_set_cursor(0, 20);
			nokia_lcd_write_string("B-change code", 1);
			nokia_lcd_set_cursor(0, 30);
			nokia_lcd_write_string("C-check code", 1);
			nokia_lcd_render();
			break;
			
		case READ_KEY_WAIT:
			break;	
			
		case READ_KEY_INIT:
			if((USART_HasReceived(0))){
				bt_check = 1;
				key = USART_Receive(0);
				USART_Flush(0);
			}
			else{
				bt_check = 0;
				key = GetKeypadKey();
				if ((key != '\0')){
					checker = 1;
				}
				else{
					checker = 0;
				}
			}
				
			nokia_lcd_clear();
			nokia_lcd_set_cursor(30, 0);
			nokia_lcd_write_string("CODE?", 1);
			nokia_lcd_set_cursor(10, 30);
			nokia_lcd_write_string("Entry: ", 1);
			nokia_lcd_set_cursor(50, 30);
			nokia_lcd_write_char(key, 1);
			nokia_lcd_render();
			break;
			
		case CURRENT_PASS:
			nokia_lcd_clear();
			nokia_lcd_set_cursor(5, 10);
			nokia_lcd_write_string("Current Code:", 1);
			nokia_lcd_set_cursor(10, 20);
			nokia_lcd_write_char(passwordarray[0], 1);
			nokia_lcd_set_cursor(20, 20);
			nokia_lcd_write_char(passwordarray[1], 1);
			nokia_lcd_set_cursor(30, 20);
			nokia_lcd_write_char(passwordarray[2], 1);
			nokia_lcd_render();
			break;
			
		case CHANGE_PASS:
			nokia_lcd_clear();
			nokia_lcd_set_cursor(5, 10);
			nokia_lcd_write_string("New Code:", 1);
			nokia_lcd_render();
			break;
			
		case READ_KEY_WAIT_2:
			break;
			
		case READ_KEY_INIT_2:
			if((USART_HasReceived(0))){
				bt_check = 1;
				key2 = USART_Receive(0);
				USART_Flush(0);
			}
			else{
				key2 = GetKeypadKey();
				if ((key2 != '\0')){
					checker2 = 1;
				}
				else{
					checker2 = 0;
				}
			}
			
			nokia_lcd_clear();
			nokia_lcd_set_cursor(30, 0);
			nokia_lcd_write_string("CODE?", 1);
			nokia_lcd_set_cursor(10, 30);
			nokia_lcd_write_string("Entry: ", 1);
			nokia_lcd_set_cursor(50, 30);
			nokia_lcd_write_char(key, 1);
			nokia_lcd_set_cursor(60, 30);
			nokia_lcd_write_char(key2, 1);
			nokia_lcd_render();
			break;
			
		case READ_KEY_WAIT_3:
			break;
		
		case READ_KEY_INIT_3:
			if((USART_HasReceived(0))){
				bt_check = 1;
				key3 = USART_Receive(0);
				USART_Flush(0);
			}
			else{
				key3 = GetKeypadKey();
				if ((key3 != '\0')){
					checker3 = 1;
				}
				else{
					checker3 = 0;
				}
			}
		
			nokia_lcd_clear();
			nokia_lcd_set_cursor(30, 0);
			nokia_lcd_write_string("CODE?", 1);
			nokia_lcd_set_cursor(10, 30);
			nokia_lcd_write_string("Entry: ", 1);
			nokia_lcd_set_cursor(50, 30);
			nokia_lcd_write_char(key, 1);
			nokia_lcd_set_cursor(60, 30);
			nokia_lcd_write_char(key2, 1);
			nokia_lcd_set_cursor(70, 30);
			nokia_lcd_write_char(key3, 1);
			nokia_lcd_render();
			break;
			
			
		case READ_KEY:
			if((key == passwordarray[0]) && (key2 == passwordarray[1]) && (key3 == passwordarray[2])){ 
				lr = 2;
			}
			else{
				nokia_lcd_clear();
				nokia_lcd_set_cursor(0, 10);
				nokia_lcd_write_string("WRONG PASSWORD", 1);
				nokia_lcd_set_cursor(0, 20);
				nokia_lcd_write_string("RESTART BOARD", 1);
				nokia_lcd_set_cursor(10, 30);
				nokia_lcd_write_string("Entry: ", 1);
				nokia_lcd_set_cursor(50, 30);
				nokia_lcd_write_char(key, 1);
				nokia_lcd_set_cursor(60, 30);
				nokia_lcd_write_char(key2, 1);
				nokia_lcd_set_cursor(70, 30);
				nokia_lcd_write_char(key3, 1);
				nokia_lcd_render();
			}
			
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
			if(garage_button == 8){PORTA = (loc[temp] << 4); PORTD = ((0x01 << 4) | PORTD); room = 2;}
			else{PORTA = loc[temp]; PORTD = SetBit(PORTD,4,0); room = 1;}
				
			nokia_lcd_clear();
			nokia_lcd_set_cursor(10, 10);
			nokia_lcd_write_string("UNLOCKING!", 1);
			nokia_lcd_set_cursor(10, 30);
			nokia_lcd_write_string("Entry: ", 1);
			if(bt_check == 0){
				nokia_lcd_set_cursor(50, 30);
				nokia_lcd_write_char(key, 1);
				nokia_lcd_set_cursor(60, 30);
				nokia_lcd_write_char(key2, 1);
				nokia_lcd_set_cursor(70, 30);
				nokia_lcd_write_char(key3, 1);
			}
			else{
				nokia_lcd_set_cursor(50, 30);
				nokia_lcd_write_string("Phone", 1);
			}
			nokia_lcd_set_cursor(10, 40);
			nokia_lcd_write_string("Room: ", 1);
			nokia_lcd_set_cursor(65, 40);
			nokia_lcd_write_char(room + '0', 1);
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
			if(garage_button == 8){PORTA = (loc[temp] << 4); PORTD = ((0x01 << 4) | PORTD); room = 2;}
			else{PORTA = loc[temp]; PORTD = SetBit(PORTD,4,0); room = 1;}
			
			nokia_lcd_clear();
			nokia_lcd_set_cursor(10, 10);
			nokia_lcd_write_string("LOCKING!", 1);
			nokia_lcd_set_cursor(10, 40);
			nokia_lcd_write_string("Room: ", 1);
			nokia_lcd_set_cursor(65, 40);
			nokia_lcd_write_char(room + '0', 1);
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
   initUSART(0);
   
   //Start Tasks  
   StartSecPulse(1);
    //RunSchedular 
   vTaskStartScheduler();
   /*
   eeprom_write_byte(0, password)
   password = eeprom_read_byte(0);
   eeprom_update_byte(0, )
   */
   return 0; 
}

/* To DO

*/