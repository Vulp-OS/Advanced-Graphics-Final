#include "TGALoader.h"

#include <stdio.h>
#include <string.h>

using namespace std;
// Define targa header. This is only used locally.
#pragma pack(1)
typedef struct
{
    GLbyte	identsize;              // Size of ID field that follows header (0)
    GLbyte	colorMapType;           // 0 = None, 1 = paletted
    GLbyte	imageType;              // 0 = none, 1 = indexed, 2 = rgb, 3 = grey, +8=rle
    unsigned short	colorMapStart;          // First colour map entry
    unsigned short	colorMapLength;         // Number of colors
    unsigned char 	colorMapBits;   // bits per palette entry
    unsigned short	xstart;                 // image x origin
    unsigned short	ystart;                 // image y origin
    unsigned short	width;                  // width in pixels
    unsigned short	height;                 // height in pixels
    GLbyte	bits;                   // bits per pixel (8 16, 24, 32)
    GLbyte	descriptor;             // image descriptor
} TGAHEADER;
#pragma pack(8)
namespace NS_TGALOADER
{
	// cannot directly read this struct, since it would need to be tightly packed for that to work
/*	struct TGAHeader
	{
		unsigned char  	ucIdent;		// size of ID field that follows 18 byte header (0 usually)
		unsigned char  	ucColorMapType;	// type of colour map 0=none, 1=has palette
		unsigned char  	ucImageType;    // type of image 0=none,1=indexed,2=rgb,3=grey,+8=rle packed

		short 			colourmapstart;	// first colour map entry in palette
		short 			colourmaplength;// number of colours in palette
		unsigned char	colourmapbits;  // number of bits per palette entry 15,16,24,32

		short 			iXStart;  		// image x origin
		short 			iYStart;  		// image y origin
		short 			iWidth;   		// image width in pixels
		short 			iHeight;  		// image height in pixels
		unsigned char  	uiBits;			// image bits per pixel 8,16,24,32
		unsigned char  	uiDescriptor;	// image descriptor bits (vh flip bits)
	};*/
	////////////////////////////////////////////////////////////////////
// Allocate memory and load targa bits. Returns pointer to new buffer,
// height, and width of texture, and the OpenGL format of data.
// Call free() on buffer when finished!
// This only works on pretty vanilla targas... 8, 24, or 32 bit color
// only, no palettes, no RLE encoding.
GLbyte *gltReadTGABits(const char *szFileName, GLint *iWidth, GLint *iHeight, GLint *iComponents, GLenum *eFormat)
	{
    FILE *pFile;			// File pointer
    TGAHEADER tgaHeader;		// TGA file header
    unsigned long lImageSize;		// Size in bytes of image
    short sDepth;			// Pixel depth;
    GLbyte	*pBits = NULL;          // Pointer to bits
    
    // Default/Failed values
    *iWidth = 0;
    *iHeight = 0;
    *eFormat = GL_RGB;
    *iComponents = GL_RGB;
    
    // Attempt to open the file
    pFile = fopen(szFileName, "rb");
    if(pFile == NULL)
        return NULL;
	
    // Read in header (binary)
    fread(&tgaHeader, 18/* sizeof(TGAHEADER)*/, 1, pFile);
    
    // Do byte swap for big vs little endian
#ifdef __APPLE__
    LITTLE_ENDIAN_WORD(&tgaHeader.colorMapStart);
    LITTLE_ENDIAN_WORD(&tgaHeader.colorMapLength);
    LITTLE_ENDIAN_WORD(&tgaHeader.xstart);
    LITTLE_ENDIAN_WORD(&tgaHeader.ystart);
    LITTLE_ENDIAN_WORD(&tgaHeader.width);
    LITTLE_ENDIAN_WORD(&tgaHeader.height);
#endif
	
	
    // Get width, height, and depth of texture
    *iWidth = tgaHeader.width;
    *iHeight = tgaHeader.height;
    sDepth = tgaHeader.bits / 8;
    
    // Put some validity checks here. Very simply, I only understand
    // or care about 8, 24, or 32 bit targa's.
    if(tgaHeader.bits != 8 && tgaHeader.bits != 24 && tgaHeader.bits != 32)
        return NULL;
	
    // Calculate size of image buffer
    lImageSize = tgaHeader.width * tgaHeader.height * sDepth;
    
    // Allocate memory and check for success
    pBits = (GLbyte*)malloc(lImageSize * sizeof(GLbyte));
    if(pBits == NULL)
        return NULL;
    
    // Read in the bits
    // Check for read error. This should catch RLE or other 
    // weird formats that I don't want to recognize
    if(fread(pBits, lImageSize, 1, pFile) != 1)
		{
        free(pBits);
        return NULL;
		}
    
    // Set OpenGL format expected
    switch(sDepth)
		{
#ifndef OPENGL_ES
        case 3:     // Most likely case
            *eFormat = GL_BGR;
            *iComponents = GL_RGB;
            break;
#endif
        case 4:
            *eFormat = GL_BGRA;
            *iComponents = GL_RGBA;
            break;
        case 1:
			*eFormat = GL_LUMINANCE16F_ARB;
            *iComponents = GL_LUMINANCE16F_ARB;
            break;
        default:        // RGB
            // If on the iPhone, TGA's are BGR, and the iPhone does not 
            // support BGR without alpha, but it does support RGB,
            // so a simple swizzle of the red and blue bytes will suffice.
            // For faster iPhone loads however, save your TGA's with an Alpha!
#ifdef OPENGL_ES
    for(int i = 0; i < lImageSize; i+=3)
        {
        GLbyte temp = pBits[i];
        pBits[i] = pBits[i+2];
        pBits[i+2] = temp;
        }
#endif
        break;
		}
	
    
    
    // Done with File
    fclose(pFile);
	
    // Return pointer to image data
    return pBits;
	}

	bool LoadTGATexture(const char *szFileName, GLenum minFilter, GLenum magFilter, GLenum wrapMode)
{
	GLbyte *pBits;
	int nWidth, nHeight, nComponents;
	GLenum eFormat;
	
	// Read the texture bits
	pBits = gltReadTGABits(szFileName, &nWidth, &nHeight, &nComponents, &eFormat);
	if(pBits == NULL) 
		return false;
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
		
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB, nWidth, nHeight, 0,
				 eFormat, GL_UNSIGNED_BYTE, pBits);
	
    free(pBits);

    if(minFilter == GL_LINEAR_MIPMAP_LINEAR || 
       minFilter == GL_LINEAR_MIPMAP_NEAREST ||
       minFilter == GL_NEAREST_MIPMAP_LINEAR ||
       minFilter == GL_NEAREST_MIPMAP_NEAREST)
        glGenerateMipmap(GL_TEXTURE_2D);
            
	return true;
}
	IMAGE::IMAGE ()
	{
		m_iImageWidth = 0;
		m_iImageHeight = 0;
		m_iBytesPerPixel = 0;
	}

	bool IMAGE::LoadTGA (const char* szFile)
	{
		FILE* pFile = fopen (szFile, "rb");

		if (!pFile)
			return (false);

		// Read the header of the TGA, compare it with the known headers for compressed and uncompressed TGAs
		unsigned char ucHeader[18];
		fread (ucHeader, sizeof (unsigned char) * 18, 1, pFile);

		while (ucHeader[0] > 0)
		{
			--ucHeader[0];

			unsigned char temp;
			fread (&temp, sizeof (unsigned char), 1, pFile);
		}

		m_iImageWidth = ucHeader[13] * 256 + ucHeader[12];
		m_iImageHeight = ucHeader[15] * 256 + ucHeader[14];
		m_iBytesPerPixel = ucHeader[16] / 8;


		// check whether width, height an BitsPerPixel are valid
		if ((m_iImageWidth <= 0) || (m_iImageHeight <= 0) || ((m_iBytesPerPixel != 1) && (m_iBytesPerPixel != 3) && (m_iBytesPerPixel != 4)))
		{
			fclose (pFile);
			return (false);
		}

		// allocate the image-buffer
		m_Pixels.resize (m_iImageWidth * m_iImageHeight * 4);


		// call the appropriate loader-routine
		if (ucHeader[2] == 2)
		{
			LoadUncompressedTGA (pFile);
		}
		else 
		if (ucHeader[2] == 10)
		{
			LoadCompressedTGA (pFile);
		}
		else
		{
			fclose (pFile);
			return (false);
		}

		fclose (pFile);
		return (true);
	}

	void IMAGE::LoadUncompressedTGA (FILE* pFile)
	{
		unsigned char ucBuffer[4] = {255, 255, 255, 255};

		unsigned int* pIntPointer = (unsigned int*) &m_Pixels[0];
		unsigned int* pIntBuffer = (unsigned int*) &ucBuffer[0];

		const int iPixelCount	= m_iImageWidth * m_iImageHeight;

		for (int i = 0; i < iPixelCount; ++i)
		{
			fread (ucBuffer, sizeof (unsigned char) * m_iBytesPerPixel, 1, pFile);

			// if this is an 8-Bit TGA only, store the one channel in all four channels
			// if it is a 24-Bit TGA (3 channels), the fourth channel stays at 255 all the time, since the 4th value in ucBuffer is never overwritten
			if (m_iBytesPerPixel == 1)
			{
				ucBuffer[1] = ucBuffer[0];
				ucBuffer[2] = ucBuffer[0];
				ucBuffer[3] = ucBuffer[0];
			}

			// copy all four values in one operation
			(*pIntPointer) = (*pIntBuffer);
			++pIntPointer;
		}
	}

	void IMAGE::LoadCompressedTGA (FILE* pFile)
	{
		int iCurrentPixel	= 0;
		int iCurrentByte	= 0;
		unsigned char ucBuffer[4] = {255, 255, 255, 255};
		const int iPixelCount	= m_iImageWidth * m_iImageHeight;

		unsigned int* pIntPointer = (unsigned int*) &m_Pixels[0];
		unsigned int* pIntBuffer = (unsigned int*) &ucBuffer[0];

		do
		{
			unsigned char ucChunkHeader = 0;

			fread (&ucChunkHeader, sizeof (unsigned char), 1, pFile);

			if (ucChunkHeader < 128)
			{
				// If the header is < 128, it means it is the number of RAW color packets minus 1
				// that follow the header
				// add 1 to get number of following color values

				ucChunkHeader++;	

				// Read RAW color values
				for (int i = 0; i < (int) ucChunkHeader; ++i)	
				{
					fread (&ucBuffer[0], m_iBytesPerPixel, 1, pFile);

					// if this is an 8-Bit TGA only, store the one channel in all four channels
					// if it is a 24-Bit TGA (3 channels), the fourth channel stays at 255 all the time, since the 4th value in ucBuffer is never overwritten
					if (m_iBytesPerPixel == 1)
					{
						ucBuffer[1] = ucBuffer[0];
						ucBuffer[2] = ucBuffer[0];
						ucBuffer[3] = ucBuffer[0];
					}

					// copy all four values in one operation
					(*pIntPointer) = (*pIntBuffer);

					++pIntPointer;
					++iCurrentPixel;
				}
			}
			else // chunkheader > 128 RLE data, next color reapeated (chunkheader - 127) times
			{
				ucChunkHeader -= 127;	// Subteact 127 to get rid of the ID bit

				// read the current color
				fread (&ucBuffer[0], m_iBytesPerPixel, 1, pFile);

				// if this is an 8-Bit TGA only, store the one channel in all four channels
				// if it is a 24-Bit TGA (3 channels), the fourth channel stays at 255 all the time, since the 4th value in ucBuffer is never overwritten
				if (m_iBytesPerPixel == 1)
				{
					ucBuffer[1] = ucBuffer[0];
					ucBuffer[2] = ucBuffer[0];
					ucBuffer[3] = ucBuffer[0];
				}

				// copy the color into the image data as many times as dictated 
				for (int i = 0; i < (int) ucChunkHeader; ++i)
				{
					(*pIntPointer) = (*pIntBuffer);
					++pIntPointer;

					++iCurrentPixel;
				}
			}
		}
		while (iCurrentPixel < iPixelCount);
	}
}



