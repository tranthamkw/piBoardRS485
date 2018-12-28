/*

Program to convert a 128x128x24bit Device independent Bitmap into a data header file useable by 

a pic to display an image. 

*
*/

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <math.h>
#include "Bitmap.cpp"
#include <stdio.h>
#include <string.h>

using namespace std;



int main (int nNumberofArgs, char* argv[]){
   
FILE *fp;
char fileName[32];
unsigned int width,height,i,j,bpp;
unsigned int color;
unsigned char red, green, blue;
unsigned char pixel[3];


 
if (nNumberofArgs==3){
      cout<<"Number of Args "<<nNumberofArgs<<"\n";
      //load bitmap
      Bitmap* sourcebmap = new Bitmap(argv[1]);

	if(sourcebmap->loaded){


	height=sourcebmap->height;
	width=sourcebmap->width;
	bpp = sourcebmap->bpp;

	printf("height %d \twidth %d\tBPP %d\n",height,width,bpp);

	if (height==128 & width==128 & bpp==24){
		strcpy(fileName,argv[2]);
		fp=fopen(fileName,"w");
		fprintf(fp,"/* image is a 128x128 pixel array encoded  with 4 bits per color.\n");
fprintf(fp,"data:  image[i]=[RG], image[i+1]=[BR], image [i+2]=[GB] corresponding to pixel[m] and pixel[m+1]. There are 192 bytes per line and 128 lines.\n*/\n");
		fprintf(fp,"const char image[]={\n");
	for (i=0;i<height;i++){
		for(j=0;j<width;j++){
		color=sourcebmap->getPixel(i,j);
		blue = ((color&0xF00000)>>20);
		green = (color&0x00F000)>>12;
		red = (color&0x0000F0)>>4;
		//printf("%02X,%02X,%02X  ",red,green,blue);

		if ((j & 0x01) ==1){
		//odd
			pixel[1] = (pixel[1] & 0xF0) | red;
			pixel[2]= (green<<4) | blue;
//		printf("%02X%02X%02X,",pixel[0],pixel[1],pixel[2]);
		fprintf(fp,"%d,%d,%d",pixel[0],pixel[1],pixel[2]);

		if (i==127) {
			if (j==127){
				fprintf(fp,"};\n");}
		}else{
			if (j==127){
			fprintf(fp,",\n");
			} else {
			fprintf(fp,",");
			}
		}


		} else {
		//even
			pixel[0]=(red<<4) | green;
			pixel[1]= (pixel[1] & 0x0F) | (blue<<4);
		}
	      } // for j

	}// for i

	}// if height & width & bpp

	fclose(fp); 
} else {
printf("error loading bmp\n");
}
} else {

printf("Usage::  ./exportBMP <source.bmp> <imageheaderfile.h>\n");

}

return 0;
}
