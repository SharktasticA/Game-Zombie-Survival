﻿#include "Game.h"

Game::Game()
{
	Utility::setColour(WHITE, BLACK);
}
bool Game::run()
{
	//purpose: handles game's operation
	//pre-condition: inside a while loop (e.g. while(game.run());) to allow continous play

	//set up game
	getDifficulty();
	createHuman();
	createZombies();

	//set up window
	Utility::setWindowTitle("Zombie Survival");
	if (boolIsExpert)
		Utility::setWindowSize(830, 1000);
	else
		Utility::setWindowSize(595, 1000);

	//run game loop
	printBoard();
	while (player.checkIfAlive())
	{
		update();
		Sleep(50);
	}

	//end game
	return Utility::getYesNo("Play again (y/n)? ");
}

void Game::createHuman()
{
	//purpose: configure's the human's start properties
	player = Human(Utility::getString("Enter your name: "), coord(Utility::generateNumber(1, board.X), Utility::generateNumber(1, board.Y)));
}
void Game::getDifficulty()
{
	//purpose: asks user for input on difficulty
	
	boolIsExpert = Utility::getYesNo("Use expert difficulty (y/n)? ");
	if (boolIsExpert)
		board = coord(90, 30);
	else
		board = coord(60, 30);
}
void Game::createZombies()
{
	//purpose: generates a bunch of zombies (based on difficulty) and randomises their location

	int intToMake = 1; //base zombie amount for normal difficulty ***DECREASED FOR TESTING***
	if (boolIsExpert)
		intToMake *= 3; //factor in expert difficulty modifier

	for (int intIndex = 0; intIndex < intToMake; intIndex++)
	{
		bool boolIsUnique = false;
		int intX, intY; 

		while (!boolIsUnique) //iterate until X-Y are unique
		{
			//generate two start numbers
			boolIsUnique = true;
			intX = Utility::generateNumber(1, board.X);
			intY = Utility::generateNumber(1, board.Y);

			//prevent zombie from spawning on other zombies
			if (!zombies.empty())
			{
				for (auto zombie : zombies)
				{
					if (boolIsUnique)
					{
						if (intX == zombie.getX() && intY == zombie.getY())
							boolIsUnique = false;
					}
				}
			}

			//prevent zombie from being spanwed on player
			if (boolIsUnique)
			{
				if (intX == player.getX() && intY == player.getY())
					boolIsUnique = false;
			}
		}
		zombies.push_back(Zombie(intIndex, coord(intX, intY)));
	}
}

void Game::printBoard()
{
	//purpose: prints out board with all entity starting positions in place

	//draw board rows
	Utility::setColour(LIGHT_BLUE, LIGHT_BLUE);
	for (int intIndex = 0; intIndex <= board.X; intIndex++)
	{
		Utility::moveCursor(4 + intIndex, 3);
		cout << charBORDER;
		Utility::moveCursor(4 + intIndex, 4 + board.Y);
		cout << charBORDER;
	}

	//draw board columns
	for (int intIndex = 0; intIndex <= board.Y + 1; intIndex++)
	{
		Utility::moveCursor(4, 3 + intIndex);
		cout << charBORDER;
		Utility::moveCursor(5 + board.X, 3 + intIndex);
		cout << charBORDER;
	}

	//draw human
	Utility::setColour(YELLOW, BLACK);
	Utility::moveCursor(4 + player.getX(), 3 + player.getY());
	cout << charHUMAN;

	//draw zombies
	Utility::setColour(LIGHT_RED, BLACK);
	for (auto zombie : zombies)
	{
		Utility::moveCursor(4 + zombie.getX(), 3 + zombie.getY());
		cout << charZOMBIE;
	}

	//draw event board
	Utility::setColour(WHITE, BLACK);
	Utility::moveCursor(0, 37);
	cout << "Events: ";
}

void Game::update()
{
	//purpose: moves game forward at every turn; handles movement and redrawing

	//update human
	drawPlayer(updateHuman());
	drawGun();

	//update zombies
	drawZombies(updateZombies());

	//post-update procedures
	events(); 
	intMove++;
}
coord Game::updateHuman()
{
	coord playerLast;
	playerLast.X = player.getX();
	playerLast.Y = player.getY();
	player.move(board, bullets);

	return playerLast;
}
vector<coord> Game::updateZombies()
{
	vector<coord> zombiesLast;
	for (unsigned int intIndex = 0; intIndex < zombies.size(); intIndex++)
	{
		coord zombieLast;
		zombieLast.X = zombies[intIndex].getX();
		zombieLast.Y = zombies[intIndex].getY();
		zombiesLast.push_back(zombieLast);
		zombies[intIndex].move(board, zombies);
	}
	return zombiesLast;
}
void Game::drawPlayer(coord playerLast)
{
	Utility::setColour(LIGHT_YELLOW, BLACK);
	Utility::moveCursor(4 + playerLast.X, 3 + playerLast.Y);
	cout << " ";
	Utility::moveCursor(4 + player.getX(), 3 + player.getY());
	cout << charHUMAN;
}
void Game::drawGun()
{
	Utility::setColour(YELLOW, BLACK);
	if (player.getDirection() == 'W' && player.getY() > 1)
	{
		Utility::moveCursor(4 + player.getX(), 3 + player.getY() - 1);
		cout << "|";
	}
	else if (player.getDirection() == 'S' && player.getY() < (board.Y))
	{
		Utility::moveCursor(4 + player.getX(), 3 + player.getY() + 1);
		cout << "|";
	}
	else if (player.getDirection() == 'A' && player.getX() > 1)
	{
		Utility::moveCursor(4 + player.getX() - 1, 3 + player.getY());
		cout << "-";
	}
	else if (player.getDirection() == 'D' && player.getX() < (board.X))
	{
		Utility::moveCursor(4 + player.getX() + 1, 3 + player.getY());
		cout << "-";
	}
}
void Game::drawZombies(vector<coord> zombiesLast)
{
	Utility::setColour(LIGHT_RED, BLACK);
	for (unsigned int intIndex = 0; intIndex < zombies.size(); intIndex++)
	{
		if (zombies[intIndex].checkIfAlive())
		{
			Utility::moveCursor(4 + zombiesLast[intIndex].X, 3 + zombiesLast[intIndex].Y);
			cout << " ";
			Utility::moveCursor(4 + zombies[intIndex].getX(), 3 + zombies[intIndex].getY());
			cout << charZOMBIE;
		}
	}
}

void Game::events()
{
	//purpose: checks for and post events

	Utility::moveCursor(0, 38 + intEventCount); //moves cursor the event board's next blank line

												//erase zombies from the board
	Utility::setColour(WHITE, BLACK);
	vector<coord> coordDeaths = checkZombies();
	for (auto death : coordDeaths)
	{
		Utility::moveCursor(4 + death.X, 3 + death.Y);
		cout << " ";
		intEventCount++;
	}
}
vector<coord> Game::checkZombies()
{
	//purpose: checks if any zombies have died and posts their death onto the event board
	//usage: this method MUST only be called in events() method since the cursor position has to be set beforehand (ie. cursor is at event board position)

	vector<coord> coordDeaths; //contains all death locations to be used elsewhere

	for (unsigned int intX = 0; intX < zombies.size(); intX++)
	{
		for (unsigned int intY = 0; intY < zombies.size(); intY++)
		{
			if (zombies[intX].checkIfAlive() && zombies[intY].checkIfAlive() && zombies[intX].getID() != zombies[intY].getID() && zombies[intX].getX() == zombies[intY].getX() && zombies[intX].getY() == zombies[intY].getY()) //checks for a collision
			{
				zombies[intX].kill();
				zombies[intY].kill();
				coordDeaths.push_back(coord(zombies[intX].getX(), zombies[intX].getY()));
				cout << "Move " << intMove << ": zombies " << zombies[intX].getID() << " and " << zombies[intY].getID() << " have died due to a collision at " << zombies[intX].getX() << "x" << zombies[intX].getY() << "!" << endl;
			}
		}
	}
	return coordDeaths;
}