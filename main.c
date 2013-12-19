#include <stdlib.h>
#include <stdio.h>

#ifdef __APPLE__
#   include <SDL/SDL.h>
#else
#   include "SDL.h"
#endif

#include <SDL/SDL_ttf.h>

/* screen dimensions + bit-depth (BPP := bits per pixel) */
#define SCREEN_WIDTH    		320
#define SCREEN_HEIGHT   		240
#define SCREEN_BPP      		32

#define BLOCK_SIZE      		16
#define BLOCK_SET_WIDTH 		10
#define BLOCK_SET_HEIGHT		10
#define BLOCK_SET_SIZE  		(BLOCK_SET_WIDTH * BLOCK_SET_HEIGHT)

Uint32 generate_userevent (Uint32 intervall, void *parameter)
{
	SDL_Event event;
	SDL_UserEvent userevent;

	/* initialisiere einen UserEvent */
	userevent.type = SDL_USEREVENT;
	userevent.code = 0;
	userevent.data1 = NULL;
	userevent.data2 = NULL; 

	/* ... initialisiere damit einen Event */
	event.type = SDL_USEREVENT;
	event.user = userevent;

	/* neuen Event erzeugen */
	SDL_PushEvent (&event);
	return intervall;
}


void drawBlock(SDL_Surface *screen, SDL_Surface *bitmap, int x, int y, int shift, int blocktype)
{
	if(blocktype >= 0 && blocktype <= BLOCK_SET_SIZE)
	{
		SDL_Rect source;
		source.x = blocktype%10*BLOCK_SIZE;
		source.y = blocktype/10*BLOCK_SIZE;
		source.w = BLOCK_SIZE;
		source.h = BLOCK_SIZE;

		SDL_Rect destination;
		destination.x = x*BLOCK_SIZE-shift;
		destination.y = y*BLOCK_SIZE;
		destination.w = BLOCK_SIZE;
		destination.h = BLOCK_SIZE;

		SDL_BlitSurface(bitmap, &source, screen, &destination);
	}
}

int main( int argc, char *argv[] )
{
	if(argc > 0)
	{
		int argx;
		for(argx = 0; argx < argc; argx++)
		{
			printf("%s\n", argv[argx]);
		}
	}

	int quit = 0;

	int playerPositionX = 0;
	int playerPositionY = 0;

	int camPositionX = 0;
	int x = 0;
	int y = 0;

	double v = 0;

	int goLeft = 0;
	int goRight = 0;

	// Initialisiere SDL
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER ) < 0 ) {
		fprintf( stderr, "SDL konnte nicht initialisiert werden:  %s\n", SDL_GetError() );
		return 1;
	}

	// Erstelle die Bildschirmfläche
	SDL_Surface *screen = NULL;
	screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_HWSURFACE);

	// Definiere Hintergrundfarbe
	Uint32 color;
	color = SDL_MapRGB( screen->format, 0, 0, 0 );

	// Bitmaps laden
	SDL_Surface *player = SDL_LoadBMP("images/player.bmp");

	SDL_Surface *blockset = SDL_LoadBMP("images/test.bmp");

	SDL_Surface *background = SDL_LoadBMP("images/background.bmp");
	int backgroundHeight = 240;
	int backgroundWidth = 647;

	/* Den Timer auf 20ms stellen und starten */
	SDL_TimerID timer;
	timer = SDL_AddTimer (16, generate_userevent, NULL);

	SDL_Event event;
	while(SDL_WaitEvent (&event))
	{
		switch(event.type)
		{
			case SDL_QUIT:
			{
				quit = 1;
				break;
			}
			case SDL_USEREVENT:
			{
				playerPositionX = playerPositionX + (goRight - goLeft) * 3;

				if(playerPositionX < 0)
				{
					playerPositionX = 0;
				}
				if(playerPositionX > 1600-BLOCK_SIZE)
				{
					playerPositionX = 1600-BLOCK_SIZE;
				}

				v += 9.81/60;
				playerPositionY += v;

				if(playerPositionY >= 9*16)
				{
					playerPositionY = 9*16;
					v=0;
				}

				camPositionX += ((playerPositionX - SCREEN_WIDTH / 2 + BLOCK_SIZE / 2) - camPositionX) / 10;
				//camPositionX = playerPositionX;

				if(camPositionX < 0)
				{
					camPositionX = 0;
				}
				if(camPositionX > 1600-SCREEN_WIDTH)
				{
					camPositionX = 1600-SCREEN_WIDTH;
				}

				// Lösche den Hintergrund
				SDL_FillRect( screen, NULL, color );

				// Zeichne das Hintergrundbild
				SDL_Rect background_source;
				background_source.x = (int) ( (((double)camPositionX/(1600-SCREEN_WIDTH))) * (backgroundWidth-SCREEN_WIDTH) );
				background_source.y = 0;
				background_source.w = SCREEN_WIDTH;
				background_source.h = SCREEN_HEIGHT;

				SDL_Rect background_destination;
				background_destination.x = 0;
				background_destination.y = 0;
				background_destination.w = SCREEN_WIDTH;
				background_destination.h = SCREEN_HEIGHT;

				SDL_BlitSurface(background, &background_source, screen, &background_destination);

				// Zeichne alle Blöcke ein
				for(y = 10; y < SCREEN_HEIGHT/BLOCK_SIZE; y++)
				{
					for(x = camPositionX/BLOCK_SIZE; x < camPositionX/BLOCK_SIZE+SCREEN_WIDTH/BLOCK_SIZE+1; x++)
					{
						drawBlock(screen, blockset, x, y, camPositionX, x);
					}
				}

				// Zeichne den Spieler
				SDL_Rect player_source;
				player_source.x = 0;
				player_source.y = 0;
				player_source.w = BLOCK_SIZE;
				player_source.h = BLOCK_SIZE;

				SDL_Rect player_destination;
				player_destination.x = playerPositionX-camPositionX;
				player_destination.y = playerPositionY;
				player_destination.w = BLOCK_SIZE;
				player_destination.h = BLOCK_SIZE;

				SDL_BlitSurface(player, &player_source, screen, &player_destination);

				// Zeichne das berechnete Bild
				SDL_Flip( screen );
				break;
			}
			case SDL_KEYDOWN:
			{
				switch(event.key.keysym.sym)
				{
					case SDLK_LEFT:
						goLeft = 1;
						break;
					case SDLK_RIGHT:
						goRight = 1;
						break;
					case SDLK_SPACE:
						if(v == 0)
							{v = -3;}
						break;
					default:
						break;
				}
				break;
			}
			case SDL_KEYUP:
			{
				switch(event.key.keysym.sym)
				{
					case SDLK_LEFT:
						goLeft = 0;
						break;
					case SDLK_RIGHT:
						goRight = 0;
						break;
					default:
						break;
				}
				break;
			}
		}

		if(quit){break;}
	}

	SDL_Quit();

	return 0;

}
