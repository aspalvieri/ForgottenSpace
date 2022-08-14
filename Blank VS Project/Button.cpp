#include "Button.h"

Button::Button() : Sprite()
{
}

Button::Button(SDL_Renderer *gR, SDL_Window *gW) : Sprite(gR, gW)
{
	gRenderer = gR;
	gWindow = gW;
}

Button::~Button()
{
}

void Button::setRender(SDL_Renderer *gR, SDL_Window *gW)
{
	Sprite::setRender(gR, gW);
	gRenderer = gR;
	gWindow = gW;
}

void Button::buildButton(int style, SDL_Rect size, std::string t, int fontSize)
{
	buttonSize = size;
	text = t;
	if (text != "")
	{
		font = TTF_OpenFont("bin/fonts/codenewroman.ttf", fontSize);
		textClip = { 0, 0, size.w - 14, size.h - 3 };
		textTexture.loadFromRenderedText(gRenderer, text, SDL_Color{ 255,255,255 }, font, buttonSize.w);
	}
	if (style == BUTTON_WIDE)
	{
		Sprite::loadSpriteImage("bin/images/buttonstyle1.png")
			.setFrameSize(100, 50)
			.pushFrame("Idle", 0, 0)
			.pushFrame("Hover", 0, 50)
			.pushFrame("Click", 0, 100);
	}
	else if (style == BUTTON_PLUS)
	{
		Sprite::loadSpriteImage("bin/images/buttonstyle2.png")
			.setFrameSize(50, 50)
			.pushFrame("Idle", 0, 0)
			.pushFrame("Hover", 0, 50)
			.pushFrame("Click", 0, 100);
	}
	else if (style == BUTTON_MINUS)
	{
		Sprite::loadSpriteImage("bin/images/buttonstyle5.png")
			.setFrameSize(50, 50)
			.pushFrame("Idle", 0, 0)
			.pushFrame("Hover", 0, 50)
			.pushFrame("Click", 0, 100);
	}
	else if (style == BUTTON_ARROW_LEFT)
	{
		Sprite::loadSpriteImage("bin/images/buttonstyle4.png")
			.setFrameSize(50, 50)
			.pushFrame("Idle", 0, 0)
			.pushFrame("Hover", 0, 50)
			.pushFrame("Click", 0, 100);
	}
	else if (style == BUTTON_ARROW_RIGHT)
	{
		Sprite::loadSpriteImage("bin/images/buttonstyle3.png")
			.setFrameSize(50, 50)
			.pushFrame("Idle", 0, 0)
			.pushFrame("Hover", 0, 50)
			.pushFrame("Click", 0, 100);
	}
	Sprite::setSize(size.w, size.h)
		.setPosition(size.x, size.y)
		.setAnimation("Idle");
}

void Button::handleEvents(SDL_Event * e)
{
	if (visible)
	{
		if (released)
			released = false;

		if (e->type == SDL_MOUSEMOTION)
		{
			SDL_GetMouseState(&mouseClip.x, &mouseClip.y);
			if (Sprite::getCurrentAnimation() != "Click" && checkCollision(mouseClip, Sprite::getBox()))
			{
				Sprite::setAnimation("Hover");
			}
			else if (Sprite::getCurrentAnimation() != "Click" || !checkCollision(mouseClip, Sprite::getBox()))
			{
				Sprite::setAnimation("Idle");
			}
		}

		if (e->type == SDL_MOUSEBUTTONDOWN)
		{
			if (e->button.button == SDL_BUTTON_LEFT)
			{
				if (checkCollision(mouseClip, Sprite::getBox()))
				{
					Sprite::setAnimation("Click");
				}
			}
		}

		if (e->type == SDL_MOUSEBUTTONUP)
		{
			if (e->button.button == SDL_BUTTON_LEFT)
			{
				if (Sprite::getCurrentAnimation() == "Click" && checkCollision(mouseClip, Sprite::getBox()))
				{
					Sprite::setAnimation("Hover");
					released = true;
				}
			}
		}

	}
}

void Button::draw()
{
	if (!visible)
	{
		visible = true;
		SDL_GetMouseState(&mouseClip.x, &mouseClip.y);
	}
	Sprite::draw();
	if (text != "")
	{
		textTexture.render(gRenderer, buttonSize.x + ((double)buttonSize.w / 2.0) - (double)textTexture.getWidth() / 2.0, buttonSize.y + 1.5 + ((double)buttonSize.h / 2.0) - (double)textTexture.getHeight() / 2.0);// , &textClip);
	}
}

MessageBox::MessageBox()
{
}

MessageBox::MessageBox(SDL_Renderer *gR, SDL_Window *gW)
{
	gRenderer = gR;
	gWindow = gW;
	Yes.setRender(gR, gW);
	No.setRender(gR, gW);
	Ok.setRender(gR, gW);
}

MessageBox::~MessageBox()
{
}

void MessageBox::setRenders(SDL_Renderer *gR, SDL_Window *gW)
{
	gRenderer = gR;
	gWindow = gW;
	Yes.setRender(gR, gW);
	No.setRender(gR, gW);
	Ok.setRender(gR, gW);
}

void MessageBox::buildBox(int style, std::string t)
{
	bg.loadFromFile("bin/images/tooltip.png", gRenderer);
	bg.setScale(450.0 / 192.0, 200.0 / 92.0);
	text = t;
	font = TTF_OpenFont("bin/fonts/codenewroman.ttf", 20);
	textTexture.loadFromRenderedText(gRenderer, text, SDL_Color{ 255,255,255 }, font, 430);
	if (style == MESSAGE_YESNO)
	{
		showyesno = true;
		Yes.buildButton(BUTTON_WIDE, SDL_Rect{ 40 + boostx, 140 + boosty, 100, 35 }, "Yes");
		No.buildButton(BUTTON_WIDE, SDL_Rect{ 310 + boostx, 140 + boosty, 100, 35 }, "No");
	}
	if (style == MESSAGE_OK)
	{
		showok = true;
		Ok.buildButton(BUTTON_WIDE, SDL_Rect{ 175 + boostx, 140 + boosty, 100, 35 }, "Ok");
	}
}

void MessageBox::buildBigBox(int style, std::string n, std::string t, std::string f) 
{
	bg.loadFromFile("bin/images/tooltip.png", gRenderer);
	bg.setScale(450.0 / 192.0, 200.0 / 92.0);
	text = t;
	name = n;
	flavor = f;
	font = TTF_OpenFont("bin/fonts/codenewroman.ttf", 14);
	textTexture.loadFromRenderedText(gRenderer, text, SDL_Color{ 255,255,255 }, font, 430);
	font = TTF_OpenFont("bin/fonts/codenewroman.ttf", 20);
	if (name != "") 
		nameTexture.loadFromRenderedText(gRenderer, name, SDL_Color{ 255,255,255 }, font, 430);
	if (flavor != "")
		flavorTexture.loadFromRenderedText(gRenderer, flavor, SDL_Color{ 255,255,255 }, font, 430);
	if (style == MESSAGE_YESNO)
	{
		showyesno = true;
		Yes.buildButton(BUTTON_WIDE, SDL_Rect{ 40 + boostx, 140 + boosty, 100, 35 }, "Yes");
		No.buildButton(BUTTON_WIDE, SDL_Rect{ 310 + boostx, 140 + boosty, 100, 35 }, "No");
	}
	if (style == MESSAGE_OK)
	{
		showok = true;
		Ok.buildButton(BUTTON_WIDE, SDL_Rect{ 175 + boostx, 140 + boosty, 100, 35 }, "Ok");
	}
}

void MessageBox::handleEvents(SDL_Event * e)
{
	if (showyesno)
	{
		Yes.handleEvents(e);
		No.handleEvents(e);
	}

	if (showok)
	{
		Ok.handleEvents(e);
	}

	if (e->type == SDL_KEYDOWN)
	{
		switch (e->key.keysym.sym)
		{
		case SDLK_ESCAPE:
			if (showyesno)
				noClicked = true;
			if (showok)
				yesClicked = true;
			return;
			break;
		}
	}

	if (Yes.released || Ok.released)
		yesClicked = true;
	else
		yesClicked = false;

	if (No.released)
		noClicked = true;
	else
		noClicked = false;
}

void MessageBox::draw()
{
	if (showyesno)
	{
		bg.render(gRenderer, 0 + boostx, 0 + boosty);
		Yes.draw();
		No.draw();
		int bonus = 0;
		if (name != "" && flavor != "")
			bonus = 5;
		textTexture.render(gRenderer, 20 + boostx, 15 + boosty + bonus);
		nameTexture.render(gRenderer, 20 + boostx, 15 + boosty);
		flavorTexture.render(gRenderer, 20 + boostx, 15 + boosty);
	}
	else if (showok)
	{
		bg.render(gRenderer, 0 + boostx, 0 + boosty);
		Ok.draw();
		int bonus = 0;
		if (name != "" && flavor != "")
			bonus = 5;
		textTexture.render(gRenderer, 20 + boostx, 15 + boosty + bonus);
		nameTexture.render(gRenderer, 20 + boostx, 15 + boosty);
		flavorTexture.render(gRenderer, 20 + boostx, 15 + boosty);
	}
}

void MessageBox::resetAll()
{
	yesClicked = false;
	noClicked = false;
	Yes.released = false;
	Yes.setAnimation("Idle");
	No.released = false;
	No.setAnimation("Idle");
	Ok.released = false;
	Ok.setAnimation("Idle");
	tag = "";
}
