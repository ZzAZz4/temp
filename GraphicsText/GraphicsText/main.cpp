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
	
	Battleship::AI ai;

	Directory::Config.outputFile = fs::current_path() / "in" / "file.in";
	Directory::Config.inputPath =  fs::current_path() / "out";
	Directory::Input::Handler::fetchData();

	Geometry::Point lastShot;
	int loseCounter = 0, winCounter = 0;
	int mode = 0;

	std::vector<int> defaultSizes = { 1, 1, 1, 1, 2, 2, 2, 3, 3, 4 };

	while (true) 
	{
		Directory::Input::Handler::fetchData();
		for (auto inMessage : Directory::Input::Handler::msgQueue)
		{
			switch (inMessage.instruction)
			{
			case (Directory::Instruction::Handshake):
				if (inMessage.status == Directory::Status::Accepted) {
					Battleship::token = inMessage.data;
					ai.newGame({10, 10}, defaultSizes);
				}
				else
					throw "Status denial exception on Handshake";
				break;

			case (Directory::Instruction::Placefleet):
				if (inMessage.status == Directory::Status::Accepted)
					continue;
				else
					throw "Status denial exception on Placefleet";
				break;

			case (Directory::Instruction::Attack):
				if (inMessage.status == Directory::Status::Accepted) {
					if (inMessage.data == "FAILED")
						ai.ShotMiss(lastShot);
					else if (inMessage.data == "DAMAGED")
						ai.ShotHit(lastShot);
					else if (inMessage.data == "DESTROYED")
						ai.ShotSunk(lastShot);
					else if (inMessage.data == "GAMEOVER")
					{
						loseCounter++;
						mode = 0;
						ai.newGame({ 10, 10 }, defaultSizes);
					}
					else if (inMessage.data == "WINNER")
					{
						winCounter++;
						mode = 0;
						ai.newGame({ 10, 10 }, defaultSizes);
					}
					else throw "Message exception on Attack";
				}
			default:
				throw "Decode exception";
			}

			if (mode == 0)
			{
				Directory::Output::Sender::enqueue(
					Directory::Output::Message(Directory::Instruction::Handshake, "Test")
				);
				while (!Directory::Output::Sender::queue.empty())
				{
					Directory::Output::Sender::send(Directory::Output::Sender::queue.front());
					Directory::Output::Sender::queue.pop_front();
				}
				mode = 1;
			}
			else if (mode == 1)
			{
				ai.messagePlacements();
				while (!Directory::Output::Sender::queue.empty())
				{
					Directory::Output::Sender::send(Directory::Output::Sender::queue.front());
					Directory::Output::Sender::queue.pop_front();
				}
				mode = 2;
			}
			else if (mode == 2)
				ai.enqueueAttack();

			while (!Directory::Output::Sender::queue.empty())
			{
				Directory::Output::Sender::send(Directory::Output::Sender::queue.front());
				Directory::Output::Sender::queue.pop_front();
			}
		}


	}



	return 0;
}
#endif