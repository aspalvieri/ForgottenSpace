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

	void setRender(SDL_Renderer*, SDL_Window*);

	void buildButton(int style, SDL_Rect size, std::string = "", int = 24);

	void handleEvents(SDL_Event *e);

	void draw();

	bool visible = true, released = false;

private:
	SDL_Rect mouseClip;
	std::string text = "";
	TTF_Font *font = NULL;
	LTexture textTexture;
	SDL_Rect textClip;
	SDL_Rect buttonSize;

	//SDL Handles
	SDL_Renderer *gRenderer = NULL;
	SDL_Window *gWindow = NULL;
};

class MessageBox
{
public:
	MessageBox();
	MessageBox(SDL_Renderer*, SDL_Window*);
	~MessageBox();

	void setRenders(SDL_Renderer*, SDL_Window*);

	void buildBox(int style, std::string t);
	void buildBigBox(int style, std::string n = "", std::string t = "", std::string f = "");

	void handleEvents(SDL_Event *e);

	void draw();

	void resetAll();

	bool yesClicked = false, noClicked = false; //yes counts as ok, these are click handles
	bool showyesno = false, showok = false;
	std::string tag = "";

private:
	Button Yes, No, Ok;
	std::string text, b1, b2, name = "", flavor = "";
	LTexture bg;
	LTexture textTexture, nameTexture, flavorTexture;
	TTF_Font* font;

	int boostx = (SCREEN_WIDTH - 450) / 2;
	int boosty = (SCREEN_HEIGHT - 200) / 2;

	//SDL Handles
	SDL_Renderer *gRenderer = NULL;
	SDL_Window *gWindow = NULL;
};

#endif//BUTTON_H