#pragma once

#include "color.h"

#include <string>

class Picture
{
public:
	Picture(unsigned int w, unsigned int h);
	Picture(std::string fileName);
	Picture();
    ~Picture();

	char* getCharPixels();

    inline int getWidth() const { return m_width; };
    inline int getHeight() const { return m_height; };

    void setColor(unsigned int x, unsigned int y, const ColorRGBA& c);
    ColorRGBA getColor(unsigned int x, unsigned int y) const;

	// I/O
	void readPNG(const std::string fileName);

private:
    int m_width;
    int m_height;
	float *m_pixels;

	int m_layers;

	bool m_initialized;

	std::string m_fileName;
};
