#ifndef OLC_PGE_APPLICATION
#define OLC_PGE_APPLICATION

#include "Application.h"


int main()
{
	// CurrentPath = C:\\Users\\esteb\\source\\repos\\GraphicsText\\GraphicsText

	Application game;
	game.Construct(160, 100, 8, 8);
	game.Start();


	return 0;
}
#endif