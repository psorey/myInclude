#ifndef _GRAPHFIX_
#define _GRAPHFIX_

#include <stdio.h>

struct TargaHeader 
{
    BYTE char_num;
    BYTE map_type;
    BYTE image_type;
    //BYTE color_origin_lo;
    //BYTE color_origin_hi;
    //BYTE color_length_lo;
    //BYTE color_length_hi;
	 WORD color_origin;
	 WORD color_length;
    //BYTE color_size;
    //BYTE image_origin_x_lo;
    //BYTE image_origin_x_hi;
    //BYTE image_origin_y_lo;
    //BYTE image_origin_y_hi;
	 WORD image_origin_x;
	 WORD image_origin_y;
	 WORD image_width;
	 WORD image_height;
    //BYTE image_width_lo;
    //BYTE image_width_hi; 
    //BYTE image_height_lo;
    //BYTE image_height_hi;
    BYTE pixel_size;
    BYTE image_descriptor;
};

struct SgiHeader 
{
	unsigned short	magic;
	char				storage;
	char				bytesPerChannel;
	unsigned short	dimension;
	unsigned short	xSize; // have to reverse bytes for Intel
	unsigned short	ySize;
	unsigned short	zSize;
	unsigned long	minPixelValue;
	unsigned long	maxPixelValue;
	long				dummy;
	char 				imageName[80];
	long				colormapId;
	char				dummy2[404];
};


class Rgb 
{
public:    
    unsigned char r;
    unsigned char g;
    unsigned char b;

    Rgb(unsigned char r, unsigned char g, unsigned char b);
	 Rgb(void);
};


class Rgba
{
public:
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;

	Rgba(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	Rgba(void);
};

class Graphfix 
{
private:
	char			fName[100];
	FILE			*fp;
	Rgb*			fImageBuffer;
	TargaHeader fTargaHeader;
	SgiHeader	fSgiHeader; // SGI format
	int			fWidth;
	int			fFudgeWidth; // number of pixels added to fWidth to be a multiple of four.
	int			fHeight;
	int			fIsRLE;
	int			fNumChannels;

	Rgb				readRGB(void);
	Rgb				readGRAYSCALE(void);
	void				putbyte(FILE* outf, unsigned char val);
	void				putshort(FILE* outf, unsigned short val);
	void				putlong(FILE* outf, unsigned long val);
	unsigned char	getbyte(FILE* inf);
	unsigned short getshort(FILE* fp);
	unsigned long  getlong(FILE* fp);

public:
	Graphfix(void);
	Graphfix(unsigned char *imagebuffer);
	void setName(const char *file_name);
	void setBuffer(unsigned char* buffer);
	void setBuffer(Rgb* buffer);
	Rgb* getBuffer(void);
	char *getName(void);
	void getSize(int &w, int &h);
	void setSize(int w, int h);
	int  getNumImageBytes(void);
	bool openTarga(char *file_name = NULL);
	void saveTarga(int width, int height, const char *file_name = NULL);
	void openSgi(char *file_name = NULL);
	void saveSgi(int width, int height, char *filename);
	void openBmp(char *file_name = NULL);
	void saveBmp(int width, int height, char *file_name = NULL);
	void expandRow(unsigned char *optr, unsigned char *iptr, int z);
	void readTarga(void);
	void readSgi(void);
	void readSgiTable(void);
	void write(const Rgb &rgb);
	void close(void);
	virtual ~Graphfix(void);

	// 
};

#endif