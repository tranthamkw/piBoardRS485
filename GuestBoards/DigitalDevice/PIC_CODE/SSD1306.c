#include "SSD1306.h"


void delay(long n){
long k;
for (k = 0; k<n; k++);
}

void itoa (unsigned int value, char *result, unsigned int base,char minPos){
   char* ptr = result, *ptr1 = result, tmp_char;
    unsigned int tmp_value;
	char i=0;
 // check that the base if valid
    if (base < 2 || base > 17) { 
		*result = '\0';
		} else {
    		do {
        	tmp_value = value;
        	value /= base;
        	*ptr++ = "0123456789ABCDEF" [(tmp_value - value * base)];
		i++;
    	} while ( value );
 		
		while (i<minPos){
                *ptr++="0"[0];
                i++;                
            }
       *ptr-- = '\0';
	    while (ptr1 < ptr) {
    	    tmp_char = *ptr;
        	*ptr--= *ptr1;
        	*ptr1++ = tmp_char;
    	}
	}
}


void resetSSD1306(void){
// reset OLED
	RST=1;
	delay(50);
	RST=0;
	delay(500);// about 12ms
	RST=1;
// final state hi
}

void sendCommandSSD1306(unsigned char command){
	CS=1;
	DC=0;
	CS=0;
	writeSPI(command);
	CS=1;
}

void initSSD1306(void){
	initSPI();
	TRISC4 = 0; // OUTPUT C4 = RESET
	TRISC5 = 0; // OUTPUT C5 = DC
  	TRISC6 = 0; // CS bit an output
	resetSSD1306();
	sendCommandSSD1306(0xAE); // display off
	sendCommandSSD1306(0xD5); // setdisplayclockdiv
	sendCommandSSD1306(0x80);	// the sugguested ratio
	sendCommandSSD1306(0xA8); 	// setmultiplex
	sendCommandSSD1306((unsigned char)31); //lcdheight - 1
	sendCommandSSD1306(0xD3); // set display offset
	sendCommandSSD1306(0x00); // no offest
	sendCommandSSD1306(0x40);//SETSTARTLINE | 0x0
	sendCommandSSD1306(0x8D);//set CHARGEPUMP
	sendCommandSSD1306(0x14); //use internal 
	sendCommandSSD1306(0x20);//MEMORYMODE
	sendCommandSSD1306(0x00); //0x00  or 0x10
	sendCommandSSD1306(0xA0 | 0x1);//SEGREMAP 0xA0
	sendCommandSSD1306(0xC8);//COMSCANDEC 0xC8
	sendCommandSSD1306(0xDA);  //setcompins
	sendCommandSSD1306(0x02);
	sendCommandSSD1306(0x81);  //setcontrast
	sendCommandSSD1306(0x8F);

	sendCommandSSD1306(0xD9); // setprechard
	sendCommandSSD1306(0xF1);
	
	sendCommandSSD1306(0xDB);// setvcomdetect
	sendCommandSSD1306(0x40);
	sendCommandSSD1306(0xA4);//displayallonresume
	sendCommandSSD1306(0xA6); // normal display
	sendCommandSSD1306(0x2E); // deactivate scroll 0x2E  // left horz scroll 0x27
	sendCommandSSD1306(0xAF); // display on


}
/*
void invertDisplaySSD1306(unsigned char i){
	if (i==1){
	sendCommandSSD1306(0xA7);
	} else {
	sendCommandSSD1306(0xA6);
	}
}
*/
/*
void bargraphSSD1306(unsigned char line, unsigned short start, unsigned short end, unsigned short value, unsigned short maxvalue){

	unsigned short numCols = end-start;
	unsigned short valueCols,unitCols;
	valueCols = (numCols * value)/maxvalue;
	unitCols = numCols/maxvalue;
	unsigned short i;
	sendCommandSSD1306(0x21);//define SSD1306_COLUMNADDR 0x21
	sendCommandSSD1306(start); // column start address
	sendCommandSSD1306(end); // column end address 128 pixels wide
	sendCommandSSD1306(0x22); //#define SSD1306_PAGEADDR   0x22
	sendCommandSSD1306(line); // line start (page start)
	sendCommandSSD1306(3); // 0 to 3 = four pages of 8 bits = 32 bits
	CS=1;
	DC=1;
	CS=0;
	for (i=0;i<(numCols-1);i++) writeSPI(0x00); // clear the display area

	sendCommandSSD1306(start); // column start address
	sendCommandSSD1306(end); // column end address 128 pixels wide
	sendCommandSSD1306(0x22); //#define SSD1306_PAGEADDR   0x22
	sendCommandSSD1306(line); // line start (page start)
	sendCommandSSD1306(3); // 0 to 3 = four pages of 8 bits = 32 bits
	CS=1;
	DC=1;
	CS=0;
	for (i=0;i<(valueCols-1);i++) {
		if (i%unitCols ==0) {
		 writeSPI(0b01111110);// draw a bar graph proportional to value
		} else {
		writeSPI(0b00011100);
		}


	}
	CS=1;
}
*/


void clearDisplaySSD1306(void){
	unsigned short i;
	
	sendCommandSSD1306(0x22); //#define SSD1306_PAGEADDR   0x22
	sendCommandSSD1306(0); // line start (page start)
	sendCommandSSD1306(3); // 0 to 3 = four pages of 8 bits = 32 bits
	sendCommandSSD1306(0x21);//define SSD1306_COLUMNADDR 0x21
	sendCommandSSD1306(0); // column start address
	sendCommandSSD1306(127); // column end address 128 pixels wide
	CS=1;
	DC=1;
	CS=0;
	
	 for (i=0; i<((128*32)/8); i++){
			writeSPI(0x00); //change back to 0x00;
	} 
	CS=1;
}

void writeCharSSD1306(unsigned char line, unsigned char column, char* c, unsigned short length,unsigned char invert){
/*
row 0 to 3
font is 5 pixels wide. add one for blank space 127/6 = 21.16 columns
*/
	if (line>3) line = 3;
	if (column>21) column=21;
	unsigned short i,k;
	unsigned char temp;
	
	sendCommandSSD1306(0x21);//define SSD1306_COLUMNADDR 0x21
	sendCommandSSD1306(column * 6); // column start address
	sendCommandSSD1306(127); // column end address 128 pixels wide
	
	sendCommandSSD1306(0x22); //#define SSD1306_PAGEADDR   0x22
	sendCommandSSD1306(line); // line start (page start)
	sendCommandSSD1306(3); // end page = 0 to 3 = four pages of 8 bits = 32 bits
	
	CS=1;
	DC=1;
	CS=0;
	for (k=0;k<length;k++){
	for (i=0;i<5;i++){
		if ((c[k]>31)&(c[k]<127)){
		temp=myFont[(c[k]-32)*5+i]; // font only goes from ascii 32 to 126.
		if (invert) temp = ~temp;
		writeSPI(temp);
		}
	}
	temp=0x00;
	if (invert) temp = ~temp;
	writeSPI(temp); // blank column between characters.
	}
	CS=1;

}
