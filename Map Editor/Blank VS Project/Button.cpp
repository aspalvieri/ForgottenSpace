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

void Button::buildButton(int style, SDL_Rect size)
{
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
				if (Sprite::getCurrentAnimation() == "Hover" && checkCollision(mouseClip, Sprite::getBox()))
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
		visible = true;
	Sprite::draw();
}
