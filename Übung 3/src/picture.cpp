

#include "picture.h"

#include <cmath>
#include <assert.h>
#include <cstdlib>

extern "C" 
{
	#include <png.h>
};

#include <iostream>

Picture::Picture(unsigned int w, unsigned int h): m_width (w), m_height (h), m_layers (4), m_pixels (NULL), m_initialized (false)
{
	m_pixels = new float[m_layers * w * h];
	m_fileName = "";
}

Picture::Picture(std::string fileName): m_pixels (NULL), m_layers(4), m_initialized (true)
{
	m_fileName = fileName;
	readPNG(fileName);
}

Picture::Picture(): m_width(0), m_height(0), m_layers(0), m_pixels(NULL), m_initialized (false)
{
	m_fileName = "";
}

Picture::~Picture()
{
	if (m_pixels)
	{
		delete [] m_pixels;
		m_pixels = NULL;
	}
}

char* Picture::getCharPixels()
{
	char* charPixels = new char[m_width * m_height * m_layers];
	char thePixelChannel;
	for (int i = 0; i < m_width * m_height * m_layers; i++)
	{
		thePixelChannel = static_cast<char>(m_pixels[i] * 255);
		charPixels[i] = thePixelChannel;
	}

	return charPixels;
};

void Picture::setColor(unsigned int x, unsigned int y, const ColorRGBA& c)
{
	// ensure correct range
	x = std::min((int)((int)this->m_width - 1), (int)std::max(0, (int)x));
	y = std::min((int)((int)this->m_height - 1), (int)std::max(0, (int)y));

	unsigned long idx = m_layers * (y * m_width + x);

	m_pixels[idx] = c.getR();
	m_pixels[idx + 1] = c.getG();
	m_pixels[idx + 2] = c.getB();
	m_pixels[idx + 3] = c.getA();
}

ColorRGBA Picture::getColor(unsigned int x, unsigned int y) const
{
	// ensure correct range
	x = std::min((int)((int)this->m_width - 1), (int)std::max(0, (int)x));
	y = std::min((int)((int)this->m_height - 1), (int)std::max(0, (int)y));

	unsigned long idx = m_layers * (y * m_width + x);

	float red = std::max(0.0f, std::min(1.0f, m_pixels[idx]));
	float green = std::max(0.0f, std::min(1.0f, m_pixels[idx + 1]));
	float blue = std::max(0.0f, std::min(1.0f, m_pixels[idx + 2]));
	float alpha = std::max(0.0f, std::min(1.0f, m_pixels[idx + 3]));

	ColorRGBA c(red, green, blue, alpha);

	return c;
}

void Picture::readPNG(const std::string fileName)
{
	char header[8];	// 8 is the maximum size that can be checked

	/* open file and test for it being a png */
	FILE *fp = fopen(fileName.c_str(), "rb");
	if (!fp)
	{
		assert(0);
	}
	fread(header, 1, 8, fp);

	/* initialize stuff */
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_ptr)
	{
		assert(0);
	}

	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		assert(0);
	}

	if (setjmp(png_jmpbuf(png_ptr)))
	{
		assert(0);
	}

	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, 8);

	png_read_info(png_ptr, info_ptr);

	this->m_width = info_ptr->width;
	this->m_height = info_ptr->height;
	//color_type = info_ptr->color_type;
	//bit_depth = info_ptr->bit_depth;

	//number_of_passes = png_set_interlace_handling(png_ptr);
	png_read_update_info(png_ptr, info_ptr);


	/* read file */
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		assert(0);
	}

	png_bytep *row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * m_height);
	for (int y = 0; y < m_height; y++)
	{
		row_pointers[y] = (png_byte*) malloc(info_ptr->rowbytes);
	}

	png_read_image(png_ptr, row_pointers);

	// renew m_pixels pointer
	if (this->m_pixels) delete [] this->m_pixels;
	this->m_pixels = new float[this->m_width * this->m_height * 4];

	this->m_pixels[0] = 1.0f;
	this->m_pixels[3] = 2.0f;

	for (int y = 0; y < m_height; y++)
	{
		png_byte* row = row_pointers[y];

		for (int x = 0; x < this->m_width; x++) 
		{
			png_byte* col = &(row[x*4]);
			//printf("Pixel at position (%d, %d) has the Color (%d, %d, %d, %d).\n", x, y, col[0], col[1], col[2], col[3]);

			this->setColor(x, m_height - 1 - y, ColorRGBA(col[0]/255.0f, col[1]/255.0f, col[2]/255.0f, col[3]/255.0f));
		}
	}

	fclose(fp);
}
