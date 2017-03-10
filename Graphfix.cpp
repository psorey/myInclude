#include "stdafx.h"
#include <string.h>
#include "Graphfix.h"

// 2/01 disabled the 'fudgeWidth function...

int count = 0;

Rgb::Rgb(void)
{
	r = 0;
	g = 0;
	b = 0;
}


Rgb::Rgb(unsigned char _R, unsigned char _G, unsigned char _B)
{
    r = _R;
    g = _G;
    b = _B;
}

Rgba::Rgba(void)
{
	r = 0;
	g = 0;
	b = 0;
	a = 0;
}


Rgba::Rgba(unsigned char _R, unsigned char _G, unsigned char _B, unsigned char _A)
{
    r = _R;
    g = _G;
    b = _B;
	 a = _A;
}


Graphfix::Graphfix(void)
{
	fFudgeWidth = 0;
}

Graphfix::Graphfix(unsigned char* imagebuffer)
{
    fImageBuffer = (Rgb *)imagebuffer;
	 fFudgeWidth = 0;
}

void Graphfix::setBuffer(unsigned char * imagebuffer)
{
    fImageBuffer = (Rgb *)imagebuffer;
}

void Graphfix::setBuffer(Rgb* imagebuffer)
{
	fImageBuffer = imagebuffer;
}

Rgb* Graphfix::getBuffer(void)
{
	return fImageBuffer;
}

void Graphfix::setName(const char *file_name) 
{
    strcpy(fName, file_name);
}

char* Graphfix::getName(void) 
{
    return (fName);
}

void Graphfix::getSize(int &w, int &h) 
{
    w = fWidth + fFudgeWidth;
    h = fHeight;
}

void Graphfix::setSize(int w, int h)
{
	fWidth = w;
	fHeight = h;
}

int Graphfix::getNumImageBytes(void)
{
	TRACE("width = %d   height = %d   numChannels = %d\n", fWidth, fHeight, fNumChannels);
	return fWidth * fHeight * fNumChannels;	
}

bool Graphfix::openTarga(char *file_name) 
{
	if (file_name != NULL)
	  strcpy(fName, file_name);
	TRACE("Graphfix: filename = %s\n", fName);
	fp = fopen(file_name, "rb");
	if (fp == NULL) return FALSE;
	
	fseek(fp, (long)0, SEEK_SET);
	fread(&fTargaHeader, sizeof(fTargaHeader), 1, fp);
	/*
    fTargaHeader.char_num = getbyte(fp);
    fTargaHeader.map_type = getbyte(fp);
    fTargaHeader.image_type = getbyte(fp);
	 fTargaHeader.color_origin = getshort(fp);
	 fTargaHeader.color_length = getshort(fp);
    fTargaHeader.color_size = getbyte(fp);
	 fTargaHeader.image_origin_x = getshort(fp);
	 fTargaHeader.image_origin_y = getshort(fp);
	 fTargaHeader.image_width = getshort(fp);
	 fTargaHeader.image_height = getshort(fp);
    fTargaHeader.pixel_size = getbyte(fp);
    fTargaHeader.image_descriptor = getbyte(fp);
	*/
	fWidth = (int)fTargaHeader.image_width;
	// get fudge width...
	//int remainder = fWidth % 4;
	//if (remainder != 0)
	//	fFudgeWidth =  4 - remainder;
	//else
	//	fFudgeWidth = 0;

	fHeight = (int)fTargaHeader.image_height;
	TRACE("size = %d  %d\n", fTargaHeader.image_width, fTargaHeader.image_height);
	TRACE("image origin = %d  %d\n", fTargaHeader.image_origin_x, fTargaHeader.image_origin_y);
	TRACE("image type = %d\n", fTargaHeader.image_type);
	TRACE("id length = %d\n", fTargaHeader.char_num);
	//fWidth = fTargaHeader.image_width_hi * 0x100 + fTargaHeader.image_width_lo;
	//fHeight = fTargaHeader.image_height_hi * 0x100 + fTargaHeader.image_height_lo;
	if (fTargaHeader.image_type == 2) fNumChannels = 3;
	else if (fTargaHeader.image_type == 3) fNumChannels = 1;
	else fNumChannels = 3;
	return TRUE;
}


void Graphfix::openSgi(char *file_name)
{
    if (file_name != NULL)
        strcpy(fName, file_name);
    fp = fopen(fName, "rb");

	fSgiHeader.magic = getshort(fp);
	fread(&fSgiHeader.storage, 1, 1, fp);
	fread(&fSgiHeader.bytesPerChannel, 1, 1, fp);
	fSgiHeader.dimension = getshort(fp);
	fSgiHeader.xSize = getshort(fp);
	fSgiHeader.ySize = getshort(fp);
	fSgiHeader.zSize = getshort(fp);
	fSgiHeader.minPixelValue = getlong(fp);
	fSgiHeader.maxPixelValue = getlong(fp);

	fNumChannels = fSgiHeader.zSize;
	fIsRLE = fSgiHeader.storage;
	fWidth = fSgiHeader.xSize;
	fHeight = fSgiHeader.ySize;
	
	TRACE("num channels = %d\n", fNumChannels);
	TRACE("isRLE = %d\n", fSgiHeader.storage);
	TRACE("width = %d\n",fSgiHeader.xSize);
	TRACE("height = %d\n",fSgiHeader.ySize);
	TRACE("headerSize = %d\n", sizeof(fSgiHeader));
}

void Graphfix::readSgiTable(void)
{
}
   
void Graphfix::saveTarga(int width, int height, const char *file_name) 
{
    if (file_name != NULL)
        strcpy(fName, file_name);
    fp = fopen(fName, "wb");
    if (fp == NULL) return;

    fTargaHeader.char_num = 0x00;
    fTargaHeader.map_type = 0x00;
    fTargaHeader.image_type = 0x02;
    fTargaHeader.color_origin = 0x00;
    fTargaHeader.color_length = 0x0000;
    // !!!!! fTargaHeader.color_size = 0x00;
    fTargaHeader.image_origin_x = 0x00;
    fTargaHeader.image_origin_y = 0x00;
	 fTargaHeader.image_width = (WORD)fWidth + fFudgeWidth;  // save it as a multiple of four.
	 fTargaHeader.image_height = (WORD)fHeight;
    fTargaHeader.pixel_size = 0x18;  // ie, 24 bits per pixel: 8/8/8
    fTargaHeader.image_descriptor = 0x20;
    fwrite(&fTargaHeader, sizeof(fTargaHeader), 1, fp);

	 // Targa pixel format
	 // r g b r g b r g b r g b
	 // r g b r g b r g b r g b
	 // ...

    int size = (fWidth + fFudgeWidth) * fHeight;
    for (int i = 0; i < size; i++)
       this->write(fImageBuffer[i]);
    fclose(fp);
}

void Graphfix::saveSgi(int _width, int _height, char *filename) 
{
    fclose(fp);
    //if (file_name != NULL)
    strcpy(fName, filename);
    fp = fopen(fName, "wb");
    if (fp == NULL) return;

	 fWidth = _width;
	 fHeight = _height;
  
	 /*
	sgiHeader.magic = 474;
	sgiHeader.storage = 0;
	sgiHeader.bytesPerChannel = 1; // 1 byte per pixel;
	sgiHeader.dimension = 3;
	sgiHeader.xSize = width;
	sgiHeader.ySize = height;
	sgiHeader.zSize = 3;
	sgiHeader.minPixelValue = 0;
	sgiHeader.maxPixelValue = 255;
	sgiHeader.dummy = 0;
	sgiHeader.colormapId = 0;
	fwrite(&sgiHeader, sizeof(sgiHeader), 1, fp);
	*/

	putshort(fp, 474);
	putbyte(fp, 0);	// VERBATIM
	putbyte(fp, 1);
	putshort(fp, 3);
	putshort(fp, fWidth);
	putshort(fp, fHeight);
	putshort(fp, 3);
	putlong(fp, 0);
	putlong(fp, 255);
    int i;
	for( i = 0; i < 4; i++)
		putbyte(fp, 0);
	char iname[80];
	strcpy(iname, "No Name");
	fwrite(iname, 80, 1, fp);
	putlong(fp, 0);
	for (i = 0; i < 404; i++)
		putbyte(fp, 0);
  

	unsigned char (*buf) = new unsigned char[fWidth];
	int count = 0;
    int y;
    int x;
	for(y = 0; y < fHeight; y++)
	{
	  for (x = 0; x < fWidth; x++)
	  {
			buf[x]  = fImageBuffer[count].r;
			count++;
	  }    
	  fwrite(buf, fWidth, 1, fp);
	}

	count = 0;
	for(y = 0; y < fHeight; y++)
	{
	  for (x = 0; x < fWidth; x++)
	  {
			buf[x] = fImageBuffer[count].g;
			count++;
	  }    
	  fwrite(buf, fWidth, 1, fp);
	}

	count = 0;
	for(y = 0; y < fHeight; y++)
	{
	  for (x = 0; x < fWidth; x++)
	  {
			buf[x]  = fImageBuffer[count].b;
			count++;
	  }    
	  fwrite(buf, fWidth, 1, fp);
	}
	delete [] buf;
	fclose(fp);
}

//     SGI pixel format...
//     b b b b b b b b
//     b b b b b b b b
//     g g g g g g g g
//     g g g g g g g g 
//     r r r r r r r r
//     r r r r r r r r
//     

Rgb Graphfix::readRGB(void) 
{
    BYTE r, g, b;
    fread(&b, 1, 1, fp);
    fread(&g, 1, 1, fp);
    fread(&r, 1, 1, fp);
    return Rgb(r,g,b); // !!!
}

Rgb Graphfix::readGRAYSCALE(void)
{
    BYTE r, g, b;
    fread(&r, 1, 1, fp);
    fread(&g, 1, 1, fp);
    fread(&b, 1, 1, fp);
    return Rgb(r,g,b);
}

void Graphfix::readTarga(void) 
{
	fseek(fp, sizeof(fTargaHeader), SEEK_SET);  // !!!!
	int size = fWidth * fHeight;
	int count = 0;
	if (fNumChannels == 1)
	{
		size /= 3;
		for(int i = 0; i < size; i++) 
		{
			fImageBuffer[i] = this->readGRAYSCALE();
		}
	}
	else
	{
		// fixed this to make width multiple of four....
		for (int k = 0; k < fHeight; k++)
		{
            int i;
			for(i = 0; i < fWidth; i++) 
				fImageBuffer[count++] = this->readRGB();
			for(i = 0; i < fFudgeWidth; i++)
				fImageBuffer[count++] = Rgb(0, 0, 0);
		}
	}
	//for (int i = 0; i < count; i++)
	//	TRACE("count %d  r %d  g %d  b %d\n", i, fImageBuffer[i].r, fImageBuffer[i].g, fImageBuffer[i].b);
}

void Graphfix::readSgi(void) 
{
	if ( fIsRLE == 0)
	{
		fseek(fp, sizeof(fSgiHeader), SEEK_SET);
		int size = fWidth * fHeight;
		unsigned char buf;
        int i;
		for (i = 0; i < size; i++)
		{
			fread(&buf, 1, 1, fp);
			fImageBuffer[i].r = buf;
		}

		for (i = 0; i < size; i++)
		{
			fread(&buf, 1, 1, fp);
			fImageBuffer[i].g = buf;
		}

		for (i = 0; i < size; i++)
		{
			fread(&buf, 1, 1, fp);
			fImageBuffer[i].b = buf;
		}
		return;
	}
	if ( fIsRLE == 1)
	{
		TRACE("here\n");

		// read in the RLE tables just below the header...
		unsigned long *starttab, *lengthtab;
		unsigned long tablen = fHeight * fNumChannels;

		// arrays to hold table indexing values: 1 entry per row * number of channels...
		starttab = new unsigned long[tablen];
		lengthtab = new unsigned long[tablen];
		fseek(fp, 512, SEEK_SET);
		int numValues = fHeight * fNumChannels;
		TRACE ("numValues = %d\n", numValues);
        int i;
		for (i = 0; i < numValues; i++)
		{
			starttab[i] = getlong(fp);
			//TRACE("starttab[%d] = %d\n", i, starttab[i]);
		}	
		for (i = 0; i < numValues; i++)
		{
			lengthtab[i] = getlong(fp);
			//TRACE("lengthtab[%d] = %d\n", i, lengthtab[i]);
		}	


		//unsigned char *optr = (unsigned char*) imageBuffer;
		unsigned char *optr = new unsigned char[fWidth];
		unsigned char *iptr = new unsigned char[fWidth];
		int rowCount = 0;
		int channo = 0;

		// step through r rows, then g rows then b rows...
		for(int rowno = 0; rowno < numValues; rowno++)
		{
			//TRACE("row number = %d\n", rowno);
			unsigned long rleoffset = starttab[rowno];
			unsigned long rlelength = lengthtab[rowno];
			//TRACE("offset = %d\n", rleoffset);
			//TRACE("length = %d\n", rlelength);

			fseek(fp, rleoffset, SEEK_SET); 
			fread(iptr,	rlelength, 1, fp);
			optr = &optr[0];
			iptr = &iptr[0];
			expandRow(optr, iptr, 0);
			
			for(int k = 0; k < fWidth; k++)
			{
				if( channo == 0)
					fImageBuffer[(rowCount * fWidth) + k].r = optr[k];
				else if (channo == 1)
					fImageBuffer[(rowCount * fWidth) + k].g = optr[k];
				else if (channo == 2)
					fImageBuffer[(rowCount * fWidth) + k].b = optr[k];
			} 
			rowCount++;
			if (rowCount >= fHeight)
			{
				rowCount = 0;
				channo++;
				TRACE("channo = %d\n", channo);
			}
		}
	}
}

void Graphfix::expandRow(unsigned char *optr, unsigned char *iptr, int channo)
{
	unsigned char pixel, count;
	int pixelcount = 0;

	while(1)
	{
		pixel = *iptr++;
		if (! (count = (pixel & 0x7f))) 
			return;
		if (pixel & 0x80) 
		{
			while (count--) 
			{
				*optr = *iptr++;
				optr++;
			}
		} 
		else
		{
			pixel = *iptr++;
			while (count--)
			{
				*optr = pixel;
				optr++;
			}
		}
	} 
}

void Graphfix::write(const Rgb &rgb) 
{
    unsigned char r, g, b;
    //r = 0xFF - rgb.r * 0xFF; // !!!
    //g = 0xFF - rgb.g * 0xFF;
    //b = 0xFF - rgb.b * 0xFF;
	 r = rgb.r;
	 g = rgb.g;
	 b = rgb.b;
	 
    fwrite(&b, 1, 1, fp);
    fwrite(&g, 1, 1, fp);
    fwrite(&r, 1, 1, fp);
}

void Graphfix::close(void) 
{
    if (fp != NULL)
        fclose(fp);
    fp = NULL;
}

Graphfix::~Graphfix(void) {
}

void Graphfix::putbyte(FILE* outf, unsigned char val)
{
	unsigned char buf[1];
	buf[0] = val;
	fwrite( buf, 1, 1, outf);
}

void Graphfix::putshort(FILE* outf, unsigned short val)
{
	unsigned char buf[2];
	buf[0] = val >> 8;
	buf[1] = val >> 0;
	fwrite( buf, 2, 1, outf);
}

void Graphfix::putlong(FILE* outf, unsigned long val)
{
	unsigned char buf[4];
	buf[0] = val >> 24;
	buf[1] = val >> 16;
	buf[2] = val >> 8;
	buf[3] = val >> 0;
	fwrite( buf, 4, 1, outf);
}

unsigned char Graphfix::getbyte(FILE* inf)
{
	unsigned char buf[1];
	fread(buf, 1, 1, inf);
	return buf[0];
}

unsigned short Graphfix::getshort(FILE* inf)
{
	unsigned char buf[2];
	fread(buf, 2, 1, inf);
	return (buf[0] << 8) + (buf[1] << 0);
}

unsigned long Graphfix::getlong(FILE* inf)
{
	unsigned char buf[4];
	fread(buf, 4, 1, inf);
	return (buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + (buf[3] << 0);
}