/* 
 * File:   simpleGFX.h
 * Author: tranthamkw
 *
 * Created on June 30, 2018, 7:10 PM
 */

#ifndef SIMPLEGFX_H
#define	SIMPLEGFX_H

#ifdef	__cplusplus
extern "C" {
#endif

// index of color codes
// it.labelpartners.com/pantone_coated_table.html
// htmlcolorcodes.com
void initDisplay(void);
//void itoa(int value, char *result, int base);
void charToHex (unsigned int value, char *result, char numPos );

void writeText(char column, char* c, char start, char length, unsigned short fcolor, unsigned short bcolor);
void updateLine(char line);
void clearLine(unsigned short color);
void printLine(char*c, char len, unsigned short fcolor, unsigned short bcolor);

#define RED 0xF00
#define BLUE 0x00F
#define CYAN 0x0FF
#define GREEN 0x0F0
#define WHITE 0xFFF
#define BLACK 0x000
#define YELLOW 0xFF0
/*
#define RED 0xFF0000
#define GREEN 0x00FF00
#define BLUE 0x0000FF
#define WHITE 0xFFFFFF
#define BLACK 0x0
#define YELLOW 0xFFFF00
#define SILVER 0xC0C0C0
#define MAGENTA 0xFF00FF
#define CYAN 0x00FFFF
 */
    



#ifdef	__cplusplus
}
#endif

#endif	/* SIMPLEGFX_H */

