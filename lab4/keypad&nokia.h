
// Permission to copy is granted provided that this header remains intact. 
// This software is provided with no warranties.

////////////////////////////////////////////////////////////////////////////////

// Returns '\0' if no key pressed, else returns char '1', '2', ... '9', 'A', ...
// If multiple keys pressed, returns leftmost-topmost one
// Keypad must be connected to port C
// Keypad arrangement
//        Px4 Px5 Px6 Px7
//	  col 1   2   3   4
//  row  ______________
//Px0 1	| 1 | 2 | 3 | A
//Px1 2	| 4 | 5 | 6 | B
//Px2 3	| 7 | 8 | 9 | C
//Px3 4	| * | 0 | # | D

#ifndef KEYPAD_H
#define KEYPAD_H

unsigned char SetBit(unsigned char pin, unsigned char number, unsigned char bin_value)
{
	return (bin_value ? pin | (0x01 << number) : pin & ~(0x01 << number));
}

unsigned char GetBit(unsigned char port, unsigned char number)
{
	return ( port & (0x01 << number) );
}

// Keypad Setup Values
#define KEYPADPORT PORTC
#define KEYPADPIN  PINC

#define ROW1 0
#define ROW2 1
#define ROW3 2
#define ROW4 3
#define COL1 4
#define COL2 5
#define COL3 6
#define COL4 7
////////////////////////////////////////////////////////////////////////////////
//Functionality - Gets input from a keypad via time-multiplexing
//Parameter: None
//Returns: A keypad button press else '\0'
unsigned char GetKeypadKey() {

	// Check keys in col 1
	KEYPADPORT = SetBit(0xFF,COL1,0); // Set Px4 to 0; others 1
	asm("nop"); // add a delay to allow PORTx to stabilize before checking
	if ( GetBit(~KEYPADPIN,ROW1) ) { return '1'; }
	if ( GetBit(~KEYPADPIN,ROW2) ) { return '4'; }
	if ( GetBit(~KEYPADPIN,ROW3) ) { return '7'; }
	if ( GetBit(~KEYPADPIN,ROW4) ) { return '*'; }

	// Check keys in col 2
	KEYPADPORT = SetBit(0xFF,COL2,0); // Set Px5 to 0; others 1
	asm("nop"); // add a delay to allow PORTx to stabilize before checking
	if ( GetBit(~KEYPADPIN,ROW1) ) { return '2'; }
	if ( GetBit(~KEYPADPIN,ROW2) ) { return '5'; }
	if ( GetBit(~KEYPADPIN,ROW3) ) { return '8'; }
	if ( GetBit(~KEYPADPIN,ROW4) ) { return '0'; }

	// Check keys in col 3
	KEYPADPORT = SetBit(0xFF,COL3,0); // Set Px6 to 0; others 1
	asm("nop"); // add a delay to allow PORTx to stabilize before checking
	if ( GetBit(~KEYPADPIN,ROW1) ) { return '3'; }
	if ( GetBit(~KEYPADPIN,ROW2) ) { return '6'; }
	if ( GetBit(~KEYPADPIN,ROW3) ) { return '9'; }
	if ( GetBit(~KEYPADPIN,ROW4) ) { return '#'; }

	// Check keys in col 4
	KEYPADPORT = SetBit(0xFF,COL4,0); // Set Px7 to 0; others 1
	asm("nop"); // add a delay to allow PORTx to stabilize before checking
	if (GetBit(~KEYPADPIN,ROW1) ) { return 'A'; }
	if (GetBit(~KEYPADPIN,ROW2) ) { return 'B'; }
	if (GetBit(~KEYPADPIN,ROW3) ) { return 'C'; }
	if (GetBit(~KEYPADPIN,ROW4) ) { return 'D'; }
	
	return '\0';
}

#endif //KEYPAD_H

// NOKIA 5110 .C AND .H FILES FROM: https://github.com/LittleBuster/avr-nokia5110
  
/* Nokia 5110 LCD AVR Library
 *
 * Copyright (C) 2015 Sergey Denisov.
 * Written by Sergey Denisov aka LittleBuster (DenisovS21@gmail.com)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public Licence
 * as published by the Free Software Foundation; either version 3
 * of the Licence, or (at your option) any later version.
 *
 * Original library written by SkewPL, http://skew.tk
 */
  
#ifndef __NOKIA_5110_H__
#define __NOKIA_5110_H__
  
/*
 * LCD's port
 */
#define PORT_LCD PORTB
#define DDR_LCD DDRB
  
/*
 * LCD's pins
 */
#define LCD_SCE 0
#define LCD_RST 1
#define LCD_DC 2
#define LCD_DIN 3
#define LCD_CLK 4
  
#define LCD_CONTRAST 0x40
  
/*
 * Must be called once before any other function, initializes display
 */
void nokia_lcd_init(void);
  
/*
 * Clear screen
 */
void nokia_lcd_clear(void);
  
/**
 * Power of display
 * @lcd: lcd nokia struct
 * @on: 1 - on; 0 - off;
 */
void nokia_lcd_power(uint8_t on);
  
/**
 * Set single pixel
 * @x: horizontal pozition
 * @y: vertical position
 * @value: show/hide pixel
 */
void nokia_lcd_set_pixel(uint8_t x, uint8_t y, uint8_t value);
  
/**
 * Draw single char with 1-6 scale
 * @code: char code
 * @scale: size of char
 */
void nokia_lcd_write_char(char code, uint8_t scale);
  
/**
 * Draw string. Example: writeString("abc",3);
 * @str: sending string
 * @scale: size of text
 */
void nokia_lcd_write_string(const char *str, uint8_t scale);
  
/**
 * Set cursor position
 * @x: horizontal position
 * @y: vertical position
 */
void nokia_lcd_set_cursor(uint8_t x, uint8_t y);
  
/*
 * Render screen to display
 */
void nokia_lcd_render(void);
  
  
#endif
  
const uint8_t CHARSET[][5] PROGMEM = {
    { 0x00, 0x00, 0x00, 0x00, 0x00 }, // 20 space
    { 0x00, 0x00, 0x5f, 0x00, 0x00 }, // 21 !
    { 0x00, 0x07, 0x00, 0x07, 0x00 }, // 22 "
    { 0x14, 0x7f, 0x14, 0x7f, 0x14 }, // 23 #
    { 0x24, 0x2a, 0x7f, 0x2a, 0x12 }, // 24 $
    { 0x23, 0x13, 0x08, 0x64, 0x62 }, // 25 %
    { 0x36, 0x49, 0x55, 0x22, 0x50 }, // 26 &
    { 0x00, 0x05, 0x03, 0x00, 0x00 }, // 27 '
    { 0x00, 0x1c, 0x22, 0x41, 0x00 }, // 28 (
    { 0x00, 0x41, 0x22, 0x1c, 0x00 }, // 29 )
    { 0x14, 0x08, 0x3e, 0x08, 0x14 }, // 2a *
    { 0x08, 0x08, 0x3e, 0x08, 0x08 }, // 2b +
    { 0x00, 0x50, 0x30, 0x00, 0x00 }, // 2c ,
    { 0x08, 0x08, 0x08, 0x08, 0x08 }, // 2d -
    { 0x00, 0x60, 0x60, 0x00, 0x00 }, // 2e .
    { 0x20, 0x10, 0x08, 0x04, 0x02 }, // 2f /
    { 0x3e, 0x51, 0x49, 0x45, 0x3e }, // 30 0
    { 0x00, 0x42, 0x7f, 0x40, 0x00 }, // 31 1
    { 0x42, 0x61, 0x51, 0x49, 0x46 }, // 32 2
    { 0x21, 0x41, 0x45, 0x4b, 0x31 }, // 33 3
    { 0x18, 0x14, 0x12, 0x7f, 0x10 }, // 34 4
    { 0x27, 0x45, 0x45, 0x45, 0x39 }, // 35 5
    { 0x3c, 0x4a, 0x49, 0x49, 0x30 }, // 36 6
    { 0x01, 0x71, 0x09, 0x05, 0x03 }, // 37 7
    { 0x36, 0x49, 0x49, 0x49, 0x36 }, // 38 8
    { 0x06, 0x49, 0x49, 0x29, 0x1e }, // 39 9
    { 0x00, 0x36, 0x36, 0x00, 0x00 }, // 3a :
    { 0x00, 0x56, 0x36, 0x00, 0x00 }, // 3b ;
    { 0x08, 0x14, 0x22, 0x41, 0x00 }, // 3c <
    { 0x14, 0x14, 0x14, 0x14, 0x14 }, // 3d =
    { 0x00, 0x41, 0x22, 0x14, 0x08 }, // 3e >
    { 0x02, 0x01, 0x51, 0x09, 0x06 }, // 3f ?
    { 0x32, 0x49, 0x79, 0x41, 0x3e }, // 40 @
    { 0x7e, 0x11, 0x11, 0x11, 0x7e }, // 41 A
    { 0x7f, 0x49, 0x49, 0x49, 0x36 }, // 42 B
    { 0x3e, 0x41, 0x41, 0x41, 0x22 }, // 43 C
    { 0x7f, 0x41, 0x41, 0x22, 0x1c }, // 44 D
    { 0x7f, 0x49, 0x49, 0x49, 0x41 }, // 45 E
    { 0x7f, 0x09, 0x09, 0x09, 0x01 }, // 46 F
    { 0x3e, 0x41, 0x49, 0x49, 0x7a }, // 47 G
    { 0x7f, 0x08, 0x08, 0x08, 0x7f }, // 48 H
    { 0x00, 0x41, 0x7f, 0x41, 0x00 }, // 49 I
    { 0x20, 0x40, 0x41, 0x3f, 0x01 }, // 4a J
    { 0x7f, 0x08, 0x14, 0x22, 0x41 }, // 4b K
    { 0x7f, 0x40, 0x40, 0x40, 0x40 }, // 4c L
    { 0x7f, 0x02, 0x0c, 0x02, 0x7f }, // 4d M
    { 0x7f, 0x04, 0x08, 0x10, 0x7f }, // 4e N
    { 0x3e, 0x41, 0x41, 0x41, 0x3e }, // 4f O
    { 0x7f, 0x09, 0x09, 0x09, 0x06 }, // 50 P
    { 0x3e, 0x41, 0x51, 0x21, 0x5e }, // 51 Q
    { 0x7f, 0x09, 0x19, 0x29, 0x46 }, // 52 R
    { 0x46, 0x49, 0x49, 0x49, 0x31 }, // 53 S
    { 0x01, 0x01, 0x7f, 0x01, 0x01 }, // 54 T
    { 0x3f, 0x40, 0x40, 0x40, 0x3f }, // 55 U
    { 0x1f, 0x20, 0x40, 0x20, 0x1f }, // 56 V
    { 0x3f, 0x40, 0x38, 0x40, 0x3f }, // 57 W
    { 0x63, 0x14, 0x08, 0x14, 0x63 }, // 58 X
    { 0x07, 0x08, 0x70, 0x08, 0x07 }, // 59 Y
    { 0x61, 0x51, 0x49, 0x45, 0x43 }, // 5a Z
    { 0x00, 0x7f, 0x41, 0x41, 0x00 }, // 5b [
    { 0x02, 0x04, 0x08, 0x10, 0x20 }, // 5c backslash
    { 0x00, 0x41, 0x41, 0x7f, 0x00 }, // 5d ]
    { 0x04, 0x02, 0x01, 0x02, 0x04 }, // 5e ^
    { 0x40, 0x40, 0x40, 0x40, 0x40 }, // 5f _
    { 0x00, 0x01, 0x02, 0x04, 0x00 }, // 60 `
    { 0x20, 0x54, 0x54, 0x54, 0x78 }, // 61 a
    { 0x7f, 0x48, 0x44, 0x44, 0x38 }, // 62 b
    { 0x38, 0x44, 0x44, 0x44, 0x20 }, // 63 c
    { 0x38, 0x44, 0x44, 0x48, 0x7f }, // 64 d
    { 0x38, 0x54, 0x54, 0x54, 0x18 }, // 65 e
    { 0x08, 0x7e, 0x09, 0x01, 0x02 }, // 66 f
    { 0x0c, 0x52, 0x52, 0x52, 0x3e }, // 67 g
    { 0x7f, 0x08, 0x04, 0x04, 0x78 }, // 68 h
    { 0x00, 0x44, 0x7d, 0x40, 0x00 }, // 69 i
    { 0x20, 0x40, 0x44, 0x3d, 0x00 }, // 6a j
    { 0x7f, 0x10, 0x28, 0x44, 0x00 }, // 6b k
    { 0x00, 0x41, 0x7f, 0x40, 0x00 }, // 6c l
    { 0x7c, 0x04, 0x18, 0x04, 0x78 }, // 6d m
    { 0x7c, 0x08, 0x04, 0x04, 0x78 }, // 6e n
    { 0x38, 0x44, 0x44, 0x44, 0x38 }, // 6f o
    { 0x7c, 0x14, 0x14, 0x14, 0x08 }, // 70 p
    { 0x08, 0x14, 0x14, 0x18, 0x7c }, // 71 q
    { 0x7c, 0x08, 0x04, 0x04, 0x08 }, // 72 r
    { 0x48, 0x54, 0x54, 0x54, 0x20 }, // 73 s
    { 0x04, 0x3f, 0x44, 0x40, 0x20 }, // 74 t
    { 0x3c, 0x40, 0x40, 0x20, 0x7c }, // 75 u
    { 0x1c, 0x20, 0x40, 0x20, 0x1c }, // 76 v
    { 0x3c, 0x40, 0x30, 0x40, 0x3c }, // 77 w
    { 0x44, 0x28, 0x10, 0x28, 0x44 }, // 78 x
    { 0x0c, 0x50, 0x50, 0x50, 0x3c }, // 79 y
    { 0x44, 0x64, 0x54, 0x4c, 0x44 }, // 7a z
    { 0x00, 0x08, 0x36, 0x41, 0x00 }, // 7b {
    { 0x00, 0x00, 0x7f, 0x00, 0x00 }, // 7c |
    { 0x00, 0x41, 0x36, 0x08, 0x00 }, // 7d }
    { 0x10, 0x08, 0x08, 0x10, 0x08 }, // 7e ~
    { 0x00, 0x00, 0x00, 0x00, 0x00 } // 7f
};
  
static struct {
    /* screen byte massive */
    uint8_t screen[504];
  
    /* cursor position */
    uint8_t cursor_x;
    uint8_t cursor_y;
  
} nokia_lcd = {
    .cursor_x = 0,
    .cursor_y = 0
};
  
/**
 * Sending data to LCD
 * @bytes: data
 * @is_data: transfer mode: 1 - data; 0 - command;
 */
static void write(uint8_t bytes, uint8_t is_data)
{
    register uint8_t i;
    /* Enable controller */
    PORT_LCD &= ~(1 << LCD_SCE);
  
    /* We are sending data */
    if (is_data)
        PORT_LCD |= (1 << LCD_DC);
    /* We are sending commands */
    else
        PORT_LCD &= ~(1 << LCD_DC);
  
    /* Send bytes */
    for (i = 0; i < 8; i++) {
        /* Set data pin to byte state */
        if ((bytes >> (7-i)) & 0x01)
            PORT_LCD |= (1 << LCD_DIN);
        else
            PORT_LCD &= ~(1 << LCD_DIN);
  
        /* Blink clock */
        PORT_LCD |= (1 << LCD_CLK);
        PORT_LCD &= ~(1 << LCD_CLK);
    }
  
    /* Disable controller */
    PORT_LCD |= (1 << LCD_SCE);
}
  
static void write_cmd(uint8_t cmd)
{
    write(cmd, 0);
}
  
static void write_data(uint8_t data)
{
    write(data, 1);
}
  
/*
 * Public functions
 */
  
void nokia_lcd_init(void)
{
    register unsigned i;
    /* Set pins as output */
    DDR_LCD |= (1 << LCD_SCE);
    DDR_LCD |= (1 << LCD_RST);
    DDR_LCD |= (1 << LCD_DC);
    DDR_LCD |= (1 << LCD_DIN);
    DDR_LCD |= (1 << LCD_CLK);
  
    /* Reset display */
    PORT_LCD |= (1 << LCD_RST);
    PORT_LCD |= (1 << LCD_SCE);
    _delay_ms(10);
    PORT_LCD &= ~(1 << LCD_RST);
    _delay_ms(70);
    PORT_LCD |= (1 << LCD_RST);
  
    /*
     * Initialize display
     */
    /* Enable controller */
    PORT_LCD &= ~(1 << LCD_SCE);
    /* -LCD Extended Commands mode- */
    write_cmd(0x21);
    /* LCD bias mode 1:48 */
    write_cmd(0x13);
    /* Set temperature coefficient */
    write_cmd(0x06);
    /* Default VOP (3.06 + 66 * 0.06 = 7V) */
    write_cmd(0xC2);
    /* Standard Commands mode, powered down */
    write_cmd(0x20);
    /* LCD in normal mode */
    write_cmd(0x09);
  
    /* Clear LCD RAM */
    write_cmd(0x80);
    write_cmd(LCD_CONTRAST);
    for (i = 0; i < 504; i++)
        write_data(0x00);
  
    /* Activate LCD */
    write_cmd(0x08);
    write_cmd(0x0C);
}
  
void nokia_lcd_clear(void)
{
    register unsigned i;
    /* Set column and row to 0 */
    write_cmd(0x80);
    write_cmd(0x40);
    /*Cursor too */
    nokia_lcd.cursor_x = 0;
    nokia_lcd.cursor_y = 0;
    /* Clear everything (504 bytes = 84cols * 48 rows / 8 bits) */
    for(i = 0;i < 504; i++)
        nokia_lcd.screen[i] = 0x00;
}
  
void nokia_lcd_power(uint8_t on)
{
    write_cmd(on ? 0x20 : 0x24);
}
  
void nokia_lcd_set_pixel(uint8_t x, uint8_t y, uint8_t value)
{
    uint8_t *byte = &nokia_lcd.screen[y/8*84+x];
    if (value)
        *byte |= (1 << (y % 8));
    else
        *byte &= ~(1 << (y %8 ));
}
  
void nokia_lcd_write_char(char code, uint8_t scale)
{
    register uint8_t x, y;
  
    for (x = 0; x < 5*scale; x++)
        for (y = 0; y < 7*scale; y++)
            if (pgm_read_byte(&CHARSET[code-32][x/scale]) & (1 << y/scale))
                nokia_lcd_set_pixel(nokia_lcd.cursor_x + x, nokia_lcd.cursor_y + y, 1);
            else
                nokia_lcd_set_pixel(nokia_lcd.cursor_x + x, nokia_lcd.cursor_y + y, 0);
  
    nokia_lcd.cursor_x += 5*scale + 1;
    if (nokia_lcd.cursor_x >= 84) {
        nokia_lcd.cursor_x = 0;
        nokia_lcd.cursor_y += 7*scale + 1;
    }
    if (nokia_lcd.cursor_y >= 48) {
        nokia_lcd.cursor_x = 0;
        nokia_lcd.cursor_y = 0;
    }
}
  
void nokia_lcd_write_string(const char *str, uint8_t scale)
{
    while(*str)
        nokia_lcd_write_char(*str++, scale);
}
  
void nokia_lcd_set_cursor(uint8_t x, uint8_t y)
{
    nokia_lcd.cursor_x = x;
    nokia_lcd.cursor_y = y;
}
  
void nokia_lcd_render(void)
{
    register unsigned i;
    /* Set column and row to 0 */
    write_cmd(0x80);
    write_cmd(0x40);
  
    /* Write screen to display */
    for (i = 0; i < 504; i++)
        write_data(nokia_lcd.screen[i]);
}
  
//END 5100.C
