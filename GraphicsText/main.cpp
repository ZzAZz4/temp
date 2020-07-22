#ifndef OLC_PGE_APPLICATION
#define OLC_PGE_APPLICATION

#include "Application.h"


int main()
{
	// CurrentPath = C:\\Users\\esteb\\source\\repos\\GraphicsText\\GraphicsText

	/*Application game;
	game.Construct(160, 100, 8, 8);
	game.Start();*/
	namespace fs = std::filesystem;
	

	Directory::configuration.outputFile = fs::current_path() / "in" / "file.txt";
	Directory::configuration.inputPath =  fs::current_path() / "out";
	Directory::Output::Sender::writeFile(
		Directory::Output::Message(Directory::Instruction::Handshake, "Test")
	);


	return 0;
}
#endif