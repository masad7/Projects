#include "image.h"

#include <math.h>


Image::Image() {
	width = 0; height = 0;
	pixels = NULL;
}

Image::Image(unsigned int width, unsigned int height)
{
	this->width = width;
	this->height = height;
	pixels = new Color[width*height];
	memset(pixels, 0, width * height * sizeof(Color));
}

//copy constructor
Image::Image(const Image& c) {
	pixels = NULL;

	width = c.width;
	height = c.height;
	if (c.pixels)
	{
		pixels = new Color[width*height];
		memcpy(pixels, c.pixels, width*height * sizeof(Color));
	}
}

//assign operator
Image& Image::operator = (const Image& c)
{
	if (pixels) delete pixels;
	pixels = NULL;

	width = c.width;
	height = c.height;
	if (c.pixels)
	{
		pixels = new Color[width*height * sizeof(Color)];
		memcpy(pixels, c.pixels, width*height * sizeof(Color));
	}
	return *this;
}

Image::~Image()
{
	if (pixels)
		delete pixels;
}



//change image size (the old one will remain in the top-left corner)
void Image::resize(unsigned int width, unsigned int height)
{
	Color* new_pixels = new Color[width*height];
	unsigned int min_width = this->width > width ? width : this->width;
	unsigned int min_height = this->height > height ? height : this->height;

	for (unsigned int x = 0; x < min_width; ++x)
		for (unsigned int y = 0; y < min_height; ++y)
			new_pixels[y * width + x] = getPixel(x, y);

	delete pixels;
	this->width = width;
	this->height = height;
	pixels = new_pixels;
}

//change image size and scale the content
void Image::scale(unsigned int width, unsigned int height)
{
	Color* new_pixels = new Color[width*height];

	for (unsigned int x = 0; x < width; ++x)
		for (unsigned int y = 0; y < height; ++y)
			new_pixels[y * width + x] = getPixel((unsigned int)(this->width * (x / (float)width)), (unsigned int)(this->height * (y / (float)height)));

	delete pixels;
	this->width = width;
	this->height = height;
	pixels = new_pixels;
}

Image Image::getArea(unsigned int start_x, unsigned int start_y, unsigned int width, unsigned int height)
{
	Image result(width, height);
	for (unsigned int x = 0; x < width; ++x)
		for (unsigned int y = 0; y < height; ++y)
		{
			if ((x + start_x) < this->width && (y + start_y) < this->height)
				result.setPixel(x, y, getPixel(x + start_x, y + start_y));
		}
	return result;
}

void Image::flipX()
{
	for (unsigned int x = 0; x < width * 0.5; ++x)
		for (unsigned int y = 0; y < height; ++y)
		{
			Color temp = getPixel(width - x - 1, y);
			setPixel(width - x - 1, y, getPixel(x, y));
			setPixel(x, y, temp);
		}
}

void Image::flipY()
{
	for (unsigned int x = 0; x < width; ++x)
		for (unsigned int y = 0; y < height * 0.5; ++y)
		{
			Color temp = getPixel(x, height - y - 1);
			setPixel(x, height - y - 1, getPixel(x, y));
			setPixel(x, y, temp);
		}
}


//Loads an image from a TGA file
bool Image::loadTGA(const char* filename)
{
	unsigned char TGAheader[12] = { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	unsigned char TGAcompare[12];
	unsigned char header[6];
	unsigned int bytesPerPixel;
	unsigned int imageSize;

	FILE * file = fopen(filename, "rb");
	if (file == NULL || fread(TGAcompare, 1, sizeof(TGAcompare), file) != sizeof(TGAcompare) ||
		memcmp(TGAheader, TGAcompare, sizeof(TGAheader)) != 0 ||
		fread(header, 1, sizeof(header), file) != sizeof(header))
	{
		std::cerr << "File not found: " << filename << std::endl;
		if (file == NULL)
			return NULL;
		else
		{
			fclose(file);
			return NULL;
		}
	}

	TGAInfo* tgainfo = new TGAInfo;

	tgainfo->width = header[1] * 256 + header[0];
	tgainfo->height = header[3] * 256 + header[2];

	if (tgainfo->width <= 0 || tgainfo->height <= 0 || (header[4] != 24 && header[4] != 32))
	{
		std::cerr << "TGA file seems to have errors or it is compressed, only uncompressed TGAs supported" << std::endl;
		fclose(file);
		delete tgainfo;
		return NULL;
	}

	tgainfo->bpp = header[4];
	bytesPerPixel = tgainfo->bpp / 8;
	imageSize = tgainfo->width * tgainfo->height * bytesPerPixel;

	tgainfo->data = new unsigned char[imageSize];

	if (tgainfo->data == NULL || fread(tgainfo->data, 1, imageSize, file) != imageSize)
	{
		if (tgainfo->data != NULL)
			delete tgainfo->data;

		fclose(file);
		delete tgainfo;
		return false;
	}

	fclose(file);

	//save info in image
	if (pixels)
		delete pixels;

	width = tgainfo->width;
	height = tgainfo->height;
	pixels = new Color[width*height];

	//convert to float all pixels
	for (unsigned int y = 0; y < height; ++y)
		for (unsigned int x = 0; x < width; ++x)
		{
			unsigned int pos = y * width * bytesPerPixel + x * bytesPerPixel;
			this->setPixel(x, height - y - 1, Color(tgainfo->data[pos + 2], tgainfo->data[pos + 1], tgainfo->data[pos]));
		}

	delete tgainfo->data;
	delete tgainfo;

	return true;
}

// Saves the image to a TGA file
bool Image::saveTGA(const char* filename)
{
	unsigned char TGAheader[12] = { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	FILE *file = fopen(filename, "wb");
	if (file == NULL)
	{
		fclose(file);
		return false;
	}

	unsigned short header_short[3];
	header_short[0] = width;
	header_short[1] = height;
	unsigned char* header = (unsigned char*)header_short;
	header[4] = 24;
	header[5] = 0;

	//tgainfo->width = header[1] * 256 + header[0];
	//tgainfo->height = header[3] * 256 + header[2];

	fwrite(TGAheader, 1, sizeof(TGAheader), file);
	fwrite(header, 1, 6, file);

	//convert pixels to unsigned char
	unsigned char* bytes = new unsigned char[width*height * 3];
	for (unsigned int y = 0; y < height; ++y)
		for (unsigned int x = 0; x < width; ++x)
		{
			Color c = pixels[(height - y - 1)*width + x];
			unsigned int pos = (y*width + x) * 3;
			bytes[pos + 2] = c.r;
			bytes[pos + 1] = c.g;
			bytes[pos] = c.b;
		}

	fwrite(bytes, 1, width*height * 3, file);
	fclose(file);
	return true;
}

#ifndef IGNORE_LAMBDAS

//you can apply and algorithm for two images and store the result in the first one
//forEachPixel( img, img2, [](Color a, Color b) { return a + b; } );
template <typename F>
void forEachPixel(Image& img, const Image& img2, F f) {
	for (unsigned int pos = 0; pos < img.width * img.height; ++pos)
		img.pixels[pos] = f(img.pixels[pos], img2.pixels[pos]);
}

#endif


//funció que dibuixa un rectangle
void Image::drawRectangle(int x, int y, int w, int h, Color color, bool fill) {
	int start_x = x;
	int start_y = y;

	if (fill == false) {
		for (int idx = start_x; idx < (start_x + w); ++idx)
		{
			setPixel(idx, start_y, color);
			setPixel(idx, start_y + h - 1, color);
		}

		for (int idy = start_y + 1; idy < (start_y + h - 1); ++idy)
		{
			setPixel(start_x, idy, color);
			setPixel(start_x + w - 1, idy, color);
		}
	}
	else {
		for (int idx = start_x; idx < (start_x + w); ++idx) {
			for (int idy = start_y; idy < (start_y + h); ++idy) {
				setPixel(idx, idy, color);
			}
		}
	}
}

//funció que dibuixa un cercle
void Image::drawCircle(int x, int y, int center, Color color, bool fill) {
	for (int idx = 0; idx < width; ++idx)
	{
		for (int idy = 0; idy < height; ++idy)
		{
			double x1 = pow(idx - x, 2);
			double y1 = pow(idy - y, 2);

			if ((x1 + y1) <= pow(center + 0.5, 2) && (x1 + y1) >= pow(center - 0.5, 2))
			{
				setPixel(idx, idy, color);
			}

		}
	}
}

//funció que dibuixa un línia
void Image::drawLine(int x1, int y1, int x2, int y2, Color color) { //eq. y = mx + b

	float m = 0; //pendent

	if (x2 == x1) {
		m = 0;
	}
	else {
		m = (float)(y2 - y1) / (x2 - x1);
	}

	int b = y1 - m * x1;


	//mirem quin és el punt més petit
	int x_first = x1;
	int x_last = x2;

	int y_first = y1;
	int y_last = y2;

	if (x1 > x2) {
		x_first = x2;
		x_last = x1;
	}

	if (y1 > y2) {
		y_first = y2;
		y_last = y1;
	}

	for (int idx = x_first; idx <= x_last; ++idx) {
		for (int idy = y_first; idy <= y_last; ++idy) {
			int line = m * idx + b;
			if (line == idy) {
				setPixel(idx, idy, color);
			}
		}
	}
}


//funció que ens fa el gradien des d'un color fins l'altre
void Image::gradient(Color color_start, Color color_final) {
	for (int x = 0; x < width; ++x)
	{
		for (int y = 0; y < height; ++y)
		{
			float f = x / (float)width;

			float red = (color_final.r - color_start.r - 1) * f - color_final.r;
			float green = (color_final.g - color_start.g - 1) * f - color_final.g;
			float blue = (color_final.b - color_start.b - 1) * f - color_final.b;

			setPixel(x, y, Color(red, green, blue));
		}
	}
}

//funció que ens fa el gradient en un punt central
void Image::gradientPoint() {
	float  x_center = width / 2;
	float  y_center = height / 2;

	float mod = sqrt(pow(x_center, 2) + pow(y_center, 2)); //calculem modul 

	for (int idx = 0; idx < width; ++idx) {
		for (int idy = 0; idy < height; ++idy) {
			float distance = sqrt(pow((x_center - idx), 2) + pow((y_center - idy), 2));
			float c = distance / mod;
			c = c * 255;
			setPixel(idx, idy, Color(c, c, c));
		}
	}
}