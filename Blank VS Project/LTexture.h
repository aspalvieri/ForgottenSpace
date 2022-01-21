#ifndef LTEXTURE_H
#define LTEXTURE_H

#include "Includes.h"

class LTexture
{
public:
	//Initializes variables
	LTexture();

	//Deallocates memory
	~LTexture();

	//Loads image at specified path
	bool loadFromFile(std::string path, SDL_Renderer* gRenderer);

	//Creates image from font string
	bool loadFromRenderedText(SDL_Renderer* gRenderer, std::string textureText, SDL_Color textColor, TTF_Font* gFont, int wrapLength);

	//Creates blank texture
	bool createBlank(int width, int height, SDL_TextureAccess = SDL_TEXTUREACCESS_STREAMING, SDL_Renderer* gRenderer = NULL);

	//Deallocates texture
	void free();

	//Set color modulation
	void setColor(Uint8 red, Uint8 green, Uint8 blue);

	//Sets the scale of the texture
	void setScale(double w, double h);

	void setTexture(LTexture t);

	LTexture getTexture();

	//Set blending
	void setBlendMode(SDL_BlendMode blending);

	//Set alpha modulation
	void setAlpha(Uint8 alpha);

	//Renders texture at given point
	void render(SDL_Renderer* gRenderer = NULL, int x = 0, int y = 0, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

	//Set self as render target
	void setAsRenderTarget(SDL_Renderer* gRenderer);

	//Gets image dimensions
	int getWidth();
	int getHeight();

	//Pixel manipulators
	bool lockTexture();
	bool unlockTexture();
	void* getPixels();
	void copyPixels(void* pixels);
	int getPitch();
	Uint32 getPixel32(unsigned int x, unsigned int y);

	//The actual hardware texture
	SDL_Texture* mTexture;

private:
	std::pair<double, double> scale;
	void* mPixels;
	int mPitch;

	std::string storedText = "";

	//Image dimensions
	int mWidth;
	int mHeight;
};

#endif // LTEXTURE_H
