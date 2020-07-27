#include "avancezlib.h"
#include "stdio.h"

// Creates the main window. Returns true on success.
bool AvancezLib::init(int width, int height, const char* name)
{
	SDL_Log("Initializing the system...\n");

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL failed the initialization: %s\n", SDL_GetError());
		return false;
	}

	//Create window
	window = SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
	if (window == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return false;
	}
	
	//Create renderer for window
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Renderer could not be created! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	TTF_Init();
	font = TTF_OpenFont("Assets/Fonts/ArcadeClassic.ttf", 28); //this opens a font style and sets a size
	if (font == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "font cannot be created! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	// initialize the keys
	key.fire = false;	key.left = false;	key.right = false;

	//Initialize renderer color
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

	//Clear screen
	SDL_RenderClear(renderer);

	SDL_Log("System up and running...\n");
	return true;
}


// Destroys the avancez library instance
void AvancezLib::destroy()
{
	SDL_Log("Shutting down the system\n");

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	TTF_CloseFont(font);

	TTF_Quit();
	SDL_Quit();
}


bool AvancezLib::update()
{
	bool go_on = true;
	SDL_Event event;

	//add additional Keys here
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
			go_on = false;

		if (event.type == SDL_KEYDOWN)
		{
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
			case SDLK_q:
				go_on = false;
				break;
			case SDLK_SPACE:
				key.fire = true;
				break;
			case SDLK_LEFT:
				key.left = true;
				break;
			case SDLK_RIGHT:
				key.right = true;
				break;
			}
		}

		if (event.type == SDL_KEYUP)
		{
			switch (event.key.keysym.sym)
			{
			case SDLK_SPACE:
				key.fire = false;
				break;
			case SDLK_LEFT:
				key.left = false;
				break;
			case SDLK_RIGHT:
				key.right = false;
				break;
			}
		}

	}

	//Update screen
	SDL_RenderPresent(renderer);

	//Clear screen
	SDL_RenderClear(renderer);

	return go_on;
}


Sprite * AvancezLib::createBmpSprite(const char * path)
{
	SDL_Surface* surf = SDL_LoadBMP(path);
	if (surf == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to load image %s! SDL_image Error: %s\n", path, SDL_GetError());
		return NULL;
	}

	//Create texture from surface pixels
	SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surf);
	if (texture == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to create texture from %s! SDL Error: %s\n", path, SDL_GetError());
		return NULL;
	}

	//Get rid of old loaded surface
	SDL_FreeSurface(surf);

	Sprite * sprite = new Sprite(renderer, texture);

	return sprite;
}

/*
Sprite * AvancezLib::createSprite(const char * path)
{
	// load sample.png into image
	SDL_Surface *image;
	SDL_RWops *rwop;
	rwop = SDL_RWFromFile(path, "rb");
	image = IMG_LoadPNG_RW(rwop);
	if (!image) {
		printf("IMG_LoadPNG_RW: %s\n", IMG_GetError());
		// handle error
	
	SDL_Surface* surf;
	SDL_RWops *rwop = SDL_RWFromFile(path, "rb");
	if (surf == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to load image %s! SDL_image Error: %s\n", path, SDL_GetError());
		return NULL;
	}

	//Create texture from surface pixels
	SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surf);
	if (texture == NULL)
	{
		printf("Unable to create texture from %s! SDL Error: %s\n", path, SDL_GetError());
		return NULL;
	}

	//Get rid of old loaded surface
	SDL_FreeSurface(surf);

	Sprite * sprite = new Sprite(renderer, texture);
	
	return sprite;

	return NULL;
}
*/

void AvancezLib::drawText(int x, int y, SDL_Color color, const char * msg)
{
	SDL_Surface* surf = TTF_RenderText_Solid(font, msg, color); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first

	SDL_Texture* msg_texture = SDL_CreateTextureFromSurface(renderer, surf); //now you can convert it into a texture

	int w = 0;
	int h = 0;
	SDL_QueryTexture(msg_texture, NULL, NULL, &w, &h);
	SDL_Rect dst_rect = { x, y, w, h };

	SDL_RenderCopy(renderer, msg_texture, NULL, &dst_rect);

 	SDL_DestroyTexture(msg_texture);
	SDL_FreeSurface(surf);
}

float AvancezLib::getElapsedTime()
{
	return SDL_GetTicks() / 1000.f;
}

void AvancezLib::getKeyStatus(KeyStatus & keys)
{
	keys.fire = key.fire;
	keys.left = key.left;
	keys.right = key.right;
}


Sprite::Sprite(SDL_Renderer * renderer, SDL_Texture * texture)
{
	this->renderer = renderer;
	this->texture = texture;
}

//draws sprite of any width and height completely to x,y
void Sprite::draw(int x, int y){
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;

	SDL_QueryTexture(texture, NULL, NULL, &(rect.w), &(rect.h));

	SDL_RenderCopy(renderer, texture, NULL, &rect);
}

void Sprite::draw(int x, int y, int id) {
	draw(32, 32, 32, 32, x, y, id);
}

void Sprite::draw(int sw, int sh, int dw, int dh, int x, int y, int id) {
	draw(sw, sh, dw, dh, x, y, sw, id);
}

/*
*	parameter sw, sh: width  and height of source rectangle; taken from sprite
*	parameter dw, sh: width  and height of destination rectangle the sprite is going to be drawn onto
*	parameter x, y: position of destination rectangle
*	parameter idStepping: size of one full sprite image; normally  = sw
*	parameter id: id of sprite to be taken from the image
*	currently does not support vertical subdivision of sprite
*/
void Sprite::draw(int sw, int sh, int dw, int dh, int x, int y, int idStepping, int id) {
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;

	//Render texture to screen
	SDL_Rect sourceRect;
	sourceRect.x = id * idStepping;
	sourceRect.y = 0;
	sourceRect.w = sw;
	sourceRect.h = sh;

	rect.w = dw;
	rect.h = dh;
	SDL_RenderCopy(renderer, texture, &sourceRect, &rect);
}



void Sprite::destroy()
{
	SDL_DestroyTexture(texture);
}

