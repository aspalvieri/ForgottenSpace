#ifndef SPRITE_H
#define SPRITE_H

#include "Includes.h"
#include "LTexture.h"

class Sprite
{
public:
	Sprite();
	Sprite(SDL_Renderer*, SDL_Window*);
	~Sprite();

	Sprite& setRender(SDL_Renderer*, SDL_Window*);

	/*  loadSpriteImage
		- Loads the specified image location
	*/
	Sprite& loadSpriteImage(std::string fileLocation);

	/* setFrameSize
		- Width and height of the Sprite's clip
	*/
	Sprite& setFrameSize(int width, int height);

	/* setPosition
		- X/Y coordinates of the Sprite
	*/
	Sprite& setPosition(double x, double y);

	/* getPosition
		- returns a pair of doubles of the position
	*/
	std::pair<double, double> getPosition();

	/* setSize
		- Width/Height to scale the Sprite by
	*/
	Sprite& setSize(double width, double height);

	/* getSize
		- returns a pair of doubles with the size
	*/
	std::pair<double, double> getSize();
	
	/* setAlpha
		- sets the transparency of the Sprite, 0 to 255
	*/
	Sprite& setAlpha(Uint8 a);

	/* pushFrameRow
		- animationID, the name of the animation
		- start X/Y, coordinates of the first frame
		- space X/Y, pixel offsets between each frame
		- numFrames, number of frames in the row
	*/
	Sprite& pushFrameRow(std::string animationID, int startX, int startY, int spaceX, int spaceY, int numFrames);

	/* pushFrame
		- animationID, the name of the animation
		- x/y, coordinates of the frame. Be sure to use setFrameSize() before this.
	*/
	Sprite& pushFrame(std::string animationID, int x, int y);

	/* setAnimation
		- sets the current animation to animationID
	*/
	Sprite& setAnimation(std::string animationID);

	Sprite& setAnimationRaw(std::string animationID);

	/* getCurrentAnimation
		- returns the string used in currentAnimation
	*/
	std::string getCurrentAnimation();

	/* getAnimationSize
		- returns the amount of frames in the current animation
	*/
	int getCurrentAnimationSize();

	/* setFrame
		- sets the frame that the animation will be on
	*/
	Sprite& setFrame(int frame);

	/* getFrame
		- returns the current frame number
	*/
	int getFrame();

	/* setAngle
		- sets the rotation of the Sprite
	*/
	Sprite& setAngle(double ang);

	/* setCenter
		- sets the X/Y pixel center of rotation of the Sprite
	*/
	Sprite& setCenter(int x, int y);

	/* getCenter
		- returns the SDL_Point of the center of rotation
	*/
	SDL_Point getCenter();

	/* setDelay
		- sets the delay between each frame update
		- needs to be atleast 1
	*/
	Sprite& setDelay(unsigned int time);

	/* resetTicks
		- resets the delay ticks
	*/
	Sprite& resetTicks();

	/* setCollide
		- True/False, sets if the sprite can be collided with
	*/
	Sprite& setCollide(bool col);

	/* getCollide
		- returns true/false of the collider
	*/
	bool getCollide();

	/* nextFrame
		- increase frame by 1
	*/
	Sprite& nextFrame();

	/* finishedAnimation
		- returns true if the Sprite has gone through all it's frames
	*/
	bool finishedAnimation();

	/* finishedFrame
		- returns true if the ticks equals the delay, meaning the frame has been used
	*/
	bool finishedFrame();

	/* draw
		- draws sprite to screen
	*/
	Sprite& draw();

	/* getBox
		- returns the bounding box of the Sprite
	*/
	SDL_Rect getBox();

	/* getSprite
		- returns this
	*/
	Sprite getSprite();

	Sprite& getSpriteReference();

	Sprite& setSprite(Sprite s);

	SDL_Rect* camera = NULL;

	SDL_Texture* getTexture();

	std::string spriteName = "";

private:
	std::unordered_map<std::string, LTexture> images;

	// a list of animations (only need to store 2 ints, x/y, because width/height are always the same)
	std::unordered_map<std::string, std::vector<std::pair<int, int>>> animations;

	// Variables relating to the Sprite
	std::pair<int, int> frameSize;
	std::pair<double, double> position;
	std::pair<double, double> scale;
	std::pair<double, double> scaleSize;
	std::string currentAnimation;
	SDL_Point center;
	SDL_Rect box;
	int currentFrame;
	int delay, ticks;
	double angle;
	bool collide;

	// This was the file location of the image we loaded
	std::string location;

	//SDL Handlers
	SDL_Window* gWindow = NULL;
	SDL_Renderer* gRenderer = NULL;
};

bool checkCollision(SDL_Rect &, SDL_Rect &);

bool checkCollision(Circle &, SDL_Rect &);

#endif // SPRITE_H
