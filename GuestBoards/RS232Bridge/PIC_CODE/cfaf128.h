/* 
 * File:   cfaf128.h
 * Author: tranthamkw
 *
 * Created on June 30, 2018, 7:19 PM
 */

#ifndef CFAF128_H
#define	CFAF128_H

#ifdef	__cplusplus
extern "C" {
#endif

/*
 Display is Crystalfontz CFAF128128B1-0145T
  https://www.crystalfontz.com/product/cfaf128128b10145t

_________________
		| - 1 [LED Anode] --\/\/[160R]/\/---- +5V
		| - 2 [LED cathode] --- gnd
sitronix	| - 3 [spi4/!spi3] ---- +3.3v
ST7735		| - 4 [VDD] ----------- +3.3v
		| - 5 [VSS] ----------- gnd
		| - 6 [CS] ------------>GPIO CE0
		| - 7 [RESET] --------->GPIO 6
		| - 8 [SPI DATA]------->GPIO MISO
		| - 9 [SPI_CLK]-------->GPIO SCK
		| - 10 [RS = DC]------->GPIO 5

*/

void initLCD(void);
void Set_LCD_for_write_at_X_Y(char x, char y);
void Fill_LCD(char R, char G, char B);
//void Put_Pixel(char x, char y, char R, char G, char B);
void displayPixels(char* pixels, unsigned short y1);

void setScrollDefinition(char top_fix_height, char bottom_fix_height, char scroll_direction);
void verticalScroll(char vsp);

void delay(unsigned int time);



#ifdef	__cplusplus
}
#endif

#endif	/* CFAF128_H */

