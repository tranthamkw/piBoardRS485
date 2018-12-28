/*
 * bitmap routines
 *
 * Only deal with 24 bit bitmaps.
 *
 *   general formula for getting pixel info
 *  int i=(col+(rows*(mywidth)))*3 + rows*offset;

    red = pB->data[i+2];
    green = pB->data[i+1];
    blue = pB->data[i];

 */


#include "Bitmap.h"

// put this in RRGGBB format
unsigned int Bitmap::getPixel(int row, int col){
    unsigned char red, green, blue;
    red = 0;
    green = 0;
    blue = 0;
    if ((row<height)&(col<width)&(row>0)&(col>0)){

       int i=(col+(row*(width)))*3 + row*(padWidth - byteWidth);
       red = data[i+2];
       green = data[i+1];
       blue = data[i];
    }
  return (red<<16|green<<8|blue);
}

void Bitmap::setPixel(int row, int col, char red, char green, char blue){
     if ((row<height)&(col<width)&(row>0)&(col>0)){
       int i=(col+(row*(width)))*3 + row*(padWidth - byteWidth);
       data[i+2]=red;
       data[i+1]=green;
       data[i]=blue;
    }
}

//basic constructor
Bitmap::Bitmap(){
    reset();
}


// constructor to make an empty bmp of size width X height X bpp
Bitmap::Bitmap(int newwidth, int newheight, unsigned short newbpp){
        reset();
    width=newwidth;
    height=newheight;
    bpp=newbpp;
     // bpp must be 24.
    //calculate the witdh of the final image in bytes
    byteWidth=padWidth=(width*3);
    //adjust the width for padding as necessary
    //
    while(padWidth%4!=0) {
        padWidth++;
    }
    int offset= padWidth-byteWidth;
    //calculate the size of the image data with padding
    dataSize=(width*height*3)+height*offset;

         //set up the data buffer for the image data
    data = new unsigned char[dataSize];
      // set up header information for new bmp... if it gets saved
    bmfh.bfType=BITMAP_MAGIC_NUMBER;
    bmfh.bfSize = dataSize+54;
    bmfh.bfReserved1=0;
    bmfh.bfReserved2=0;
    bmfh.bfOffBits =54;
    bmih.biBitCount=bpp;
    bmih.biClrImportant=0;
    bmih.biClrUsed=0;
    bmih.biCompression=0;
	bmih.biHeight=height;
    bmih.biPlanes=1;
	bmih.biSize=40;
    bmih.biSizeImage=dataSize;
	bmih.biWidth=width;
    bmih.biXPelsPerMeter=2834;   // not sure what this number is supposed to be. This works though
    bmih.biYPelsPerMeter=2834;
    // clear the data array.

    for (unsigned int i = 0; i!=dataSize; i++){
        data[i]=0x88; // this is essentially gray.
        }
}

//constructor loads the bitmap when it is created
Bitmap::Bitmap(char *file){
    reset();
    loaded = loadBMP(file);
}

//destructor
Bitmap::~Bitmap(){
    if(data!=0) {
        delete[] data;
    }
}


bool Bitmap::saveBMP(char *file){

    FILE *out;                 //file stream for reading

    //bitmap is not loaded yet
    loaded=false;
    //open the file for writing in binary mode
    out=fopen(file,"wb");
    //if the file does not exist return in error
    if(out==NULL) {

        fclose(out);
        return false;
    }

    /*
     * writing the entire BITMAPFILEHEADER results in an offset error
     * Individual components of the struc bmfh adds to 14 bytes, but
     * most compilers pad that to 16bytes.  this results in a 2byte shift
     * when reading the entire structure. Below, I suppose is the safest
     * way, reading in each part individually.
     */
    fwrite(&bmfh.bfType,sizeof(bmfh.bfType),1,out);
    fwrite(&bmfh.bfSize,sizeof(bmfh.bfSize),1,out);
    fwrite(&bmfh.bfReserved1,sizeof(bmfh.bfReserved1),1,out);
    fwrite(&bmfh.bfReserved2,sizeof(bmfh.bfReserved2),1,out);
    fwrite(&bmfh.bfOffBits,sizeof(bmfh.bfOffBits),1,out);
    fwrite(&bmih,sizeof(BitmapInfoHeader),1,out);
    fwrite(data,sizeof(char),dataSize,out);
    fclose(out);
    return true;
}

/*
 *
 *  load a bitmap from a file 
 *
 */
bool Bitmap::loadBMP(char *file) {

    FILE *in;                  //file stream for reading
    //unsigned char *tempData;       //temp storage for image data
    int numColours;            //total available colours
    //bitmap is not loaded yet
    loaded=false;

    //make sure memory is not lost
    if(data!=0) {
        delete[] data; // delete is a c++ operator
    }

    //open the file for reading in binary mode
    in=fopen(file,"rb");

    //if the file does not exist return in error
    if(in==NULL) {
        cout<<"File does not exist \n";
        fclose(in);
        return false;
    }

    /*
     * reading in the entire BITMAPFILEHEADER results in an offset error
     * Individual components of the struc bmfh adds to 14 bytes, but
     * most compilers pad that to 16bytes.  this results in a 2byte shift
     * when reading the entire structure. Below I suppose is the safest
     * way of reading in each part individually.
     */

    fread(&bmfh.bfType,sizeof(bmfh.bfType),1,in);
    fread(&bmfh.bfSize,sizeof(bmfh.bfSize),1,in);
    fread(&bmfh.bfReserved1,sizeof(bmfh.bfReserved1),1,in);
    fread(&bmfh.bfReserved2,sizeof(bmfh.bfReserved2),1,in);
    fread(&bmfh.bfOffBits,sizeof(bmfh.bfOffBits),1,in);

    // diagnostic information.  uncomment if needed
    cout << "bmfh.bfType      =" << bmfh.bfType << "\t";
    cout << "bmfh.bfSize      =" << bmfh.bfSize << "\n";
    cout << "bmfh.bfReserved1 =" << bmfh.bfReserved1<< "\t";
    cout << "bmfh.bfReserved2 =" << bmfh.bfReserved2<< "\n";
    cout << "bmfh.OffBits     =" << bmfh.bfOffBits << "\t";

    //check for the magic number that says this is a bitmap
    if(bmfh.bfType!=BITMAP_MAGIC_NUMBER) {
        cout<<"Not a Device Independent Bitmap \n";
        fclose(in);
        return false;
    }

    /*
     * read in the entire BITMAPINFOHEADER..
     * note that this stucture is already a multiple of 4 bytes so we
     * don't have to break this one up.
     */
 
    fread(&bmih,sizeof(BitmapInfoHeader),1,in);

    //save the width, height and bits per pixel for external use
    width=bmih.biWidth;
    height=bmih.biHeight;
    bpp=bmih.biBitCount;

     // some diagnostic information:
    cout << "sizeof(BitmapInfoHeader)=" << sizeof(BitmapInfoHeader) << endl;
    cout <<"\n";    
	cout << "biBitCount      =" << bmih.biBitCount << endl;
    cout << "biClrImportant  =" << bmih.biClrImportant << "\t";
	cout << "biClrUsed       =" << bmih.biClrUsed << endl;
    cout << "biCompression   =" << bmih.biCompression << "\t";
	cout << "biHeight        =" << bmih.biHeight << endl;
    cout << "biPlanes        =" << bmih.biPlanes << "\t";
	cout << "biSize          =" << bmih.biSize << endl;
    cout << "biSizeImage     =" << bmih.biSizeImage << "\t";
	cout << "biWidth         =" << bmih.biWidth << endl;
    cout << "biXPelsPerMeter =" << bmih.biXPelsPerMeter << "\t";
    cout << "biYPelsPerMeter =" << bmih.biYPelsPerMeter << endl;

    //calculate the witdh of the final image in bytes
    byteWidth=padWidth=(width*3);

    //adjust the width for padding as necessary
    //
    while(padWidth%4!=0) {
        padWidth++;
    }

    int offset= padWidth-byteWidth;
    //calculate the size of the image data with padding
    dataSize=(width*height*3)+height*offset;

    cout << "byteWidth       ="<<byteWidth<< "\t";
    cout << "padWidth        ="<<padWidth<< endl;
    cout << "offset          ="<<offset<<"\t";
    cout << "dataSize        =" << dataSize<< endl;

    //if the bitmap is not 24 bits per pixel
    //return in error
    if(bpp!=24) {
	 printf("Not 24 bit per pixel\n");
       fclose(in);
       return false;
    }
    //set up the data buffer for the image data
    data = new unsigned char[dataSize];

    //exit if there is not enough memory
    if(data==NULL) {
        fclose(in);
        return false;
    }
    //read in the entire image
    fread(data,sizeof(char),dataSize,in);
    //close the file now that we have all the info
    fclose(in);
    //return success
    return true;
}

//function to set the inital values
void Bitmap::reset(void) {
    loaded=false;
    data=0;

}


