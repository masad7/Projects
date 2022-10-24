#include "image.h"


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
	if(c.pixels)
	{
		pixels = new Color[width*height];
		memcpy(pixels, c.pixels, width*height*sizeof(Color));
	}
}

//assign operator
Image& Image::operator = (const Image& c)
{
	if(pixels) delete pixels;
	pixels = NULL;

	width = c.width;
	height = c.height;
	if(c.pixels)
	{
		pixels = new Color[width*height*sizeof(Color)];
		memcpy(pixels, c.pixels, width*height*sizeof(Color));
	}
	return *this;
}

Image::~Image()
{
	if(pixels) 
		delete pixels;
}



//change image size (the old one will remain in the top-left corner)
void Image::resize(unsigned int width, unsigned int height)
{
	Color* new_pixels = new Color[width*height];
	unsigned int min_width = this->width > width ? width : this->width;
	unsigned int min_height = this->height > height ? height : this->height;

	for(unsigned int x = 0; x < min_width; ++x)
		for(unsigned int y = 0; y < min_height; ++y)
			new_pixels[ y * width + x ] = getPixel(x,y);

	delete pixels;
	this->width = width;
	this->height = height;
	pixels = new_pixels;
}

//change image size and scale the content
void Image::scale(unsigned int width, unsigned int height)
{
	Color* new_pixels = new Color[width*height];

	for(unsigned int x = 0; x < width; ++x)
		for(unsigned int y = 0; y < height; ++y)
			new_pixels[ y * width + x ] = getPixel((unsigned int)(this->width * (x / (float)width)), (unsigned int)(this->height * (y / (float)height)) );

	delete pixels;
	this->width = width;
	this->height = height;
	pixels = new_pixels;
}

Image Image::getArea(unsigned int start_x, unsigned int start_y, unsigned int width, unsigned int height)
{
	Image result(width, height);
	for(unsigned int x = 0; x < width; ++x)
		for(unsigned int y = 0; y < height; ++x)
		{
			if( (x + start_x) < this->width && (y + start_y) < this->height) 
				result.setPixel( x, y, getPixel(x + start_x,y + start_y) );
		}
	return result;
}

void Image::flipX()
{
	for(unsigned int x = 0; x < width * 0.5; ++x)
		for(unsigned int y = 0; y < height; ++y)
		{
			Color temp = getPixel(width - x - 1, y);
			setPixel( width - x - 1, y, getPixel(x,y));
			setPixel( x, y, temp );
		}
}

void Image::flipY()
{
	for(unsigned int x = 0; x < width; ++x)
		for(unsigned int y = 0; y < height * 0.5; ++y)
		{
			Color temp = getPixel(x, height - y - 1);
			setPixel( x, height - y - 1, getPixel(x,y) );
			setPixel( x, y, temp );
		}
}


//Loads an image from a TGA file
bool Image::loadTGA(const char* filename)
{
	unsigned char TGAheader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	unsigned char TGAcompare[12];
	unsigned char header[6];
	unsigned int bytesPerPixel;
	unsigned int imageSize;

	FILE * file = fopen(filename, "rb");
   	if ( file == NULL || fread(TGAcompare, 1, sizeof(TGAcompare), file) != sizeof(TGAcompare) ||
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
	if(pixels)
		delete pixels;

	width = tgainfo->width;
	height = tgainfo->height;
	pixels = new Color[width*height];

	//convert to float all pixels
	for(unsigned int y = 0; y < height; ++y)
		for(unsigned int x = 0; x < width; ++x)
		{
			unsigned int pos = y * width * bytesPerPixel + x * bytesPerPixel;
			this->setPixel(x , height - y - 1, Color( tgainfo->data[pos+2], tgainfo->data[pos+1], tgainfo->data[pos]) );
		}

	delete tgainfo->data;
	delete tgainfo;

	return true;
}

// Saves the image to a TGA file
bool Image::saveTGA(const char* filename)
{
	unsigned char TGAheader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	FILE *file = fopen(filename, "wb");
	if ( file == NULL )
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
	unsigned char* bytes = new unsigned char[width*height*3];
	for(unsigned int y = 0; y < height; ++y)
		for(unsigned int x = 0; x < width; ++x)
		{
			Color c = pixels[(height-y-1)*width+x];
			unsigned int pos = (y*width+x)*3;
			bytes[pos+2] = c.r;
			bytes[pos+1] = c.g;
			bytes[pos] = c.b;
		}

	fwrite(bytes, 1, width*height*3, file);
	fclose(file);
	return true;
}


FloatImage::FloatImage(unsigned int width, unsigned int height)
{
	this->width = width;
	this->height = height;
	pixels = new float[width*height];
	memset(pixels, 0, width * height * sizeof(float));
}

//copy constructor
FloatImage::FloatImage(const FloatImage& c) {
	pixels = NULL;

	width = c.width;
	height = c.height;
	if (c.pixels)
	{
		pixels = new float[width*height];
		memcpy(pixels, c.pixels, width*height * sizeof(float));
	}
}

//assign operator
FloatImage& FloatImage::operator = (const FloatImage& c)
{
	if (pixels) delete pixels;
	pixels = NULL;

	width = c.width;
	height = c.height;
	if (c.pixels)
	{
		pixels = new float[width*height * sizeof(float)];
		memcpy(pixels, c.pixels, width*height * sizeof(float));
	}
	return *this;
}

FloatImage::~FloatImage()
{
	if (pixels)
		delete pixels;
}


//change image size (the old one will remain in the top-left corner)
void FloatImage::resize(unsigned int width, unsigned int height)
{
	float* new_pixels = new float[width*height];
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


#ifndef IGNORE_LAMBDAS

//you can apply and algorithm for two images and store the result in the first one
//forEachPixel( img, img2, [](Color a, Color b) { return a + b; } );
template <typename F>
void forEachPixel(Image& img, const Image& img2, F f) {
	for(unsigned int pos = 0; pos < img.width * img.height; ++pos)
		img.pixels[pos] = f( img.pixels[pos], img2.pixels[pos] );
}

#endif

//*******************************************************IMPLEMENTACIÓ DE FUNCIONS********************************************************

//Funció per determinar si el número és 1 o -1.
int Image::sign(int num) {
	if (num < 1)
		num = -1;
	else
		num = 1;

	return num;
}

//Funció per dibuixar les linies de Bresenham. 
void Image::drawLineBresenham(int x0, int y0, int x1, int y1, Color c) {

	int dx, dy, inc_E, inc_NE, d, x, y, num, sumx, sumy;

	dx = x1 - x0;
	dy = y1 - y0;

	dx = abs(dx);
	dy = abs(dy);

	if (dx == 0) {
		num = sign(y1 - y0);
		while (y0 != y1) {
			y0 += num;
			setPixel(x0, y0, c);
		}
	}

	if (dy == 0) {
		num = sign(x1 - x0);
		while (x0 != x1) {
			x0 += num;
			setPixel(x0, y0, c);
		}
	}

	sumx = 0;
	sumy = 0;

	if (dx >= dy) { //Mirem si son linies horitzontals o verticales, horitzontals en aquest cas
		sumx += 1;
		num = sign(y1 - y0);

		if (x0 < x1) {
			x = x0;
			y = y0;
			sumy += num;
		}
		else {
			x = x1;
			x1 = x0;
			y = y1;
			sumy -= num;
		}

		setPixel(x, y, c);
		d = 2 * dy - dx;
		inc_E = 2 * dy;
		inc_NE = 2 * (dy - dx);

		while (x < x1) {
			if (d <= 0) {
				x += sumx;
				y = y;
				d += inc_E;
			}
			else {
				x += sumx;
				y += sumy;
				d += inc_NE;
			}

			setPixel(x, y, c);
		}
	}

	if (dx < dy) { //Mirem si son linies horitzontals o verticales, verticals en aquest cas verticals

		sumy += 1;
		num = sign(x1 - x0);

		if (y0 < y1) {
			y = y0;
			x = x0;
			sumx += num;
		}
		else {
			x = x1;
			y = y1;
			y1 = y0;
			sumx -= num;
		}


		setPixel(x, y, c);
		d = 2 * dx - dy;
		inc_E = 2 * dx;
		inc_NE = 2 * (dx - dy);

		while (y < y1) {
			if (d <= 0) {
				y += sumy;
				x = x;
				d += inc_E;
			}

			else {
				x += sumx;
				y += sumy;
				d += inc_NE;
			}

			setPixel(x, y, c);
		}
	}
}

//Funció per fer les línies de Bresenham amb una taula.
void Image::BresenhamwithTable(int x0, int y0, int x1, int y1, std::vector<sCelda>& table, Color c) {

	int dx, dy, inc_E, inc_NE, d, x, y, num, sumx, sumy;

	dx = x1 - x0;
	dy = y1 - y0;

	dx = abs(dx);
	dy = abs(dy);

	if (dx == 0) {
		num = sign(y1 - y0);

		int idy = y0;
		for (idy; idy <= y1; idy++)
		{

			if (x0 <= table[y0].minx) {
				table[y0].minx = x0;
			}

			if (x0 >= table[y0].maxx) {
				table[y0].maxx = x0;
			}
			y0 += num;

		}
	}



	if (dy == 0) {
		num = sign(x1 - x0);


		int idx = x0;
		for (idx; idx <= x1; idx++) {
			if (x0 <= table[y0].minx) {
				table[y0].minx = x0;
			}

			if (x0 >= table[y0].maxx) {
				table[y0].maxx = x0;
			}

			x0 += num;
		}


	}

	sumx = 0;
	sumy = 0;

	if (dx >= dy) { //Mirem si son linies horitzontals o verticales, horitzontals en aquest cas
		sumx += 1;
		num = sign(y1 - y0);

		if (x0 < x1) {
			x = x0;
			y = y0;
			sumy += num;
		}
		else {
			x = x1;
			x1 = x0;
			y = y1;
			sumy -= num;
		}


		d = 2 * dy - dx;
		inc_E = 2 * dy;
		inc_NE = 2 * (dy - dx);


		int idx = x;
		for (idx; idx <= x1; idx++) {
			if (x <= table[y].minx) {
				table[y].minx = x;
			}

			if (x >= table[y].maxx) {
				table[y].maxx = x;
			}

			if (d <= 0) {
				x += sumx;
				y = y;
				d += inc_E;
			}
			else {
				x += sumx;
				y += sumy;
				d += inc_NE;
			}
		}


	}

	if (dx < dy) { //Mirem si son linies horitzontals o verticales, verticals en aquest cas verticals

		sumy += 1;
		num = sign(x1 - x0);

		if (y0 < y1) {
			y = y0;
			x = x0;
			sumx += num;
		}
		else {
			x = x1;
			y = y1;
			y1 = y0;
			sumx -= num;
		}


		d = 2 * dx - dy;
		inc_E = 2 * dx;
		inc_NE = 2 * (dx - dy);

		int idy = y;
		for (idy; idy <= y1; idy++) {
			if (x <= table[y].minx) {
				table[y].minx = x;
			}

			if (x >= table[y].maxx) {
				table[y].maxx = x;
			}

			if (d <= 0) {
				y += sumy;
				x = x;
				d += inc_E;
			}
			else {
				x += sumx;
				y += sumy;
				d += inc_NE;
			}
		}


	}

}

//FUNCIÓ PER FER LA ACTIVITAT 3: funció que donats tres punts en el pla dibuixa un triangle
void Image::drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, Color color, bool fill) {

	if (fill == true) {

		//inicialitzem la taula
		std::vector<sCelda> table;
		table.resize(this->height);
		for (int i = 0; i < table.size(); i++) {
			table[i].minx = 100000;
			table[i].maxx = -100000;
		}

		if (y2 < y1) { //Ordenem els punts, per començar a dibuixar desde abaix.
			int y = y1;
			int x = x1;
			y1 = y2;
			x1 = x2;
			y2 = y;
			x2 = x;
		}

		if (y1 < y0) //ordenem els punts per poder-los dibuixar
		{
			int y = y0;
			int x = x0;
			y0 = y1;
			x0 = x1;
			y1 = y;
			x1 = x;
		}


		BresenhamwithTable(x0, y0, x1, y1, table, color);
		BresenhamwithTable(x0, y0, x2, y2, table, color);
		BresenhamwithTable(x1, y1, x2, y2, table, color);



		int j = y0;
		for (j; j <= y2; j++) {
			int r = table[j].minx;
			for (r; r <= table[j].maxx; r++) {
				setPixel(r, j, color);
			}

		}
	}

	else {
		drawLineBresenham(x0, y0, x1, y1, color);
		drawLineBresenham(x1, y1, x2, y2, color);
		drawLineBresenham(x0, y0, x2, y2, color);
	}

}

//FUNCIÓ PER FER LA ACTIVITAT 4: funció que donat tres punts i tres colors, ens dibuixa un triangle interpolat.
void Image::drawTriangleInterpolated(int x0, int y0, int x1, int y1, int x2, int y2, Color c0, Color c1, Color c2) {
	Vector2 P0, P1, P2, P;
	Vector2 v0, v1, v2;

	P0.x = x0;
	P0.y = y0;
	P1.x = x1;
	P1.y = y1;
	P2.x = x2;
	P2.y = y2;

	//inicialitzem la taula 
	std::vector<sCelda> table;
	table.resize(this->height);
	for (int i = 0; i < table.size(); ++i)
	{
		table[i].minx = 100000;
		table[i].maxx = -100000;
	}

	BresenhamwithTable(P0.x, P0.y, P1.x, P1.y, table, Color::BLUE);
	BresenhamwithTable(P0.x, P0.y, P2.x, P2.y, table, Color::BLUE);
	BresenhamwithTable(P1.x, P1.y, P2.x, P2.y, table, Color::BLUE);

	int j = P0.y;
	for (j; j <= P2.y; j++)
	{
		int r = table[j].minx;

		P.x = r;

		for (P.x; P.x <= table[j].maxx; P.x++) {
			v0.x = P1.x - P0.x;
			v0.y = P1.y - P0.y;

			v1.x = P2.x - P0.x;
			v1.y = P2.y - P0.y;
			v2.x = P.x - P0.x;
			v2.y = j - P0.y;

			float d00 = v0.dot(v0);
			float d01 = v0.dot(v1);
			float d11 = v1.dot(v1);
			float d20 = v2.dot(v0);
			float d21 = v2.dot(v1);
			float denom = d00 * d11 - d01 * d01;

			float v = (d11 * d20 - d01 * d21) / denom;
			float w = (d00 * d21 - d01 * d20) / denom;
			float u = 1.0 - v - w;

			Color c = c0 * u + c1 * v + c2 * w;

			setPixel(P.x, j, c);
		}
	}

}

//FUNCIÓ PER FER LA ACTIVITAT 5: funció que permet tractar les oclusions.
void Image::Zbuffer(const Vector3& p0, const Vector3& p1, const Vector3& p2, FloatImage *zbuffer) {

	//inicialitzem la taula
	std::vector<sCelda> table;
	table.resize(this->height);
	for (int i = 0; i < table.size(); i++) {
		table[i].minx = 100000; //very big number
		table[i].maxx = -100000; //very small number
	}

	for (int i = 0; i < table.size(); ++i)
	{
		table[i].minx = 100000;
		table[i].maxx = -100000;
	}

	BresenhamwithTable(p0.x, p0.y, p1.x, p1.y, table, Color::BLACK);
	BresenhamwithTable(p0.x, p0.y, p2.x, p2.y, table, Color::BLACK);
	BresenhamwithTable(p1.x, p1.y, p2.x, p2.y, table, Color::BLACK);

	Vector2 v0, v1, v2;
	v0.x = p1.x - p0.x;
	v0.y = p1.y - p0.y;

	v1.x = p2.x - p0.x;
	v1.y = p2.y - p0.y;

	float d00 = v0.dot(v0);
	float d01 = v0.dot(v1);
	float d11 = v1.dot(v1);

	float denom = d00 * d11 - d01 * d01;


	for (int idy = 0; idy < table.size(); idy++)
	{
		for (int idx = table[idy].minx; idx < table[idy].maxx; idx++)
		{

			float denom = d00 * d11 - d01 * d01;
			
			v2.x = idx - p0.x;
			v2.y = idy - p0.y;

			float d20 = v2.dot(v0);
			float d21 = v2.dot(v1);

			float v = (d11 * d20 - d01 * d21) / denom;
			float w = (d00 * d21 - d01 * d20) / denom;
			float u = 1.0 - v - w;

			Color c0, c1, c2, c;
			c0 = Color::RED;
			c1 = Color::GREEN;
			c2 = Color::BLUE;

			c = c0 * u + c1 * v + c2 * w;

			float z = (p0.z * u + p1.z * v + p2.z * w );

			if (idx < width && idy < height) {
				if (z < zbuffer->getPixel(idx, idy)) {
					setPixel(idx, idy, c);
					zbuffer->setPixel(idx, idy, z);

					
				}
			}

		}
	}

	

}

//FUNCIÓ PER FER LA ACTIVITAT 6: funció per fer la textura.
void Image::texturing(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector2& uv1, const Vector2& uv2, const Vector2& uv3, Image* texture, FloatImage* zbuffer) {

	//inicialitzem la taula
	std::vector<sCelda> table;
	table.resize(this->height);
	for (int i = 0; i < table.size(); i++) {
		table[i].minx = 100000; //very big number
		table[i].maxx = -100000; //very small number
	}

		for (int i = 0; i < table.size(); ++i)
	{
		table[i].minx = 100000;
		table[i].maxx = -100000;
	}

	BresenhamwithTable(p0.x, p0.y, p1.x, p1.y, table, Color::BLACK);
	BresenhamwithTable(p0.x, p0.y, p2.x, p2.y, table, Color::BLACK);
	BresenhamwithTable(p1.x, p1.y, p2.x, p2.y, table, Color::BLACK);

	Vector2 v0, v1, v2;
	v0.x = p1.x - p0.x;
	v0.y = p1.y - p0.y;

	v1.x = p2.x - p0.x;
	v1.y = p2.y - p0.y;

	float d00 = v0.dot(v0);
	float d01 = v0.dot(v1);
	float d11 = v1.dot(v1);

	float denom = d00 * d11 - d01 * d01;

	for (int idy = 0; idy < table.size(); idy++)
	{
		
		for (int idx = table[idy].minx; idx <= table[idy].maxx; idx++) {
			float denom = d00 * d11 - d01 * d01;

			v2.x = idx - p0.x;
			v2.y = idy - p0.y;

			float d20 = v2.dot(v0);
			float d21 = v2.dot(v1);

			float v = (d11 * d20 - d01 * d21) / denom;
			float w = (d00 * d21 - d01 * d20) / denom;
			float u = 1.0 - v - w;

			Vector2 uv = (uv1 * u + uv2 * v + uv3 * w);

			if (uv.x < texture->width && uv.y < texture->height && uv.x >= 0 && uv.y >= 0) {
				Color c = texture->getPixel(uv.x, uv.y);
				
				float z = (p0.z * u + p1.z * v + p2.z * w);

				if (idx < width - 1 && idy < height - 1) {
					if (z < zbuffer->getPixel(idx, idy)) {
						setPixel(idx, idy, c);
						zbuffer->setPixel(idx, idy, z);
					}
				}
			}

		}
		
	}
}

