#define green_leds (char *) 0x000010a0
#define switches (volatile char *) 0x00001100

#define RS232_Control (*(volatile unsigned char *)(0x84000200))
#define RS232_Status  (*(volatile unsigned char *)(0x84000200))
#define RS232_TxData  (*(volatile unsigned char *)(0x84000202))
#define RS232_RxData  (*(volatile unsigned char *)(0x84000202))
#define RS232_Baud    (*(volatile unsigned char *)(0x84000204))

#define Touch_Control (*(volatile unsigned char *)(0x84000230))
#define Touch_Status  (*(volatile unsigned char *)(0x84000230))
#define Touch_Tx 	  (*(volatile unsigned char *)(0x84000232))
#define Touch_Rx      (*(volatile unsigned char *)(0x84000232))
#define Touch_Baud    (*(volatile unsigned char *)(0x84000234))

#include <stdlib.h>
#include <stdio.h>
#include "io.h"
#include "altera_up_avalon_character_lcd.h"

/*** FUNCTION PROTOTYPES ***/
void WAIT(void);
alt_up_character_lcd_dev * initLCD();

void init_RS232(void);
void init_Touch(void);

int putCharRS232(int c);
int getCharRS232(void);
int RS232TestForReceivedData(void);

int ScreenTouched(void);

/*** MAIN ***/
int main(void) {
	/* LCD */
	alt_up_character_lcd_dev * char_lcd_dev = initLCD();
	alt_up_character_lcd_string(char_lcd_dev, "^.^>Exercise 1.3");
	alt_up_character_lcd_set_cursor_pos(char_lcd_dev, 0, 1);
	alt_up_character_lcd_string(char_lcd_dev, "     Serial Port");

	/* Serial */
//	init_RS232();
	init_Touch();

	while (1) {
		*green_leds = *switches;

		if (ScreenTouched() == 1) {
			printf("YOU TOUCHED IT!\n");
		}
	}

	return 0;
}

/*** FUNCTION DEFINITIONS ***/

void WAIT()
{
	int i;
	for(i = 0; i < 300000; i++);
}

// initLCD - enables the use of the Altera LCD screen
alt_up_character_lcd_dev * initLCD()
{
	alt_up_character_lcd_dev * char_lcd_dev;
	char_lcd_dev = alt_up_character_lcd_open_dev("/dev/character_lcd_0");

	if(char_lcd_dev == NULL) {
		printf ("Error: could not open character LCD device\n");
	} else {
		printf ("< LCD Initialized >\n");
	}

	/* Initialize the character display */
	alt_up_character_lcd_init(char_lcd_dev);

	return char_lcd_dev;
}

// set up the 6850 Control Register
void init_RS232(void)
{
	RS232_Control = 0x15;
	RS232_Baud = 0x1; // 115k

	printf("< RS232 Initialized >\n");
}
void init_Touch(void)
{
	Touch_Control = 0x15;
	Touch_Baud = 0x7;  // 9600

	Touch_Tx = 0x13;  // disable
	WAIT();
	printf("Disabled: %x\n", Touch_Rx);
	Touch_Tx = 0x55;  // sync
	WAIT();
	printf("Sync: %x\n", Touch_Rx);
	Touch_Tx = 0x01;  // size
	WAIT();
	printf("Size: %x\n", Touch_Rx);
	Touch_Tx = 0x12;  // enable
	WAIT();
	printf("Enable: %x\n", Touch_Rx);

	printf("< Touch Screen Initialized >\n");
}

// poll Tx bit in 6850 status register. LOCKING
int putCharRS232(int c)
{
	while ( (RS232_Status & 0x2) == 0x0 );
	RS232_TxData = c;
	return c;
}

// poll Rx bit in 6850 status register. LOCKING
int getCharRS232( void )
{
	while ( (RS232_Status & 0x1 ) == 0x1 );
	return RS232_RxData;
}

// the following function polls the 6850 to determine if any character
// has been received. It doesn't wait for one, or read it, it simply tests
// to see if one is available to read
int RS232TestForReceivedData( void )
{
	// Test Rx bit in 6850 serial comms chip status register
	// if RX bit is set, return TRUE, otherwise return FALSE
	if ( (RS232_Status & 0x1) == 0x1 ) {
		return 1;
	} else {
		return 0;
	}
}



/*****************************************************************************
** test if screen touched
*****************************************************************************/
int ScreenTouched( void )
{
 // return TRUE if any data received from 6850 connected to touchscreen
 // or FALSE otherwise

	//If touch screen is touched, the first bit of byte 1 will be 1.
	while ( (Touch_Status & 0x1 ) != 0x1 );
	char firstByte = Touch_Rx;

	return ( (firstByte & 0x1) == 0x1 );


}
//****************************************************************************
//** wait for screen to be touched
//****************************************************************************
//void WaitForTouch()
//{
//	while(!ScreenTouched())
// ;
//}
//
//void WaitForRelease()
//{
//	while(ScreenTouched())
// ;
//}

///* a data type to hold a point/coord */
//typedef struct { int x, y; } Point ;
///*****************************************************************************
//* This function waits for a touch screen press event and returns X,Y coord
//*****************************************************************************/
//Point GetPress(void)
//{
// Point p1;
// // wait for a pen down command then return the X,Y coord of the point
// // calibrated correctly so that it maps to a pixel on screen
// WaitForTouch();
// int xVal = Tx[1];
// int yVal = Tx[3];
//
// int xExtension = Tx[2];
// int yExtension = Tx[4];
//
// xExtension = xExtension << 7;
// yExtension = yExtension << 7;
//
// xVal = xVal | xExtension;
// yVal = yVal | yExtension;
//
// p1.x = xVal;
// p1.y = yVal;
//
// return p1;
//}
///*****************************************************************************
//* This function waits for a touch screen release event and returns X,Y coord
//*****************************************************************************/
//Point GetRelease(void)
//{
// Point p1;
// // wait for a pen up command then return the X,Y coord of the point
// // calibrated correctly so that it maps to a pixel on screen
//
// WaitForRelease();
// int xVal = Tx[1];
// int yVal = Tx[3];
//
// int xExtension = Tx[2];
// int yExtension = Tx[4];
//
// xExtension = xExtension << 7;
// yExtension = yExtension << 7;
//
// xVal = xVal | xExtension;
// yVal = yVal | yExtension;
//
// p1.x = xVal;
// p1.y = yVal;
//
// return p1;
//}

