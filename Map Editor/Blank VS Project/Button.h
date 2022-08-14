#ifndef BUTTON_H
#define BUTTON_H

#include "Includes.h"
#include "GlobalVariables.h"
#include "Sprite.h"

class Button : public Sprite
{
public:
	Button();
	Button(SDL_Renderer*, SDL_Window*);
	~Button();

	void buildButton(int style, SDL_Rect size);

	void handleEvents(SDL_Event *e);

	void draw();

	bool visible = true, released = false;

private:
	SDL_Rect mouseClip;

	//SDL Handles
	SDL_Renderer *gRenderer = NULL;
	SDL_Window *gWindow = NULL;
};

#endif//BUTTON_H