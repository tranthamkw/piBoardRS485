#include <iostream>
#include <cstdio>
#include <string>
using namespace std;

const short BITMAP_MAGIC_NUMBER=19778;
const int RGB_BYTE_SIZE=3;


typedef struct tagRGBQuad {
	char rgbBlue;
	char rgbGreen;
	char rgbRed;
	char rgbReserved;
} RGBQuad;



typedef struct tagBitmapFileHeader {
	unsigned short bfType;
	unsigned int bfSize;
	unsigned short bfReserved1;
	unsigned short bfReserved2;
	unsigned int bfOffBits;
} BitmapFileHeader;

typedef struct tagBitmapInfoHeader {
	unsigned int biSize;
	int biWidth;
	int biHeight;
	unsigned short biPlanes;
	unsigned short biBitCount;
	unsigned int biCompression;
	unsigned int biSizeImage;
	int biXPelsPerMeter;
	int biYPelsPerMeter;
	unsigned int biClrUsed;
	unsigned int biClrImportant;
} BitmapInfoHeader;



class Bitmap {
public:
    //variables
    bool loaded;
    int width;
    int height;
    unsigned short bpp;  // this should be 24 as only deal with 24bit
    int byteWidth;            //the width in bytes of the image
    int padWidth;             //the width in bytes of the added image

    //methods
    Bitmap(void);
    Bitmap(char *);
    Bitmap(int newwidth, int newheight, unsigned short newbpp);
    ~Bitmap();
    bool loadBMP(char *);
    bool saveBMP(char *);
    unsigned int getPixel(int row, int col);
    void setPixel(int row, int col, char red, char green, char blue);

private:
    //variables
    unsigned char *data;
    BitmapFileHeader bmfh;
    BitmapInfoHeader bmih;
    unsigned int dataSize;                //size of the data in the file
    //methods
    void reset(void);
 
};


