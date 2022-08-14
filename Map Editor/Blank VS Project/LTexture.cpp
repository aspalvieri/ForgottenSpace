#include "LTexture.h"

LTexture::LTexture() {
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
	mPixels = NULL;
	scale = { 1.0, 1.0 };
	mPitch = 0;
}

LTexture::~LTexture() {
	//Deallocate
	//free();
}

bool LTexture::loadFromFile(std::string path, SDL_Renderer* gRenderer) {
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL) {
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else {
		//Convert surface to display format
		SDL_Surface* formattedSurface = SDL_ConvertSurfaceFormat(loadedSurface, SDL_PIXELFORMAT_RGBA8888, 0);
		if (formattedSurface == NULL) {
			printf("Unable to convert loaded surface to display format! %s\n", SDL_GetError());
		}
		else {
			//Create blank streamable texture
			newTexture = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, formattedSurface->w, formattedSurface->h);
			if (newTexture == NULL) {
				printf("Unable to create blank texture! SDL Error: %s\n", SDL_GetError());
			}
			else {
				//Enable blending on texture
				SDL_SetTextureBlendMode(newTexture, SDL_BLENDMODE_BLEND);

				//Lock texture for manipulation
				SDL_LockTexture(newTexture, &formattedSurface->clip_rect, &mPixels, &mPitch);

				//Copy loaded/formatted surface pixels
				memcpy(mPixels, formattedSurface->pixels, formattedSurface->pitch * formattedSurface->h);

				//Get image dimensions
				mWidth = formattedSurface->w;
				mHeight = formattedSurface->h;

				//Get pixel data in editable format
				Uint32* pixels = (Uint32*)mPixels;
				int pixelCount = (mPitch / 4) * mHeight;
				
				//Map colors
				Uint32 colorKey = SDL_MapRGB(formattedSurface->format, 255, 255, 255); //Checks for this color on pictures
				Uint32 transparent = SDL_MapRGBA(formattedSurface->format, 255, 255, 255, 0x00);

				//Color key pixels
				for (int i = 0; i < pixelCount; ++i) {
					if (pixels[i] == colorKey) {
						pixels[i] = transparent;
					}
				}

				//Unlock texture to update
				SDL_UnlockTexture(newTexture);
				mPixels = NULL;
			}

			//Get rid of old formatted surface
			SDL_FreeSurface(formattedSurface);
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}

bool LTexture::loadFromRenderedText(SDL_Renderer* gRenderer, std::string textureText, SDL_Color textColor, TTF_Font* gFont, int wrapLength) {
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Blended_Wrapped(gFont, textureText.c_str(), textColor, wrapLength);
	if (textSurface != NULL) {
		//Create texture from surface pixels
		mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
		if (mTexture == NULL) {
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		}
		else {
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface(textSurface);
	}
	else {
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}


	//Return success
	return mTexture != NULL;
}

bool LTexture::createBlank(int width, int height, SDL_TextureAccess access, SDL_Renderer* gRenderer) {
	//Create uninitialized texture
	mTexture = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGBA8888, access, width, height);
	if (mTexture == NULL) {
		printf("Unable to create blank texture! SDL Error: %s\n", SDL_GetError());
	}
	else {
		mWidth = width;
		mHeight = height;
	}

	return mTexture != NULL;
}

void LTexture::free() {
	//Free texture if it exists
	if (mTexture != NULL) {
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
		mPixels = NULL;
		mPitch = 0;
	}
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue) {
	//Modulate texture rgb
	SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::setScale(double w, double h)
{
	scale = { w, h };
}

void LTexture::setTexture(LTexture t)
{
	*this = t;
}

LTexture LTexture::getTexture()
{
	return *this;
}

void LTexture::setBlendMode(SDL_BlendMode blending) {
	//Set blending function
	SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha) {
	//Modulate texture alpha
	SDL_SetTextureAlphaMod(mTexture, alpha);
}

void LTexture::render(SDL_Renderer* gRenderer, int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip) {
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	//Set clip rendering dimensions
	if (clip != NULL) {
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	renderQuad.w = (int)(renderQuad.w * scale.first);
	renderQuad.h = (int)(renderQuad.h * scale.second);

	//Render to screen
	SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
}

void LTexture::setAsRenderTarget(SDL_Renderer* gRenderer) {
	//Make self render target
	SDL_SetRenderTarget(gRenderer, mTexture);
}

int LTexture::getWidth() {
	return mWidth;
}

int LTexture::getHeight() {
	return mHeight;
}

bool LTexture::lockTexture() {
	bool success = true;

	//Texture is already locked
	if (mPixels != NULL) {
		printf("Texture is already locked!\n");
		success = false;
	}
	//Lock texture
	else {
		if (SDL_LockTexture(mTexture, NULL, &mPixels, &mPitch) != 0) {
			printf("Unable to lock texture! %s\n", SDL_GetError());
			success = false;
		}
	}
	return success;
}

bool LTexture::unlockTexture() {
	bool success = true;

	//Texture is not locked
	if (mPixels == NULL) {
		printf("Texture is not locked!\n");
		success = false;
	}
	//Unlock texture
	else {
		SDL_UnlockTexture(mTexture);
		mPixels = NULL;
		mPitch = 0;
	}
	return success;
}

void* LTexture::getPixels() {
	return mPixels;
}

void LTexture::copyPixels(void* pixels) {
	//Texture is locked
	if (mPixels != NULL) {
		//Copy to locked pixels
		memcpy(mPixels, pixels, mPitch * mHeight);
	}
}

int LTexture::getPitch() {
	return mPitch;
}

Uint32 LTexture::getPixel32(unsigned int x, unsigned int y) {
	//Convert the pixels to 32 bit
	Uint32 *pixels = (Uint32*)mPixels;

	//Get the pixel requested
	return pixels[(y * (mPitch / 4)) + x];
}
