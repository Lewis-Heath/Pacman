#include "Pacman.h"
#include <time.h>
#include <sstream>
#include <iostream>
#include <amp_graphics.h>
#include <iostream>
#include <fstream>

using namespace std;

// Constructor method
Pacman::Pacman(int argc, char* argv[]) : Game(argc, argv)
{
	// Initialise member variables

	// Local variables
	int i;
	int v;
	srand(time(NULL));

	// Leaderboard Variables
	_lead = new Leaderboard();
	string name;
	cout << "Please enter your name: ";
	cin >> name;
	_lead->playerName = name;

	// Cherry Variables
	_cherry = new Enemy();
	_cherry->FrameTime = 300;
	_cherry->CurrentFrameTime = 0;
	_cherry->frameCount = 0;
	_cherry->gotPosition = false;
	_cherry->scoreProp = 2500;
	_cherry->lifetime = 4001;

	// Pacman variables
	_pacman = new Player();
	_pacman->dead = false;
	_pacman->speed = 0.15f;
	_pacman->speedMultiplier = 1.0f;
	_pacman->FrameTime = 250;
	_pacman->CurrentFrameTime = 0;
	_pacman->Frame = 0;
	_pacman->Moving = false;
	_pacman->speedBoostTimer = 0;
	_pacman->speedBoost = false;
	_pacman->Direction = 4;
	_pacman->barPerc = 196;
	_pacman->recharging = false;
	_pacman->cooldownCount = 0;
	_pacman->soundCounter = 0;
	_pacman->soundPlaying = false;

	//Menu variables
	_menu = new Menu();
	_menu->paused = false;
	_menu->started = false;
	_menu->munchieCounter = 0;
	_menu->maxMunchieCounter = 1;
	_menu->wallCounter = 0;
	_menu->ghostCounter = 0;
	_menu->titleCounter = 0;
	_menu->screen = "Main";
	_menu->map = 1;
	_menu->ghostFright = false;
	_menu->startPlayed = false;
	_menu->playerScore = 0;
	_menu->ghostMultiplier = 2;
	_menu->scoreAdd = 200;
	_menu->scoreAdd2 = 100;
	_menu->scoreAddTimer = 101;
	_menu->scoreAddTimer2 = 101;
	_menu->addedOnce = false;
	_menu->leadSorted = false;
	_menu->musicCounter = 0;					
	_menu->musicPlaying = false;

	// Sound creation
	_pop = new SoundEffect();
	_coin = new SoundEffect();
	_inter = new SoundEffect();
	_speed = new SoundEffect();
	_death = new SoundEffect();
	_ghostdeath = new SoundEffect();
	_pop2 = new SoundEffect();
	_music = new SoundEffect();


	// Loops through every ghost
	for (v = 0; v < GHOSTCOUNT; v++)
	{
		// Ghost creation
		_ghosts[v] = new MovingEnemy();
		// Ghost variables
		_ghosts[v]->direction = 0;
		_ghosts[v]->moveCounter = 0;
		_ghosts[v]->speed = 2;
		_ghosts[v]->randomCounter = rand() % 45;
	}

	//Initialise important Game aspects
	Audio::Initialise();
	Graphics::Initialise(argc, argv, this, 1024, 768, false, 25, 25, "Pacman", 60);
	Input::Initialise();
	// Start the Game Loop - This calls Update and Draw in game loop
	Graphics::StartGameLoop();
}


// Destructor method
Pacman::~Pacman()
{
	// Clean up pointers within the Pacman structure
	delete _pacman->Position;
	delete _pacman->Texture;
	delete _pacman->SourceRect;

	// Clean up the Pacman structure pointer
	delete _pacman;

	// Clean up sounds
	delete _pop;
	delete _coin;
	delete _inter;
	delete _speed;
	delete _death;
	delete _ghostdeath;
	delete _pop2;
	delete _music;
	
	// Clean up the texture
	for (int i = 0; i < _menu->munchieCounter; i++) 
	{ 
		delete _munchies[i]->Texture;
		delete _munchies[i]->SourceRect; 
	} 

	// Clean up Ghosts
	for (int i = 0; i < GHOSTCOUNT; i++)
	{
		delete _ghosts[i]->texture;
		delete _ghosts[i]->sourceRect;
	}

	// Clean up Walls
	for (int i = 0; i < _menu->wallCounter; i++)
	{
		delete _walls[i]->Texture;
		delete _walls[i]->sourceRect;
	}

	// Order of operations is important, array deleted last
	delete[] _munchies;
	delete[] _ghosts;
	delete[] _walls;
}

int Pacman::indexofSmallestElement(int array[], int size) // Returns the smallest index of the array passed in.
{
	int index = 0;

	if (size != 1)
	{

		int n = array[0];
		for (int i = 1; i < size; i++)
		{
			if (array[i] < n)
			{
				n = array[i];
				index = i;
			}
		}
	}
	return index;
}


void Pacman::LoadMap(string nameOfMap)  // Loads the map into a multi dimensional array.
{
	ifstream infile(nameOfMap); // Opens file
	if (!infile) // If it cant be opened
	{
		cerr << "File could not be opened!";
		exit(1);
	}
	else // If it can be opened
	{

		int lineNum = -1;
		string line;


		// Creates and loads textures
		Texture2D* munchieTex = new Texture2D();
		munchieTex->Load("Textures/Munchies.png", true);
		Texture2D* munchieUpTex = new Texture2D();
		munchieUpTex->Load("Textures/MunchiesUp.png", false);
		Texture2D* wallTex = new Texture2D();
		wallTex->Load("Textures/Wall.png", false);
		Texture2D* wallTex2 = new Texture2D();
		wallTex2->Load("Textures/Gate.png", false);

		while (getline(infile, line)) // Loops through every line in the file
		{
			lineNum++; 
			string a;
			a = line;
			for (int i = 0; i < a.length(); i++) // Loops through every character in the line
			{
				if (a[i] == 'G') // If the character is G creates a ghost
				{
					_ghosts[_menu->ghostCounter]->sourceRect = new Rect(0.0f, 0.0f, 32, 32);
					_ghosts[_menu->ghostCounter]->position = new Vector2((32 * i), (32 * lineNum));
					_ghosts[_menu->ghostCounter]->spawnPosition = new Vector2((32 * 16), (32 * 14)); 
					_ghosts[_menu->ghostCounter]->moveUp = true;
					_ghosts[_menu->ghostCounter]->moveDown = true;
					_ghosts[_menu->ghostCounter]->moveLeft = true;
					_ghosts[_menu->ghostCounter]->moveRight = true;
					_ghosts[_menu->ghostCounter]->destructable = false;
					_menu->ghostCounter++;
					_menu->tileMap[lineNum][i] = "-";
				}
				if (a[i] == 'x') // If the character is x creates a gate
				{
					_walls[_menu->wallCounter] = new Wall();
					_walls[_menu->wallCounter]->Texture = wallTex2;
					_walls[_menu->wallCounter]->sourceRect = new Rect(0, 0, 32, 32);
					_walls[_menu->wallCounter]->Position = new Vector2((32 * i), (32 * lineNum));
					_walls[_menu->wallCounter]->Gate = true;
					_menu->wallCounter++;
					_menu->tileMap[lineNum][i] = "-";
				}
				if (a[i] == 'P') // If the character is P creates Pacman
				{
					_pacman->Texture = new Texture2D();
					_pacman->Texture->Load("Textures/Pacman.png", false);
					_pacman->Position = new Vector2((32*i),(32*lineNum));
					_pacman->SourceRect = new Rect(0.0f, 0.0f, 32, 32);
					_pacman->HitRect = new Rect(_pacman->Position->X, _pacman->Position->Y, 32, 32);
					_pacman->tileMapX = _pacman->Position->X / 32;
					_pacman->tileMapY = _pacman->Position->Y / 32;
					_menu->tileMap[lineNum][i] = "-";
				}
				if (a[i] == 'X') // If the character is X creates a wall
				{
					_walls[_menu->wallCounter] = new Wall();
					_walls[_menu->wallCounter]->Texture = wallTex;
					_walls[_menu->wallCounter]->sourceRect = new Rect(0, 0, 32, 32);
					_walls[_menu->wallCounter]->Position = new Vector2((32 * i), (32 * lineNum));
					_walls[_menu->wallCounter]->Gate = false;
					_menu->wallCounter++;
					_menu->tileMap[lineNum][i] = "X";
				}
				if (a[i] == '-') // If the character is a - creates a munchie
				{
					_munchies[_menu->munchieCounter] = new Enemy();
					_munchies[_menu->munchieCounter]->FrameTime = rand() % 500 + 250;
					_munchies[_menu->munchieCounter]->CurrentFrameTime = 0;
					_munchies[_menu->munchieCounter]->frameCount = rand() % 1;
					_munchies[_menu->munchieCounter]->Texture = munchieTex;
					_munchies[_menu->munchieCounter]->SourceRect = new Rect(0, 0, 12, 12);
					_munchies[_menu->munchieCounter]->Position = new Vector2(((32 * i) + 10), ((32 * lineNum) + 10));
					_munchies[_menu->munchieCounter]->dead = false;
					_munchies[_menu->munchieCounter]->type = "Normal";
					_menu->munchieCounter++;
					_menu->tileMap[lineNum][i] = "-";
				}
				if (a[i] == 'p') // If the character is a p creates a power up munchie
				{
					_munchies[_menu->munchieCounter] = new Enemy();
					_munchies[_menu->munchieCounter]->FrameTime = rand() % 500 + 250;
					_munchies[_menu->munchieCounter]->CurrentFrameTime = 0;
					_munchies[_menu->munchieCounter]->frameCount = rand() % 1;
					_munchies[_menu->munchieCounter]->Texture = munchieUpTex;
					_munchies[_menu->munchieCounter]->SourceRect = new Rect(0, 0, 12, 12);
					_munchies[_menu->munchieCounter]->Position = new Vector2(((32 * i) + 10), ((32 * lineNum) + 10));
					_munchies[_menu->munchieCounter]->dead = false;
					_munchies[_menu->munchieCounter]->type = "Powerup";
					_menu->munchieCounter++;
					_menu->tileMap[lineNum][i] = "-";
				}
				if (a[i] == '_') // If the charcater is a _ creates a gap
				{
					_menu->tileMap[lineNum][i] = "-";
				}
			}
		}
	}
	UpdateWalls();
}

void Pacman::LoadContent() // Loads key info
{
	// Local Var
	int i;
	int v;

	// Creates and loads ghost textures
	_menu->ghostBlue = new Texture2D();
	_menu->ghostBlue->Load("Textures/BlueSheet.png", false);
	_menu->ghostRed = new Texture2D();
	_menu->ghostRed->Load("Textures/RedSheet.png", false);
	_menu->ghostOrange = new Texture2D();
	_menu->ghostOrange->Load("Textures/OrangeSheet.png", false);
	_menu->ghostPurple = new Texture2D();
	_menu->ghostPurple->Load("Textures/PurpleSheet.png", false);
	_menu->ghostGreen = new Texture2D();
	_menu->ghostGreen->Load("Textures/GreenSheet.png", false);
	_menu->frightTexture = new Texture2D();
	_menu->frightTexture->Load("Textures/frightSheet.png", false);

	// Loops through the ghosts setting each ghost a different texture
	for (int i = 0; i < GHOSTCOUNT; i++)
	{
		if (i == 0)
			_ghosts[i]->texture = _menu->ghostBlue;
		if (i == 1)
			_ghosts[i]->texture = _menu->ghostRed;
		if (i == 2)
			_ghosts[i]->texture = _menu->ghostPurple;
		if (i == 3)
			_ghosts[i]->texture = _menu->ghostOrange;
		if (i == 4)
			_ghosts[i]->texture = _menu->ghostGreen;
	}

	// Creates and loads all the textures, variables and positions for the menu
	_menu->Background = new Texture2D();
	_menu->pauseBackground = new Texture2D();
	_menu->Start = new Texture2D();
	_menu->Level = new Texture2D();
	_menu->Leaderboard = new Texture2D();
	_menu->pacmanOpen = new Texture2D();
	_menu->pacmanClosed = new Texture2D();
	_menu->backStart = new Texture2D();
	_menu->backLevel = new Texture2D();
	_menu->backLead = new Texture2D();
	_menu->innerButton = new Texture2D();
	_menu->thePacman = new Texture2D();
	_menu->level1 = new Texture2D();
	_menu->level2 = new Texture2D();
	_menu->level3 = new Texture2D();
	_menu->back = new Texture2D();
	_menu->exit = new Texture2D();
	_menu->pauseBackground->Load("Textures/Transparency.png", false);
	_menu->back->Load("Textures/back.png", false);
	_menu->level1->Load("Textures/level1.png", false);
	_menu->level2->Load("Textures/level2.png", false);
	_menu->level3->Load("Textures/level3.png", false);
	_menu->thePacman->Load("Textures/Pacman.png", false);
	_menu->innerButton->Load("Textures/innerButton.png", false);
	_menu->Background->Load("Textures/background.png", false);
	_menu->Start->Load("Textures/start.png", false);
	_menu->Level->Load("Textures/level.png", false);
	_menu->Leaderboard->Load("Textures/leaderboard.png", false);
	_menu->pacmanOpen->Load("Textures/pacmanOpen.png", false);
	_menu->pacmanClosed->Load("Textures/pacmanClosed.png", false);
	_menu->backStart->Load("Textures/backGreen.png", false);
	_menu->backLevel->Load("Textures/backGreen.png", false);
	_menu->backLead->Load("Textures/backGreen.png", false);
	_menu->exit->Load("Textures/exit.png", false);
	_menu->scoreAddPosition = new Vector2(-10000, -10000);
	_menu->backPosition = new Vector2(Graphics::GetViewportWidth() / 2 - 200, Graphics::GetViewportHeight() / 2 - 370);
	_menu->backBackPosition = new Vector2(Graphics::GetViewportWidth() / 2 - 261, Graphics::GetViewportHeight() / 2 - 375);
	_menu->innerButtonPosition4 = new Vector2(Graphics::GetViewportWidth() / 2 - 251, Graphics::GetViewportHeight() / 2 - 365);
	_menu->thePacmanPosition = new Vector2(Graphics::GetViewportWidth() / 2 - 400, Graphics::GetViewportHeight() / 2 + 250);
	_menu->stringPosition = new Vector2(Graphics::GetViewportWidth() / 2 - 50, Graphics::GetViewportHeight() / 2);
	_menu->leadPosition = new Vector2(Graphics::GetViewportWidth() / 2 - 50, 150);
	_menu->stringPosition2 = new Vector2(Graphics::GetViewportWidth() / 2 - 80, Graphics::GetViewportHeight() / 2 + 40);
	_menu->startPosition = new Vector2(Graphics::GetViewportWidth() / 2 - 200, Graphics::GetViewportHeight() / 2 - 230);
	_menu->level1Position = new Vector2(Graphics::GetViewportWidth() / 2 - 200, Graphics::GetViewportHeight() / 2 - 230);
	_menu->innerButtonPosition1 = new Vector2(Graphics::GetViewportWidth() / 2 - 251, Graphics::GetViewportHeight() / 2 - 225);
	_menu->levelPosition = new Vector2(Graphics::GetViewportWidth() / 2 - 200, Graphics::GetViewportHeight() / 2 - 75);
	_menu->level2Position = new Vector2(Graphics::GetViewportWidth() / 2 - 200, Graphics::GetViewportHeight() / 2 - 75);
	_menu->innerButtonPosition2 = new Vector2(Graphics::GetViewportWidth() / 2 - 251, Graphics::GetViewportHeight() / 2 - 71);
	for (int i = 0; i < GHOSTCOUNT; i++)
	{
		_ghosts[i]->menuPosition = new Vector2(Graphics::GetViewportWidth() / 2 - 400 - ((i + 1) * 125) , Graphics::GetViewportHeight() / 2 + 250);
		_ghosts[i]->namePosition = new Vector2(Graphics::GetViewportWidth() / 2 - 405 - ((i + 1) * 125), Graphics::GetViewportHeight() / 2 + 350);
		_ghosts[i]->menuSourceRect = new Rect(64, 0, 32, 32);
	}
	_menu->leaderboardPosition = new Vector2(Graphics::GetViewportWidth() / 2 - 200, Graphics::GetViewportHeight() / 2 + 80);
	_menu->level3Position = new Vector2(Graphics::GetViewportWidth() / 2 - 200, Graphics::GetViewportHeight() / 2 + 80);
	_menu->innerButtonPosition3 = new Vector2(Graphics::GetViewportWidth() / 2 - 251, Graphics::GetViewportHeight() / 2 + 87);
	_menu->pacmanPosition = new Vector2(Graphics::GetViewportWidth() / 2 - 200, Graphics::GetViewportHeight() / 2 - 370);
	_menu->backStartPosition = new Vector2(Graphics::GetViewportWidth() / 2 - 261, Graphics::GetViewportHeight() / 2 - 235);
	_menu->backLevelPosition = new Vector2(Graphics::GetViewportWidth() / 2 - 261, Graphics::GetViewportHeight() / 2 - 80);
	_menu->backLeadPosition = new Vector2(Graphics::GetViewportWidth() / 2 - 261, Graphics::GetViewportHeight() / 2 + 76);
	_menu->startRect = new Rect(0, 0, 400, 100);
	_menu->levelRect = new Rect(0, 0, 400, 100);
	_menu->levelNRect = new Rect(0, 0, 400, 100);
	_menu->leaderboardRect = new Rect(0, 0, 400, 100);
	_menu->pacmanRect = new Rect(0, 0, 400, 100);
	_menu->backStartRect = new Rect(0, 0, 510, 110);
	_menu->backLevelRect = new Rect(0, 0, 510, 110);
	_menu->backLeadRect = new Rect(0, 0, 510, 110);
	_menu->innerButtonRect = new Rect(0, 0, 490, 90);
	_menu->thePacmanRect = new Rect(0, 0, 32, 32);
	_menu->Rectangle = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());
	
	// Load Cherry 
	_cherry->Texture = new Texture2D();
	_cherry->Texture->Load("Textures/Cherries.png", false);
	_cherry->SourceRect = new Rect(0.0f, 0.0f, 32, 32);
	_cherry->Position = new Vector2(rand() % Graphics::GetViewportWidth(), rand() % Graphics::GetViewportHeight());

	// Load Sounds
	_pop->Load("Sounds/pacman_chomp.wav");
	_coin->Load("Sounds/pacman_beginning.wav");
	_inter->Load("Sounds/pacman_intermision.wav");
	_speed->Load("Sounds/gallop.wav");
	_death->Load("Sounds/pacman_death.wav");
	_ghostdeath->Load("Sounds/pacman_eatghost.wav");
	_pop2->Load("Sounds/pacman_eatfruit.wav");
	_music->Load("Sounds/background_music.wav");
	
	// Set string position
	_stringPosition = new Vector2(10.0f, 22.0f);
	_textPosition1 = new Vector2(10.0f, 22.0f);
	_textPosition2 = new Vector2(125.0f, 22.0f);
	_textPosition3 = new Vector2(485.0f, 22.0f);

	//Audio::Play(_music);
}

void Pacman::UpdateWalls() // Updaes the wall textures to look smooth in game 

{
	// Creates and loads all the different wall textures
	Texture2D* wallTex2 = new Texture2D();
	wallTex2->Load("Textures/Gate.png", false);

	Texture2D* wallRD = new Texture2D();
	wallRD->Load("Walls/RD.png", false);

	Texture2D* wallRD2 = new Texture2D();
	wallRD2->Load("Walls/RD2.png", false);

	Texture2D* wallLD = new Texture2D();
	wallLD->Load("Walls/LD.png", false);

	Texture2D* wallLD2 = new Texture2D();
	wallLD2->Load("Walls/LD2.png", false);

	Texture2D* wallRUD = new Texture2D();
	wallRUD->Load("Walls/RUD.png", false);

	Texture2D* wallRUD2 = new Texture2D();
	wallRUD2->Load("Walls/RUD2.png", false);

	Texture2D* wallRUD3 = new Texture2D();
	wallRUD3->Load("Walls/RUD3.png", false);

	Texture2D* wallRUD4 = new Texture2D();
	wallRUD4->Load("Walls/RUD4.png", false);

	Texture2D* wallLUD = new Texture2D();
	wallLUD->Load("Walls/LUD.png", false);

	Texture2D* wallLUD2 = new Texture2D();
	wallLUD2->Load("Walls/LUD2.png", false);

	Texture2D* wallLUD3 = new Texture2D();
	wallLUD3->Load("Walls/LUD3.png", false);

	Texture2D* wallLUD4 = new Texture2D();
	wallLUD4->Load("Walls/LUD4.png", false);

	Texture2D* wallRU = new Texture2D();
	wallRU->Load("Walls/RU.png", false);

	Texture2D* wallRU2 = new Texture2D();
	wallRU2->Load("Walls/RU2.png", false);

	Texture2D* wallLU = new Texture2D();
	wallLU->Load("Walls/LU.png", false);

	Texture2D* wallLU2 = new Texture2D();
	wallLU2->Load("Walls/LU2.png", false);

	Texture2D* wallD = new Texture2D();
	wallD->Load("Walls/D.png", false);

	Texture2D* wallU = new Texture2D();
	wallU->Load("Walls/U.png", false);

	Texture2D* wallUD = new Texture2D();
	wallUD->Load("Walls/UD.png", false);

	Texture2D* wallLRD = new Texture2D();
	wallLRD->Load("Walls/LRD.png", false);

	Texture2D* wallLRD2 = new Texture2D();
	wallLRD2->Load("Walls/LRD2.png", false);

	Texture2D* wallLRD3 = new Texture2D();
	wallLRD3->Load("Walls/LRD3.png", false);

	Texture2D* wallLRD4 = new Texture2D();
	wallLRD4->Load("Walls/LRD4.png", false);

	Texture2D* wallLRU = new Texture2D();
	wallLRU->Load("Walls/LRU.png", false);

	Texture2D* wallLRU2 = new Texture2D();
	wallLRU2->Load("Walls/LRU2.png", false);

	Texture2D* wallLRU3 = new Texture2D();
	wallLRU3 ->Load("Walls/LRU3.png", false);

	Texture2D* wallLRU4 = new Texture2D();
	wallLRU4->Load("Walls/LRU4.png", false);

	Texture2D* wallNONE = new Texture2D();
	wallNONE->Load("Walls/NONE.png", false);

	Texture2D* wallALL = new Texture2D();
	wallALL->Load("Walls/ALL.png", false);

	Texture2D* wallALL1 = new Texture2D();
	wallALL1->Load("Walls/ALL1.png", false);

	Texture2D* wallALL2 = new Texture2D();
	wallALL2->Load("Walls/ALL2.png", false);

	Texture2D* wallALL3 = new Texture2D();
	wallALL3->Load("Walls/ALL3.png", false);

	Texture2D* wallALL4 = new Texture2D();
	wallALL4->Load("Walls/ALL4.png", false);

	Texture2D* wallALL5 = new Texture2D();
	wallALL5->Load("Walls/ALL5.png", false);

	Texture2D* wallLR = new Texture2D();
	wallLR->Load("Walls/LR.png", false);

	Texture2D* wallL = new Texture2D();
	wallL->Load("Walls/L.png", false);

	Texture2D* wallR = new Texture2D();
	wallR->Load("Walls/R.png", false);

	// Loops through all the walls
	for (int i = 0; i < _menu->wallCounter; i++)
	{
		// Works out the tilemap position of the wall
		int tileMapX = _walls[i]->Position->X / 32;
		int tileMapY = _walls[i]->Position->Y / 32;
	
		// Creates and sets the directions as false
		bool leftWall = false;
		bool rightWall = false;
		bool topWall = false;
		bool bottomWall = false;
		bool topLeftWall = false;
		bool topRightWall = false;
		bool bottomLeftWall = false;
		bool bottomRightWall = false;
		bool againstLeftWall = false;
		bool againstRightWall = false;

		// If against the left wall
		if (tileMapX == 0)
		{
			againstLeftWall = true;
		}

		// If against the right wall
		if (tileMapX == 31)
		{
			againstRightWall = true;
		}

		// Checks the top, bottom , left, right, topright, topleft, bottomleft and bottomright to see if there is a wall present
		if (_menu->tileMap[tileMapY - 1][tileMapX] == "X")
			topWall = true;
		if (_menu->tileMap[tileMapY + 1][tileMapX] == "X")
			bottomWall = true;
		if (_menu->tileMap[tileMapY][tileMapX - 1] == "X")
			leftWall = true;
		if (_menu->tileMap[tileMapY][tileMapX + 1] == "X")
			rightWall = true;	
		if (_menu->tileMap[tileMapY - 1][tileMapX + 1] == "X")
			topRightWall = true;
		if (_menu->tileMap[tileMapY - 1][tileMapX - 1] == "X")
			topLeftWall = true;
		if (_menu->tileMap[tileMapY + 1][tileMapX - 1] == "X")
			bottomLeftWall = true;
		if (_menu->tileMap[tileMapY + 1][tileMapX + 1] == "X")
			bottomRightWall = true;


		// Sets the walls texture depending on what variables are true and false
		if (rightWall && bottomWall && !leftWall && !topWall && bottomRightWall)
			_walls[i]->Texture = wallRD;

		if (rightWall && bottomWall && !leftWall && !topWall && !bottomRightWall)
			_walls[i]->Texture = wallRD2;

		if (!rightWall && bottomWall && leftWall && !topWall && bottomLeftWall)
			_walls[i]->Texture = wallLD;

		if (!rightWall && bottomWall && leftWall && !topWall && !bottomLeftWall)
			_walls[i]->Texture = wallLD2;

		if (rightWall && bottomWall && !leftWall && topWall && bottomRightWall && topRightWall)
			_walls[i]->Texture = wallRUD;

		if (rightWall && bottomWall && !leftWall && topWall && !bottomRightWall && !topRightWall)
			_walls[i]->Texture = wallRUD2;

		if (rightWall && bottomWall && !leftWall && topWall && !bottomRightWall && !topRightWall)
			_walls[i]->Texture = wallRUD2;

		if (rightWall && bottomWall && !leftWall && topWall && !topRightWall && bottomRightWall)
			_walls[i]->Texture = wallRUD3;

		if (rightWall && bottomWall && !leftWall && topWall && topRightWall && !bottomRightWall)
			_walls[i]->Texture = wallRUD4;

		if (!rightWall && bottomWall && leftWall && topWall && bottomLeftWall && topLeftWall)
			_walls[i]->Texture = wallLUD;

		if (!rightWall && bottomWall && leftWall && topWall && !bottomLeftWall && !topLeftWall)
			_walls[i]->Texture = wallLUD2;

		if (!rightWall && bottomWall && leftWall && topWall && bottomLeftWall && !topLeftWall)
			_walls[i]->Texture = wallLUD3;

		if (!rightWall && bottomWall && leftWall && topWall && !bottomLeftWall && topLeftWall)
			_walls[i]->Texture = wallLUD4;

		if (rightWall && !bottomWall && !leftWall && topWall && topRightWall)
			_walls[i]->Texture = wallRU;

		if (rightWall && !bottomWall && !leftWall && topWall && !topRightWall)
			_walls[i]->Texture = wallRU2;

		if (!rightWall && !bottomWall && leftWall && topWall && topLeftWall)
			_walls[i]->Texture = wallLU;

		if (!rightWall && !bottomWall && leftWall && topWall && !topLeftWall)
			_walls[i]->Texture = wallLU2;

		if (!rightWall && bottomWall && !leftWall && !topWall)
			_walls[i]->Texture = wallD;

		if (!rightWall && bottomWall && !leftWall && topWall)
			_walls[i]->Texture = wallUD;

		if (!rightWall && !bottomWall && !leftWall && topWall)
			_walls[i]->Texture = wallU;

		if (rightWall && bottomWall && leftWall && !topWall && bottomRightWall && bottomLeftWall)
			_walls[i]->Texture = wallLRD;

		if (rightWall && bottomWall && leftWall && !topWall && !bottomRightWall && !bottomLeftWall)
			_walls[i]->Texture = wallLRD2;

		if (rightWall && bottomWall && leftWall && !topWall && bottomRightWall && !bottomLeftWall)
			_walls[i]->Texture = wallLRD3;

		if (rightWall && bottomWall && leftWall && !topWall && !bottomRightWall && bottomLeftWall)
			_walls[i]->Texture = wallLRD4;

		if (rightWall && !bottomWall && leftWall && topWall && topRightWall &&  topLeftWall)
			_walls[i]->Texture = wallLRU;

		if (rightWall && !bottomWall && leftWall && topWall && !topRightWall && !topLeftWall)
			_walls[i]->Texture = wallLRU2;

		if (rightWall && !bottomWall && leftWall && topWall && topRightWall && !topLeftWall)
			_walls[i]->Texture = wallLRU3;

		if (rightWall && !bottomWall && leftWall && topWall && topLeftWall && !topRightWall)
			_walls[i]->Texture = wallLRU4;

		if (rightWall && bottomWall && leftWall && topWall && topRightWall && topLeftWall && bottomLeftWall && bottomRightWall)
			_walls[i]->Texture = wallALL;

		if (rightWall && bottomWall && leftWall && topWall && !bottomLeftWall && !bottomRightWall)
			_walls[i]->Texture = wallALL1;

		if (rightWall && bottomWall && leftWall && topWall && !topLeftWall && !topRightWall)
			_walls[i]->Texture = wallALL2;

		if (rightWall && bottomWall && leftWall && topWall && !topRightWall && !bottomRightWall)
			_walls[i]->Texture = wallALL3;

		if (rightWall && bottomWall && leftWall && topWall && !topLeftWall && !bottomLeftWall)
			_walls[i]->Texture = wallALL4;

		if (!rightWall && !bottomWall && !leftWall && !topWall)
			_walls[i]->Texture = wallNONE;

		if (rightWall && !bottomWall && leftWall && !topWall)
			_walls[i]->Texture = wallLR;

		if (!rightWall && !bottomWall && leftWall && !topWall)
			_walls[i]->Texture = wallL;

		if (rightWall && !bottomWall && !leftWall && !topWall)
			_walls[i]->Texture = wallR;

		if (againstLeftWall && !rightWall && bottomWall && !topWall)
			_walls[i]->Texture = wallD;

		if (againstLeftWall && !rightWall && bottomWall && topWall)
			_walls[i]->Texture = wallUD;

		if (againstLeftWall && !rightWall && !bottomWall && topWall)
			_walls[i]->Texture = wallU;

		if (rightWall && bottomWall && againstLeftWall && !topWall && bottomRightWall)
			_walls[i]->Texture = wallRD;

		if (rightWall && bottomWall && againstLeftWall && !topWall && !bottomRightWall)
			_walls[i]->Texture = wallRD2;

		if (rightWall && !bottomWall && againstLeftWall && topWall && topRightWall)
			_walls[i]->Texture = wallRU;

		if (rightWall && !bottomWall && againstLeftWall && topWall && !topRightWall)
			_walls[i]->Texture = wallRU2;

		if (againstRightWall && !leftWall && bottomWall && !topWall)
			_walls[i]->Texture = wallD;

		if (againstRightWall && !leftWall && bottomWall && topWall)
			_walls[i]->Texture = wallUD;

		if (againstRightWall && !leftWall && !bottomWall && topWall)
			_walls[i]->Texture = wallU;

		if (againstRightWall && !bottomWall && leftWall && topWall && topLeftWall)
			_walls[i]->Texture = wallLU;

		if (againstRightWall && !bottomWall && leftWall && topWall && !topLeftWall)
			_walls[i]->Texture = wallLU2;

		if (againstRightWall && bottomWall && leftWall && !topWall && bottomLeftWall)
			_walls[i]->Texture = wallLD;

		if (againstRightWall && bottomWall && leftWall && !topWall && !bottomLeftWall)
			_walls[i]->Texture = wallLD2;

		if (rightWall && bottomWall && leftWall && topWall && !topRightWall && !topLeftWall && !bottomLeftWall && !bottomRightWall)
			_walls[i]->Texture = wallALL5;

		// If the wall is a gate change to the other texture
		if (_walls[i]->Gate)
			_walls[i]->Texture = wallTex2;
	}
}

void Pacman::Input(int elapsedTime, Input::KeyboardState* state, Input::MouseState*mouseState) // Takes in the inputs of the keyboard and mouse
{
	if ((state->IsKeyDown(Input::Keys::LEFTSHIFT)) && _pacman->speedBoostTimer < 100) // If shift is pressed and timer is less than 100 applies the speedboost
	{
		if (!_pacman->speedBoost) // Plays the speed boost sound once
			Audio::Play(_speed);

		if (!_pacman->recharging) // If its not recharging applies the boost
		{
			_pacman->speedMultiplier = 2.0f;
			_pacman->speedBoost = true;
		}		
	}
	else 
	{
		if (_pacman->speedBoostTimer == 100) // If the speed boost timer is at its max - 100
		{
			Audio::Stop(_speed); // Stop playing the sound
			_pacman->speedMultiplier = 1.0f; // Reset multiplier
			_pacman->speedBoost = false;
			_pacman->recharging = true;
			_pacman->speedBoostCooldownTimer++; // Increases cooldown timer
			UpdatePerc(500, 196); // Calls the function to update the cooldown bar
			if (_pacman->speedBoostCooldownTimer > 500) // If the cooldown timer gets to 500
			{
				// Reset cooldown and boost timer
				_pacman->recharging = false;
				_pacman->speedBoostTimer = 0;
				_pacman->speedBoostCooldownTimer = 0;
				UpdatePerc(100, 196);
			}
		}
	}

	if (state->IsKeyDown(Input::Keys::D) || state->IsKeyDown(Input::Keys::RIGHT)) // If D or RIGHT arrow key is pressed
	{
		if (_menu->tileMap[_pacman->tileMapY][_pacman->tileMapX + 1] == "-") // If there is a gap change direction
		{
			_pacman->Direction = 0;
			_pacman->Moving = true;
		}
	}
	else if (state->IsKeyDown(Input::Keys::A) || state->IsKeyDown(Input::Keys::LEFT)) // If A or LEFT arrow is pressed
	{
		if (_menu->tileMap[_pacman->tileMapY][_pacman->tileMapX - 1] == "-") // If there is a gap change direction
		{
			_pacman->Direction = 2;
			_pacman->Moving = true;
		}
	}
	else if (state->IsKeyDown(Input::Keys::W) || state->IsKeyDown(Input::Keys::UP)) // If W or UP arrow is pressed
	{
		if (_menu->tileMap[_pacman->tileMapY - 1][_pacman->tileMapX] == "-") // If there is a gap change direction
		{
			_pacman->Direction = 3;
			_pacman->Moving = true;
		}
	}
	else if (state->IsKeyDown(Input::Keys::S) || state->IsKeyDown(Input::Keys::DOWN)) // If S or DOWN arrow is pressed
	{
		if (_menu->tileMap[_pacman->tileMapY + 1][_pacman->tileMapX] == "-") // If there is a gap change direction
		{
			_pacman->Direction = 1;
			_pacman->Moving = true;
		}
	}
}

void Pacman::CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey) // Check to see if the game is paused
{
	if (state->IsKeyUp(pauseKey)) // If P is lifted up
	{
		_menu->pKeyDown = false;
	}

	if (state->IsKeyDown(pauseKey) && !_menu->pKeyDown) // If P is down
	{
		_menu->pKeyDown = true;
		_menu->paused = !_menu->paused; // Unpause the menu
	}
}

void Pacman::CheckViewportCollision() // Checks too see if Pacman is colliding with the Viweport walls
{
	if (_pacman->Position->X + _pacman->SourceRect->Width > Graphics::GetViewportWidth()) //1024 right of the screen
	{
		_pacman->Position->X = 0; //Pacman gets reset to the left side
	}
	if (_pacman->Position->X < 0) //0 left of the screen game width
	{
		_pacman->Position->X = Graphics::GetViewportWidth() - _pacman->SourceRect->Width; //Pacman gets reset to the right side
	}
	if (_pacman->Position->Y + _pacman->SourceRect->Height > Graphics::GetViewportHeight()) //768 bottom of the screen
	{
		_pacman->Position->Y = 0; //Pacman gets reset to the top side
	}
	if (_pacman->Position->Y < 0) //0 top of the screen
	{
		_pacman->Position->Y = Graphics::GetViewportHeight() - _pacman->SourceRect->Width; //Pacman gets reset to the bottom side
	}
}

void Pacman::CheckViewportCollision2(MovingEnemy* ghost) // Checks too see if the ghosts are colliding with the viewport walls
{
	if (ghost->position->X + ghost->sourceRect->Width > 1024) //1024 right of the screen
	{
		ghost->position->X = 0; //Ghost gets reset to the left side
		ghost->moveCounter = 0;
		ghost->direction = 3;
		ghost->gotDirection = true;
	}
	if (ghost->position->X < 0) //0 left of the screen game width
	{
		ghost->position->X = 1024 - ghost->sourceRect->Width; //Ghost gets reset to the right side
		ghost->moveCounter = 0;
		ghost->direction = 2;
		ghost->gotDirection = true;
	}
	if (ghost->position->Y + ghost->sourceRect->Height > 768) //768 bottom of the screen
	{
		ghost->position->Y = 0; //Ghost gets reset to the top side
		ghost->moveCounter = 0;
		ghost->direction = 1;
		ghost->gotDirection = true;
	}
	if (ghost->position->Y < 0) //0 top of the screen
	{
		ghost->position->Y = 768 - ghost->sourceRect->Width; //Ghost gets reset to the bottom side
		ghost->moveCounter = 0;
		ghost->direction = 0;
		ghost->gotDirection = true;
	}
}

void Pacman::CheckWallCollision(string direction) // Checks to see if Pacman is colliding with the walls
{

	// Sets the different positions of Pacman
	int i = 0;
	int bottom1 = _pacman->Position->Y + _pacman->SourceRect->Height;
	int bottom2 = 0;
	int left1 = _pacman->Position->X;
	int left2 = 0;
	int right1 = _pacman->Position->X + _pacman->SourceRect->Width;
	int right2 = 0;
	int top1 = _pacman->Position->Y;
	int top2 = 0;
	int center1X = _pacman->Position->X + _pacman->SourceRect->Width / 2;
	int center1Y = _pacman->Position->Y + _pacman->SourceRect->Height / 2;
	int center2X = 0;
	int center2Y = 0;

	for (i = 0; i < _menu->wallCounter; i++) // Loops through all the walls
	{
		// Populate variables with Wall data
		bottom2 = _walls[i]->Position->Y + _walls[i]->sourceRect->Height;
		left2 = _walls[i]->Position->X;
		right2 = _walls[i]->Position->X + _walls[i]->sourceRect->Width;
		top2 = _walls[i]->Position->Y;
		center2X = _walls[i]->Position->X + _walls[i]->sourceRect->Width / 2;
		center2Y = _walls[i]->Position->Y + _walls[i]->sourceRect->Height / 2;

		if (direction == "X") // If the direction is X
			if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2)) // If they are colliding
			{
				if (center2X > center1X) // If its colliding in the left direction
				{
					_pacman->Position->X = left2 - _pacman->SourceRect->Width; // Set the X to the left side of the wall

					// Changes the Y position of the Pacman depending on where it hits the wall
					if (center2Y > center1Y - 12)
					{
						_pacman->Position->Y = top2 - _pacman->SourceRect->Height;
					}
					if (center2Y < center1Y + 12)
					{
						_pacman->Position->Y = bottom2;
					}
					if (center2Y > center1Y - 12 && center2Y < center1Y + 12)
					{
						_pacman->Position->Y = center1Y - _pacman->SourceRect->Height/ 2;
					}
				}
				if (center2X < center1X)  // If its colliding in the right direction
				{
					_pacman->Position->X = right2; // Set the X to the right side of the wall

					// Changes the Y position of the Pacman depending on where it hits the wall
					if (center2Y > center1Y - 12)
					{
						_pacman->Position->Y = top2 - _pacman->SourceRect->Height;
					}
					if (center2Y < center1Y + 12)
					{
						_pacman->Position->Y = bottom2;
					}
					if (center2Y > center1Y - 12 && center2Y < center1Y + 12)
					{
						_pacman->Position->Y = center1Y - _pacman->SourceRect->Height / 2;
					}
				}
				_pacman->speed = 0; // Sets the Pacman speed to 0
			}

		if (direction == "Y") // If the direction is Y
			if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2)) // If they are colliding
			{
				if (center2Y > center1Y) // If its colliding in the top direction
				{
					_pacman->Position->Y = top2 - _pacman->SourceRect->Height; // Set the Y to the top side of the wall

					// Changes the X position of the Pacman depending on where it hits the wall
					if (center2X > center1X - 12)
					{
						_pacman->Position->X = left2 - _pacman->SourceRect->Width;
					}
					if (center2X < center1X + 12)
					{
						_pacman->Position->X = right2;
					}
					if (center2X > center1X - 12 && center2X < center1X + 12)
					{
						_pacman->Position->X = center1X - _pacman->SourceRect->Width / 2;
					}
				}
				if (center2Y < center1Y)  // If its colliding in the bottom direction
				{
					_pacman->Position->Y = bottom2; // Set the Y to the bottom side of the wall

					// Changes the X position of the Pacman depending on where it hits the wall
					if (center2X > center1X - 12)
					{
						_pacman->Position->X = left2 - _pacman->SourceRect->Width;
					}
					if (center2X < center1X + 12)
					{
						_pacman->Position->X = right2;
					}
					if (center2X > center1X - 12 && center2X < center1X + 12)
					{
						_pacman->Position->X = center1X - _pacman->SourceRect->Width / 2;
					}
				}
				_pacman->speed = 0;  // Sets the Pacman speed to 0
			}
	}
}

void Pacman::CheckCherryCollision() // Checks Pacmans collsiion with cherries
{
	// Sets the different positions of Pacman
	int i = 0;
	int bottom1 = _pacman->Position->Y + _pacman->SourceRect->Height - 3;
	int bottom2 = 0;
	int left1 = _pacman->Position->X + 3;
	int left2 = 0;
	int right1 = _pacman->Position->X + _pacman->SourceRect->Width - 3;
	int right2 = 0;
	int top1 = _pacman->Position->Y + 3;
	int top2 = 0;

	for (i = 0; i < _menu->munchieCounter; i++)
	{
		// Populate variables with Ghost data
		bottom2 = _cherry->Position->Y + _cherry->SourceRect->Height;
		left2 = _cherry->Position->X;
		right2 = _cherry->Position->X + _cherry->SourceRect->Width;
		top2 = _cherry->Position->Y;

		if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2)) // If colliding
		{
			Audio::Play(_pop2); //Plays the cherry pop sound
			_menu->scoreAddTimer2 = 0;
			// Resets the cherry position
			_cherry->Position->X = 32 * 20;
			_cherry->Position->Y = 0;
			// Adds 100 to the score
			_menu->playerScore += 100;
		}
	}
}


void Pacman::CheckMunchieCollision() // Checks Pacmans collisions with muchies
{

	// Sets the different positions of Pacman
	int i = 0;
	int bottom1 = _pacman->Position->Y + _pacman->SourceRect->Height-3;
	int bottom2 = 0;
	int left1 = _pacman->Position->X +3;
	int left2 = 0;
	int right1 = _pacman->Position->X + _pacman->SourceRect->Width -3;
	int right2 = 0;
	int top1 = _pacman->Position->Y +3;
	int top2 = 0;

	for (i = 0; i < _menu->munchieCounter; i++) // Loops through all the munchies
	{
		// Populate variables with Ghost data
		bottom2 = _munchies[i]->Position->Y + _munchies[i]->SourceRect->Height;
		left2 = _munchies[i]->Position->X;
		right2 = _munchies[i]->Position->X + _munchies[i]->SourceRect->Width;
		top2 = _munchies[i]->Position->Y;

		if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2)) // If colliding
		{
			_munchies[i]->dead = true;
			// Sets the munchie position of the map
			_munchies[i]->Position->X = -1000;
			_munchies[i]->Position->Y = -1000;
			_munchies[i]->SourceRect = new Rect(-1000, -1000, 12, 12);

			if (_munchies[i]->type == "Powerup") // If the munchie is a powerup
			{
				Audio::Play(_inter); // Play the frigtened sound
				_menu->ghostFrightTimer = 0; // Resets the timer

				for (int i = 0; i < _menu->ghostCounter; i++) // Loops through all the ghosts
				{
					// Resets the counters
					_ghosts[i]->frightCounter = 0;
					_ghosts[i]->frightCounter2 = 0;
					_ghosts[i]->frightCounter3 = 0;
					// Resets the image rect
					_ghosts[i]->sourceRect->Y = 0;
					// Changes the texture to frightneed ghost
					_ghosts[i]->texture = _menu->frightTexture;
					// Sets them as destructable
					_ghosts[i]->destructable = true;
				}
				_menu->ghostFright = true; // Sets in fright mode to true
					
			}
			_menu->playerScore += 10; // Adds 10 to the player score
			i = _menu->munchieCounter; // Resets i so it can carry on looping from that pooint
		}
	}
}

void Pacman::CheckGhostCollisions()// Checks Pacmans collisions with the ghosts
{

	// Sets the different positions of Pacman
	int i = 0;
	int bottom1 = _pacman->Position->Y + _pacman->SourceRect->Height -3;
	int bottom2 = 0;
	int left1 = _pacman->Position->X +3;
	int left2 = 0;
	int right1 = _pacman->Position->X + _pacman->SourceRect->Width -3;
	int right2 = 0;
	int top1 = _pacman->Position->Y +3;
	int top2 = 0;

	for (i = 0; i < GHOSTCOUNT; i++) // Loops through all the ghosts
	{
		// Populate variables with Ghost data
		bottom2 = _ghosts[i]->position->Y + _ghosts[i]->sourceRect->Height;
		left2 = _ghosts[i]->position->X;
		right2 = _ghosts[i]->position->X + _ghosts[i]->sourceRect->Width;
		top2 = _ghosts[i]->position->Y;

		if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2)) // If colliding
		{
			if (_ghosts[i]->destructable) // If the ghost is destructable
			{
				Audio::Play(_ghostdeath); // Play the ghost death sound
				_menu->scoreAddTimer = 0; // Resets the timer
				
				// For the first Add add 200 for the rest multiply the score by 2 for each ghost you eat in that frightened stage
				if (!_menu->addedOnce)
				{
					_menu->scoreAdd = _menu->scoreAdd;
					_menu->addedOnce = true;
				}
				else
				{
					_menu->scoreAdd = _menu->scoreAdd * 2;
				}


				_menu->playerScore += _menu->scoreAdd; // Add the score to the player score
				resetGhostsTexture(_ghosts[i]); // Calls the function to reset the ghost texture back
				_ghosts[i]->moveCounter = 0; // Resets the move counter
				_ghosts[i]->direction = 3; // Sets the position to up
				_ghosts[i]->destructable = false; // Sets it to not destructable
				// Restes the position to the start
				_ghosts[i]->position->X = 32*14;
				_ghosts[i]->position->Y= 32*14;

			}
			else // If not destructable
			{
				Audio::Stop(_speed); // Stop the speed sound
				Audio::Play(_death); // Play the death sound
				_pacman->dead = true; // Sets pacman to dead
				// Sets pacmans position to off the map
				_pacman->Position->X = -100000;
				_pacman->Position->Y = -100000;
				i = GHOSTCOUNT;
				_menu->screen = "Game Over"; // Sets the game screen to game over
				
			}
			
		}
	}
}

void Pacman::CheckWallCollision2(string direction, MovingEnemy* ghost) // Checks to see if the ghost being passed in is colliding with the walls
{
	// Sets the different positions of the Ghost
	int i = 0;
	int bottom1 = ghost->position->Y + ghost->sourceRect->Height;
	int bottom2 = 0;
	int left1 = ghost->position->X;
	int left2 = 0;
	int right1 = ghost->position->X + ghost->sourceRect->Width;
	int right2 = 0;
	int top1 = ghost->position->Y;
	int top2 = 0;
	int center1X = ghost->position->X + ghost->sourceRect->Width / 2;
	int center1Y = ghost->position->Y + ghost->sourceRect->Height / 2;
	int center2X = 0;
	int center2Y = 0;

	for (i = 0; i < _menu->wallCounter; i++) // Loops through all the walls
	{
		// Populate variables with Ghost data
		bottom2 = _walls[i]->Position->Y + _walls[i]->sourceRect->Height;
		left2 = _walls[i]->Position->X;
		right2 = _walls[i]->Position->X + _walls[i]->sourceRect->Width;
		top2 = _walls[i]->Position->Y;
		center2X = _walls[i]->Position->X + _walls[i]->sourceRect->Width / 2;
		center2Y = _walls[i]->Position->Y + _walls[i]->sourceRect->Height / 2;

		if (!_walls[i]->Gate) // If its not a gate
		{

			// Same code used as before in Pacman Wall collision
			if (direction == "X")
				if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2))
				{
					if (center2X > center1X)
					{
						ghost->position->X = left2 - ghost->sourceRect->Width;
						ghost->moveRight = false;
					}
					if (center2X < center1X)
					{
						ghost->position->X = right2;
						ghost->moveLeft = false;
					}
					ghost->speed = 0;
				}
			if (direction == "Y")
				if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2))
				{
					if (center2Y > center1Y)
					{
						ghost->position->Y = top2 - ghost->sourceRect->Height;
						ghost->moveDown = false;
					}
					if (center2Y < center1Y)
					{
						ghost->position->Y = bottom2;
						ghost->moveUp = false;
					}
					ghost->speed = 0;
				}
		}
	}
}

void Pacman::UpdatePacman(int elapsedTime) // Updates various elements of Pacman
{
	_menu->scoreAddPosition = new Vector2(_pacman->Position->X, _pacman->Position->Y - 5); // Sets the one of the score positions
	_menu->scoreAddPosition2 = new Vector2(_pacman->Position->X, _pacman->Position->Y + 45); // Sets the other score position
	_pacman->tileMapX = (_pacman->Position->X + (_pacman->SourceRect->Width / 2)) / 32; // Sets the Pacman tilemap X from the centre of Pacman
	_pacman->tileMapY = (_pacman->Position->Y + (_pacman->SourceRect->Height / 2)) / 32; // Sets the Pacman tilemap Y from the centre of Pacman

	if (_pacman->speedBoost) // If speed boost is active
	{
		UpdatePerc(100, 196); // Update the bar
		_pacman->speedBoostTimer++; // Increase timer
	}

	if (!_pacman->Moving) // If pacman is not moving
		_pacman->SourceRect->X = 0; // Reset the source rect image so that pacmans mouth is closed
	else // If moving
	{
		_pacman->SourceRect->X = _pacman->SourceRect->Width * _pacman->Frame; // Sets the source rect X depending on the frame

		if (_pacman->speed == 0) // If Pacman is colliding with a wall
		{
			_pacman->CurrentFrameTime = 0; // Reset frame time
			Audio::Stop(_pop); // Stop playing moving sound
			_pacman->soundPlaying = false; // Reset sound playing
			_pacman->soundCounter = 0; // Reset sound counter
			_pacman->Moving = false; // Set moving to false
		}

		if (_pacman->Direction == 1) // If direction is 1
		{
			_pacman->speed = 0.125f; // Sets speed
			_pacman->Position->Y += _pacman->speed * elapsedTime * _pacman->speedMultiplier; //Moves Pacman down Y axix
			_pacman->SourceRect->Y = 96; // Sets source rect image to Pacman facing down
		}
		if (_pacman->Direction == 3) // If direction is 3
		{
			_pacman->speed = 0.125f; // Sets speed
			_pacman->Position->Y -= _pacman->speed * elapsedTime * _pacman->speedMultiplier; //Moves Pacman up Y axis
			_pacman->SourceRect->Y = 64;  // Sets source rect image to Pacman facing up
		}
		if (_pacman->Direction == 2) // If direction is 2
		{
			_pacman->speed = 0.125f; // Sets speed
			_pacman->Position->X -= _pacman->speed * elapsedTime * _pacman->speedMultiplier; //Moves Pacman left across X axis
			_pacman->SourceRect->Y = 32;  // Sets source rect image to Pacman facing left
		}
		if (_pacman->Direction == 0) // If direction is 0
		{
			_pacman->speed = 0.15f; // Sets speed
			_pacman->Position->X += _pacman->speed * elapsedTime * _pacman->speedMultiplier; //Moves Pacman right across X axis
			_pacman->SourceRect->Y = 128;  // Sets source rect image to Pacman facing right
		}
		if (_pacman->Direction == 4) // If direction is 4
		{
			_pacman->SourceRect->X = 0;  // Sets source rect image to Pacman with mouth closed
		}
	}
	_pacman->CurrentFrameTime += elapsedTime; // Increase frame time
}

void Pacman::resetGhostsTexture(MovingEnemy* ghost) // Rests ghost image
{
	// Resets the ghost image depending on there name so they reset to the same colours
	if (ghost->name == "Inky")
		ghost->texture = _menu->ghostBlue;
	if (ghost->name == "Blinky")
		ghost->texture = _menu->ghostRed;
	if (ghost->name == "Sue")
		ghost->texture = _menu->ghostPurple;
	if (ghost->name == "Clyde")
		ghost->texture = _menu->ghostOrange;
	if (ghost->name == "Funky")
		ghost->texture = _menu->ghostGreen;
}

void Pacman::updateGhostImage(MovingEnemy* ghost) // Update ghost image
{
	if (!ghost->destructable) // If ghost is not destructable
	{
		// Chnages ghost source rect image depending on direction to correspond with facing the right direction
		if (ghost->direction == 0)
			ghost->sourceRect->X = 0;
		if (ghost->direction == 1)
			ghost->sourceRect->X = 32;
		if (ghost->direction == 2)
			ghost->sourceRect->X = 96;
		if (ghost->direction == 3)
			ghost->sourceRect->X = 64;
	}
	if (_menu->ghostFright) // If the ghosts are in fright mode
	{
		_menu->ghostFrightTimer++; // Increase fright timer

		if (_menu->ghostFrightTimer == 4000) // If the timer is at 4000
		{
			for (int i = 0; i < _menu->ghostCounter; i++) // Loop through all the ghosts
			{
				resetGhostsTexture(_ghosts[i]); // Resets the ghost textures
				_ghosts[i]->destructable = false; // Sets the ghost to not destructable
			}
			_menu->scoreAdd = 200; // Resets the score add to 200
			_menu->addedOnce = false; // Resets added once
			_menu->ghostFright = false; // Resets the ghost fright
		}
		if (ghost->destructable) // If ghost is destructable
		{
			if (_menu->ghostFrightTimer > 2000 && _menu->ghostFrightTimer < 3500) // If the timer is between 2000-3500
			{

				// Starts flashing between white and blue on the sprite sheet at a steady rate to show time is almost up
				if (ghost->frightCounter2 == 50)
					ghost->frightCounter2 = 0;
				if (ghost->frightCounter2 < 25)
					ghost->sourceRect->Y = 0;
				if (ghost->frightCounter2 >= 25 && ghost->frightCounter2 < 50)
					ghost->sourceRect->Y = 32;
				ghost->frightCounter2++; // Increase timer
			}
			if (_menu->ghostFrightTimer > 3500 && _menu->ghostFrightTimer < 4000) // If the timer is between 3500-4000
			{
				// Starts flashing between white and blue on the sprite sheet at a fast rate to show time is about to be up
				if (ghost->frightCounter3 == 20)
					ghost->frightCounter3 = 0;
				if (ghost->frightCounter3 < 10)
					ghost->sourceRect->Y = 0;
				if (ghost->frightCounter3 >= 10 && ghost->frightCounter3 < 20)
					ghost->sourceRect->Y = 32;
				ghost->frightCounter3++; // Increase timer
			}

			// Changes the image on the sprite sheet so that his mouth moves up and down but not change the colour
			if (ghost->frightCounter == 100)
				ghost->frightCounter = 0;
			if (ghost->frightCounter < 50)
				ghost->sourceRect->X = 0;
			if (ghost->frightCounter >= 50 && ghost->frightCounter < 100)
				ghost->sourceRect->X = 32;
			ghost->frightCounter++; // Increase timer 
		}
	}
}

void Pacman::UpdateGhost(MovingEnemy* ghost, int elapsedTime, string movementType) // Updates the ghosts various elements
{
	// Sets the ghost tile map position
	ghost->tileMapX = ghost->position->X / 32;
	ghost->tileMapY = ghost->position->Y / 32;
	updateGhostImage(ghost); // Updates the ghost image
	CheckViewportCollision2(ghost); // Checks too see if the ghost is colliding with the edges of the screen

	if (movementType == "randomUponTile") // If movement type is random upon tile
	{
		if (ghost->gotDirection) // If the ghost has a direction
		{
			ghost->moveCounter = 0; // Resets the counter
			ghost->gotDirection = false; // Sets the got direction to false
		}

		else if (ghost->moveCounter < 16) // If counter is less than 16
		{
			ghost->speed = 2; // Set the speed to 2
			  
			if (ghost->direction == 0) // If the direction is 0
			{
				ghost->position->Y -= ghost->speed; // Move up
				ghost->moveCounter++; // Increase counter
			}
			if (ghost->direction == 1)  // If the direction is 1
			{
				ghost->position->Y += ghost->speed; // Move down
				ghost->moveCounter++; // Increase counter
			}
			if (ghost->direction == 2) // If the direction is 2
			{
				ghost->position->X -= ghost->speed; // Move left
				ghost->moveCounter++; // Increase counter
			}
			if (ghost->direction == 3)  // If the direction is 3
			{
				ghost->position->X += ghost->speed; // Move right
				ghost->moveCounter++; // Increase counter
			}
		}

		else if (ghost->moveCounter == 16) // If the counter is 16
		{
			if (!ghost->gotDirection) // If the ghost has no direction
			{
				ghost->direction = pickDirection("Random", ghost); // Picks a random direction
				ghost->gotDirection = true; // Sets got direction as true
				ghost->randomCounter++; // Increases random counter
			}
		}
	}
	if (movementType == "followSight") // If the movement type is follow sight
	{
		ghost->previousDirection = ghost->direction; // Sets the previous direction as the direction now

		// Same code as random upon tile
		if (ghost->gotDirection)
		{
			ghost->moveCounter = 0;
			ghost->gotDirection = false;
		}

		else if (ghost->moveCounter < 16)
		{
			ghost->speed = 2;
			if (ghost->direction == 0)
			{
				ghost->position->Y -= ghost->speed;
				ghost->moveCounter++;
			}
			if (ghost->direction == 1)
			{
				ghost->position->Y += ghost->speed;
				ghost->moveCounter++;
			}
			if (ghost->direction == 2)
			{
				ghost->position->X -= ghost->speed;
				ghost->moveCounter++;
			}
			if (ghost->direction == 3)
			{
				ghost->position->X += ghost->speed;
				ghost->moveCounter++;
			}
		}

		else if (ghost->moveCounter == 16)
		{
			if (!ghost->gotDirection)
			{
				ghost->direction = pickDirection("followSight", ghost); // Picks a direction towards the player
				ghost->gotDirection = true;
			}
		}
	}
}

int Pacman::pickDirection(string type, MovingEnemy* ghost) // Returns a direction for the ghost
{
	if (type == "Random") // If type is random
	{
		if (ghost->direction == 1) // If direction is 1
		{
			vector <int> temp; // Creates vector
			if (ghost->tileMapY == 23) // If against the bottom wall
			{
				temp.clear(); // Clear vector
				temp.push_back(1); // Add down to the vector
			}
			else // If not against the wall
			{
				if (_menu->tileMap[ghost->tileMapY][ghost->tileMapX - 1] == "-") // Checks left for a gap
				{
					// Add left to the vector
					temp.push_back(2);
				}
				if (_menu->tileMap[ghost->tileMapY][ghost->tileMapX + 1] == "-") // Checks right for a gap
				{
					// Add right to the vector
					temp.push_back(3);
				}
				if (_menu->tileMap[ghost->tileMapY + 1][ghost->tileMapX] == "-") // Checks down for a gap
				{
					// Add down to the vector
					temp.push_back(1);
				}
				if (temp.empty())
				{
					// If its empty, add up to the vector
					temp.push_back(0);
				}
			}
			int ranNum = rand() % temp.size(); // Picks a random number from the vector size
			return temp[ranNum]; // Returns a random direction from vector
		}

		// Up
		else if (ghost->direction == 0) // If direction is 0
		{
			vector <int> temp; // Creates temp vector
			if (ghost->tileMapY == 0) // If against the top wall
			{
				temp.clear(); // Clear vector
				temp.push_back(0); // Add up to the vector
			}
			else // If not against the wall
			{
				if (_menu->tileMap[ghost->tileMapY][ghost->tileMapX - 1] == "-") // Checks left for a gap
				{
					// Add left to the vector
					temp.push_back(2);
				}
				if (_menu->tileMap[ghost->tileMapY][ghost->tileMapX + 1] == "-") // Checks right for a gap
				{
					// Add right to the vector
					temp.push_back(3);
				}
				if (_menu->tileMap[ghost->tileMapY - 1][ghost->tileMapX] == "-") // Checks up for a gap
				{
					// Add up to the vector
					temp.push_back(0);
				}
				if (temp.empty())
				{
					// If its empty, add down to the vector
					temp.push_back(1);
				}
			}
			int ranNum = rand() % temp.size(); // Picks a random number from the vector size
			return temp[ranNum]; // Returns a random direction from vector
		}

		// Left
		else if (ghost->direction == 2) // If direction is 2
		{
			vector <int> temp; // Creates a vector
			if (ghost->tileMapX == 0) // If agaisnt the left wall
			{
				temp.clear(); // Clear the vector
				temp.push_back(2); // Add left to the vector
			}
			else // If not agaisnt the wall
			{
				if (_menu->tileMap[ghost->tileMapY][ghost->tileMapX - 1] == "-") // Checks left for a gap
				{
					// Add left to the vector
					temp.push_back(2);
				}
				if (_menu->tileMap[ghost->tileMapY + 1][ghost->tileMapX] == "-") // Checks down for a gap
				{
					// Add down to the vector
					temp.push_back(1);
				}
				if (_menu->tileMap[ghost->tileMapY - 1][ghost->tileMapX] == "-") // Checks up for a gap
				{
					// Add up to the vector
					temp.push_back(0);
				}
				if (temp.empty())
				{
					// If its empty, add right to the vector
					temp.push_back(3);
				}
			}
			int ranNum = rand() % temp.size(); // Picks a random number from the vector size
			return temp[ranNum]; // Returns a random direction from the vector
		}

		// Right
		else if (ghost->direction == 3) // If direction is 3
		{
			vector <int> temp; // Creates a vector
			if (ghost->tileMapX == 31) // If agaisnt the right wall
			{
				temp.clear(); // Clear the vector
				temp.push_back(3); // Add right to the vector
			}
			else
			{
				if (_menu->tileMap[ghost->tileMapY][ghost->tileMapX + 1] == "-") // Checks right for a gap
				{
					// Add right to the vector
					temp.push_back(3);
				}
				if (_menu->tileMap[ghost->tileMapY + 1][ghost->tileMapX] == "-") // Checks down for a gap
				{
					// Add down to the vector
					temp.push_back(1);
				}
				if (_menu->tileMap[ghost->tileMapY - 1][ghost->tileMapX] == "-") // Checks up for a gap
				{
					// Add up to the vector
					temp.push_back(0);
				}
				if (temp.empty())
				{
					// If its empty, add left to the vector
					temp.push_back(2);
				}
			}
			int ranNum = rand() % temp.size(); // Picks a random number from the vector size
			return temp[ranNum]; // Returns a random direction from the vector
		}
	}
	if (type == "followSight") // If type is follow sight
	{
		// Create direction bools and set them as false
		bool moveLeft = false;
		bool moveRight = false;
		bool moveDown = false;
		bool moveUp = false;

		vector <int> temp; // Creates a vector
		if (ghost->direction == 1 || ghost->direction == 2 || ghost->direction == 3 || ghost->direction == 0) // If the ghost has a moving direction
		{
			if (ghost->tileMapY == 23) // If agaisnt the bottom wall
			{
				return 1; // Return down
			}
			else if (ghost->tileMapY == 0) // If agaisnt the top wall
			{
				return 0; // Return up
			}
			else if (ghost->tileMapX == 0) // If agaisnt the left wall
			{
				return 2; // Return left
			}
			else if (ghost->tileMapX == 31) // If agaisnt the right wall
			{
				return 3; // Return right
			}
			else
			{
				if (_menu->tileMap[ghost->tileMapY][ghost->tileMapX - 1] == "-") // Checks left for a gap
				{
					temp.push_back(2); // Adds left to the vector
					moveLeft = true; // Sets left as true
				}
				if (_menu->tileMap[ghost->tileMapY][ghost->tileMapX + 1] == "-") // Checks right for a gap
				{
					temp.push_back(3); // Adds right to the vector
					moveRight = true; // Sets right as true
				}
				if (_menu->tileMap[ghost->tileMapY + 1][ghost->tileMapX] == "-") // Checks down for a gap
				{
					temp.push_back(1); // Adds down to the vector
					moveDown = true; // Sets down as true

				}
				if (_menu->tileMap[ghost->tileMapY - 1][ghost->tileMapX] == "-") // Checks up for a gap
				{
					temp.push_back(0); // Adds up to the vector
					moveUp = true; // Sets up as true
				}
			}

			vector <int> distances; // Creates a vector
			vector <int> directions; // Creates a second vector

			// Creates direction integers and sets them as 0
			int xLeft = 0;
			int xRight = 0;
			int yDown = 0;
			int yUp = 0;

			if (moveRight && moveLeft) // If left and right is true
			{
				if (ghost->tileMapX - _pacman->tileMapX < 0) // If the ghost tile X - pacman tile X is less than 0
				{
					xRight = abs(ghost->tileMapX - _pacman->tileMapX); // Calculates difference in X
					distances.push_back(xRight); // Adds differece to the vector
					directions.push_back(3); // Adds Right to the vector
				}

				else if (ghost->tileMapX - _pacman->tileMapX > 0)  // If the ghost tile X - pacman tile X is greater than 0
				{
					xLeft = abs(ghost->tileMapX - _pacman->tileMapX); // Calculates differnce in X
					distances.push_back(xLeft); // Adds differnce to the vector
					directions.push_back(2); // Adds left to the vector
				}
			}

			if (moveLeft && !moveRight) // If left is true and right is false
			{
				xLeft = abs(ghost->tileMapX - _pacman->tileMapX); // Calculates differnce in X
				distances.push_back(xLeft); // Adds differnce to the vector
				directions.push_back(2); // Adds left to the vector
			}

			if (moveRight && !moveLeft) // If right is true and left is false
			{
				xRight = abs(ghost->tileMapX - _pacman->tileMapX); // Calculates differnce in X
				distances.push_back(xRight); // Adds difference to the vector
				directions.push_back(3); // Adds right to the vector
			}

			if (!moveRight && !moveLeft) // If right and left is false
			{
				distances.push_back(0); // Adds 0 to the distance
				directions.push_back(ghost->previousDirection); // Adds previous direction to vector
			}

			if (moveDown && moveUp) // If up and down is true
			{
				if (ghost->tileMapY - _pacman->tileMapY < 0) // If the ghost tile Y - pacman tile Y is less than 0
				{
					yDown = abs(ghost->tileMapY - _pacman->tileMapY);  // Calculates differnce in Y
					distances.push_back(yDown); // Adds difference to the vector
					directions.push_back(1);  // Adds down to the vector
				}
				if (ghost->tileMapY - _pacman->tileMapY > 0) // If the ghost tile X - pacman tile X is greater than 0
				{
					yUp = abs(ghost->tileMapY - _pacman->tileMapY); // Calculates differnce in Y
					distances.push_back(yUp); // Adds difference to the vector
					directions.push_back(0);  // Adds up to the vector
				}
			}

			if (moveUp && !moveDown) // If up is true and down is false
			{
				yUp = abs(ghost->tileMapY - _pacman->tileMapY); // Calculates differnce in Y
				distances.push_back(yUp); // Adds difference to the vector
				directions.push_back(0);  // Adds up to the vector
			}

			if (moveDown && !moveUp) // If down is true and up is false
			{
				yDown = abs(ghost->tileMapY - _pacman->tileMapY); // Calculates differnce in Y
				distances.push_back(yDown); // Adds difference to the vector
				directions.push_back(1);  // Adds down to the vector
			}

			if (!moveUp && !moveDown) // If up and down is false
			{
				distances.push_back(0); // Adds 0 to the distance
				directions.push_back(ghost->previousDirection); // Adds previous direction to the vector
			}

			ghost->randomCounter++; // Increases counter

			int largest = distances[0]; // Set the largest as 0
			int largestDirection = directions[0]; // Set largest as 0

			for (int i = 0; i < distances.size(); i++) // Loop through whole vector
			{
				if (distances[i] > largest) // If the element in distnaces is larger than largest
				{
					largest = distances[i]; // Set largest as element in distnaces
					largestDirection = directions[i]; // Set largest direction as element in directions
				}
			}
			return largestDirection; // Return the largest direction
		}
	
	}
}


void Pacman::displayMap(Menu* menu) // Displays tile map
{
	for (int i = 0; i < 24; ++i) // Loops through each collum
	{
		for (int j = 0; j < 32; ++j) // Loops throgh each row
		{
			cout << menu->tileMap[i][j]; // Prints the element in tha row and collum
		}
		cout << endl; // New line
	}
	cout << endl; // New line
	cout << endl; // New line
}

void Pacman::UpdateMunchie(Enemy* munchie, int elapsedTime) // Updates elements for munchies
{
	munchie->SourceRect->X = 12 * munchie->frameCount; // Changes source rect image depending on frame
	munchie->CurrentFrameTime += elapsedTime; // Icreases frame time

	if (munchie->CurrentFrameTime > munchie->FrameTime) // If  current frame time is greater tha frame time
	{
		munchie->frameCount++; // Increases frame count

		if (munchie->frameCount >= 2) // If frame count is greater than or equal to 2
			munchie->frameCount = 0; // Reset frame count to 0

		munchie->CurrentFrameTime = 0; // Reset current frame time to 0
	}
}

void Pacman::UpdateCherry(Enemy* cherry, int elapsedTime) // Update elements for the cherry
{
	if (_menu->playerScore >= _cherry->scoreProp) // If player score is greater than or equal to score prop
	{
		_cherry->scoreProp += 2500; // Increase score prop by 2500
		_cherry->lifetime = 0; // Reset lifetime
		_cherry->gotPosition = false; // Set got position to false
	}

	if (_cherry->lifetime < 1000) // If lifetime is less than 1000
	{
		_cherry->lifetime++; // Increase lifetime counter
		if (!_cherry->gotPosition) // If got position is false
		{
			bool looping = true; // Set looping as true
			int x = 0; // Create int X and set as 0
			int y = 0; // Create int Y and set as 0
			while (looping) // While looping is true
			{
				x = rand() % 32; // Pick a random tile X on the map
				y = rand() % 24; // Pick a random tile Y on the map

				if (_menu->tileMap[y][x] == "-") // If the postiion is a gap
				{
					looping = false; // Break the loop
					continue;
				}
			}

			_cherry->Position->X = x * 32; // Changes X from tile to pixel
			_cherry->Position->Y = y * 32; // Changes Y from tile to pixel
			_cherry->gotPosition = true; // Sets got position to true
		}

		cherry->SourceRect->X = cherry->SourceRect->Width * cherry->frameCount; // Changes source rect image depending on the frame count
		cherry->CurrentFrameTime += elapsedTime; // Increase frame time

		if (cherry->CurrentFrameTime > cherry->FrameTime) // If current frame time is greater tha frame time
		{
			cherry->frameCount++; // Increase frame count

			if (cherry->frameCount >= 2) // If frame count is greater than or equal to 2
				cherry->frameCount = 0; // Resets the frame count to 0

			cherry->CurrentFrameTime = 0; // Resets the current frame time to 0
		}
	}
	else // If greater than lifetime 
	{
		// Resets the cherry posiion to top of screen in wall
		_cherry->Position->X = 32 * 20; 
		_cherry->Position->Y = 0;
	}

}

void Pacman::UpdatePerc(int length, int barLength) // Updates the bar percentage
{
	float a; // Creates float a
	float b; // Creates float b

	if (_pacman->speedBoostTimer == 100) // IF the speed boost timer is equal to 100
	{
		a = _pacman->speedBoostCooldownTimer; // Sets a as the cooldown timer
		b = (((a) / length) * 100); // Converts a into percentage
		float d = _pacman->speedBoostCooldownTimer; // Creates d and sets it as cooldwon timer
		d = 500 - d; // -500 of cooldown timer
		int e = (d / 100) + 1; // Creates an int e and sets it as d divided by 100 + 1
		_pacman->cooldownCount = e; // Sets cooldown count to e 
	}
		
	else // If the speedboost timer is less than 100
	{
		a = _pacman->speedBoostTimer; // Sets a as boost timer
		b = (((length - a) / length) * 100); // Converts a into percentage
	}

	float c = (b / 100) * barLength; // Creates c and sets it as b divided by 100 times by the legth of the bar to get the percentage of the bar
	_pacman->barPerc = c; // Sets the bar percetage as C
}

void Pacman::Update(int elapsedTime) // Updates all main game functions
{
	Input::KeyboardState* keyboardState = Input::Keyboard::GetState(); // Gets the current state of the keyboard
	Input::MouseState* mouseState = Input::Mouse::GetState(); // Gets the current state of the mouse
	Input::Keyboard::GetState();

	if (!_menu->musicPlaying) // If the music is not playing
	{
		//Audio::Play(_music); // Plays the music
		_menu->musicPlaying = true; // Sets music playing as true
	}

	if (_menu->musicCounter == 5200) // If the counter is 5200
	{
		//Audio::Stop(_music); // Stop playing the music
		_menu->musicCounter = 0; // Reset the music counter
		_menu->musicPlaying = false; // Sets the music playing to false
	}

	if (!_pacman->dead) // If pacman isnt dead
	{
		if (_pacman->Moving) // If pacman is moving
		{
			if (!_pacman->soundPlaying) // If the sound isnt playing
			{
				Audio::Play(_pop); // Plays the pop sound
				_pacman->soundPlaying = true; // Set the sound as playing
			}
			_pacman->soundCounter++; // Increase sound counter

			if (_pacman->soundCounter > 40) // If the counter is greater than 40
			{
				_pacman->soundPlaying = false; // Sets the sound as not playing
				_pacman->soundCounter = 0; // Resets the sound counter
			}
		}
	}
	_menu->musicCounter++; // Increases the music counter

	if (_menu->screen == "Playing") // If the menu screen is Playing
	{
		int deathCounter = 0; // Create death counter and set as 0
		for (int i = 0; i < _menu->munchieCounter; i++) // Loop through all the munchies
		{
			if (_munchies[i]->dead) // If the munchie is dead
				deathCounter++; // Increase the death counter
		}

		if (deathCounter == _menu->munchieCounter) // If all the munchies are dead
		{
			Audio::Stop(_speed); // Stop the speed sound
			Audio::Play(_death); // Play the death sound
			_pacman->dead = true; // Sets pacman to dead
			// Sets pacmans position to off the map
			_pacman->Position->X = -100000;
			_pacman->Position->Y = -100000;
			_menu->screen = "Game Over"; // Sets the game screen to game over
		}

		if (!_menu->startPlayed) // If start played is false
		{
			Audio::Play(_coin); // Play coin sound
			_menu->startPlayed = true; // Sets start played as true
		}

		CheckPaused(keyboardState, Input::Keys::P); //Check if game is paused

		if (!_menu->paused) // If not pasued
		{
			Input(elapsedTime, keyboardState, mouseState); //Calls the keyboard input function
			UpdatePacman(elapsedTime);  //Calls the update function for pacman

			if (!_pacman->dead) // If pacman isnt dead
			{
				for (int i = 0; i < GHOSTCOUNT; i++) // Loops through all the ghosts
				{
					if (_ghosts[i]->destructable) // IF the ghost is destructable
					{
						_ghosts[i]->randomCounter = rand() % 10; // Reset the random counter between 0-10
					}

					if (_ghosts[i]->randomCounter <= 30) // If the random counter is less than or equal to 30
					{
						UpdateGhost(_ghosts[i], elapsedTime, "randomUponTile"); // Updates the ghost as random direction
					}

					if (_ghosts[i]->randomCounter > 30 && _ghosts[i]->randomCounter <= 45) // If the random counter is between 31 and 45
					{
						UpdateGhost(_ghosts[i], elapsedTime, "followSight"); // Updates the ghost as follow pacman
					}

					if (_ghosts[i]->randomCounter == 45) // If the random counter is equal to 45
					{
						_ghosts[i]->randomCounter = 0; // Reset the counter to 0
					}
				}
			}
			CheckGhostCollisions(); // Calls the function that checks Ghost collisions
			UpdateCherry(_cherry, elapsedTime);// Calls the update for the Cherry
			CheckCherryCollision(); // Calls the function that checks for cherry collisions
			for (int i = 0; i < _menu->munchieCounter; i++) // Loop to update each munchie
				UpdateMunchie(_munchies[i], elapsedTime); //Calls the update function for the munchie
			CheckMunchieCollision(); // Calls the function that checks Munchie collisions
			CheckViewportCollision(); //Calls the function that checks Pacman collisions

			if (_pacman->Direction == 0 || _pacman->Direction == 2) // If the direction is X
				CheckWallCollision("X"); // Calls the fucntion to check wall collisions in the X direction
			else if (_pacman->Direction == 1 || _pacman->Direction == 3) // If the direction is Y
				CheckWallCollision("Y");  // Calls the fucntion to check wall collisions in the Y direction
		}
	}
}

void Pacman::Draw(int elapsedTime) // Draws all the features for the game
{
	std::stringstream stream1, stream2, stream3, stream4, stream5; // Creates string streams 

	if (_menu->screen == "Playing") // IF the menu screen is Playing
	{
		if (_pacman->Moving && !_menu->paused) // If pacman moving and screen is not paused
		{
			if (_pacman->CurrentFrameTime > _pacman->FrameTime) // If current frame time is greater than frame time
			{
				_pacman->Frame++; // Increases frame
				if (_pacman->Frame >= 2) // If frame is greater than or equal to 2
					_pacman->Frame = 0; // Resets the frame to 0
				
				_pacman->CurrentFrameTime = 0; // Resets the current frame time
			}
		}
		SpriteBatch::BeginDraw(); // Starts Drawing
		if (!_pacman->dead) // Checks if pacman is alive
		{
			SpriteBatch::Draw(_pacman->Texture, _pacman->Position, _pacman->SourceRect); // Draws Pacman
		}

		for (int i = 0; i < _menu->munchieCounter; i++) // Loops through all the munchies
		{
			if (!_munchies[i]->dead) // If the munchies arent dead
			{
				SpriteBatch::Draw(_munchies[i]->Texture, _munchies[i]->Position, _munchies[i]->SourceRect); // Draws munchies
			}
		}

		for (int i = 0; i < GHOSTCOUNT; i++) // Loops through all the ghosts
		{
			SpriteBatch::Draw(_ghosts[i]->texture, _ghosts[i]->position, _ghosts[i]->sourceRect); // Draws Ghosts
		}

		for (int i = 0; i < _menu->wallCounter; i++) // Loops through all the walls
		{
			SpriteBatch::Draw(_walls[i]->Texture, _walls[i]->Position, _walls[i]->sourceRect); // Draws Walls
		}

		SpriteBatch::DrawRectangle(new Rect(245, 1, 200, 30), Color::White); // Draws boost bar

		stream3 << "Score: " << _menu->playerScore; // Sets stream
		SpriteBatch::DrawString(stream3.str().c_str(), _textPosition3, Color::Yellow); // Draws stream

		if (_pacman->speedBoostTimer == 100) // If the speed boost timer is equal to 100
		{
			SpriteBatch::DrawRectangle(new Rect(247, 3, _pacman->barPerc, 26), Color::Red); // Draws bar
			stream2 << "Speed Boost recharging: " << _pacman->cooldownCount; // Sets stream
			SpriteBatch::DrawString(stream2.str().c_str(), _textPosition1, Color::Red); // Draw stream
		}
		else // If speed boost timer is less than 100
		{
			SpriteBatch::DrawRectangle(new Rect(247, 3, _pacman->barPerc, 26), Color::Magenta); // Draws Bar
			stream2 << "Speed Boost"; // Sets stream
			SpriteBatch::DrawString(stream2.str().c_str(), _textPosition2, Color::Magenta); // Draws stream
		}

		if (_cherry->frameCount == 0) // If frame count is 0
		{
			// Draws Red Cherry
			SpriteBatch::Draw(_cherry->Texture, _cherry->Position, _cherry->SourceRect);

		}
		else // If frame count is not 0
		{
			// Draw Blue Cherry
			SpriteBatch::Draw(_cherry->Texture, _cherry->Position, _cherry->SourceRect);

		}

		stream4 << _menu->scoreAdd; // Sets stream
		if (_menu->scoreAddTimer < 100) // If score add timer is less than 100
		{
			_menu->scoreAddTimer++; // Increase timer
			SpriteBatch::DrawString(stream4.str().c_str(), _menu->scoreAddPosition, Color::Yellow); // Draw stream
		}

		stream5 << _menu->scoreAdd2; // Sets stream
		if (_menu->scoreAddTimer2 < 100) // If score add timer 2 is less than 100
		{
			_menu->scoreAddTimer2++; // Increases timer
			SpriteBatch::DrawString(stream5.str().c_str(), _menu->scoreAddPosition2, Color::Yellow); // Draws stream
		}




		if (_menu->paused) // IF paused
		{
			stringstream menuStream; // Creates stream 
			menuStream << "PAUSED!"; // Sets stream
			SpriteBatch::Draw(_menu->pauseBackground, _menu->Rectangle, nullptr); // Draws transparent image to dim the screen
			SpriteBatch::DrawString(menuStream.str().c_str(), _menu->stringPosition, Color::Red); // Draws stream
		}
	}
	else // If the menu screen is not playing
	{
		SpriteBatch::Draw(_menu->Background, _menu->Rectangle, nullptr); // Draws black image to cover screen
		SpriteBatch::Draw(_menu->thePacman, _menu->thePacmanPosition, _menu->thePacmanRect); // Draw Pacman

		if (_menu->thePacmanPosition->X > 1024 - 32) // If Pacman goes of the screen
			_menu->thePacmanPosition->X = -32; // Reset him to the left side of the screen

		_menu->thePacmanPosition->X += 0.1 * elapsedTime; // Increase pacman position

		// Changes source rect image depending on counter
		if (_menu->pacmanCounter == 40)
			_menu->pacmanCounter = 0;
		if (_menu->pacmanCounter >= 20 && _menu->pacmanCounter < 40)
			_menu->thePacmanRect->X = 32;
		if (_menu->pacmanCounter < 20)
			_menu->thePacmanRect->X = 0;
		_menu->pacmanCounter++; // Increase counter

		stringstream nameStream1, nameStream2, nameStream3, nameStream4, nameStream5; // Create streams 
		// Set streams as names
		nameStream1 << "Blinky"; 
		nameStream2 << "Sue";
		nameStream3 << "Inky";
		nameStream4 << "Clyde";
		nameStream5 << "Funky";

		for (int i = 0; i < GHOSTCOUNT; i++) // Loop through all ghosts
		{
			if (i == 0) // First ghost
			{
				SpriteBatch::DrawString(nameStream3.str().c_str(), _ghosts[i]->namePosition, Color::Blue); // Draws stream
				_ghosts[i]->name = "Inky"; // Sets ghost name
			}	
			if (i == 1) // Second ghost
			{
				SpriteBatch::DrawString(nameStream1.str().c_str(), _ghosts[i]->namePosition, Color::Red); // Draws stream
				_ghosts[i]->name = "Blinky"; // Sets ghost name
			}
				
			if (i == 2) // Third ghost
			{
				SpriteBatch::DrawString(nameStream2.str().c_str(), _ghosts[i]->namePosition, Color::Magenta); // Draws stream
				_ghosts[i]->name = "Sue"; // Sets ghost name
			}
				
			if (i == 3) // Fourth ghost
			{
				SpriteBatch::DrawString(nameStream4.str().c_str(), _ghosts[i]->namePosition, Color::Yellow); // Draws stream
				_ghosts[i]->name = "Clyde"; // Sets ghost name
			}
				
			if (i == 4) // Fifth ghost
			{
				SpriteBatch::DrawString(nameStream5.str().c_str(), _ghosts[i]->namePosition, Color::Green); // Draws stream
				_ghosts[i]->name = "Funky"; // Sets ghost name
			}
				
			SpriteBatch::Draw(_ghosts[i]->texture, _ghosts[i]->menuPosition, _ghosts[i]->menuSourceRect); // Draws ghosts

			if (_ghosts[i]->menuPosition->X > 1024 - 32) // If the ghost goes of the screen 
			{
				// Reset the name and ghost position to the left of the screen
				_ghosts[i]->namePosition->X = -32;
				_ghosts[i]->menuPosition->X = -32;
			}

			// Increase name and ghost position
			_ghosts[i]->menuPosition->X += 0.1 * elapsedTime;
			_ghosts[i]->namePosition->X += 0.1 * elapsedTime;
		}

		if (_menu->screen == "Main") // IF the screen is main
		{
			Input::MouseState* mouseState = Input::Mouse::GetState(); // Get mouse input
			if (mouseState->X >= 256 && mouseState->X <= 756 && mouseState->Y >= 154 && mouseState->Y <= 254) // If the mouse is hovering over a button
			{
				SpriteBatch::Draw(_menu->backStart, _menu->backStartPosition, _menu->backStartRect); // Draw an inner rect
				if (mouseState->LeftButton == Input::ButtonState::PRESSED) // If Left click
				{
					// Load map depending on what the user has picked
					if (_menu->map == 1)
						LoadMap("Maps/Map1.txt");
					if (_menu->map == 2)
						LoadMap("Maps/Map2.txt");
					if (_menu->map == 3)
						LoadMap("Maps/Map3.txt");

					_menu->screen = "Playing"; // Change screen to playing
				}

			}
				
			if (mouseState->X >= 256 && mouseState->X <= 756 && mouseState->Y >= 308 && mouseState->Y <= 408) // If the mouse is hovering over a button
			{
				SpriteBatch::Draw(_menu->backLevel, _menu->backLevelPosition, _menu->backLevelRect); // Draw inner rect
				if (mouseState->LeftButton == Input::ButtonState::PRESSED) // If left click
				{
					_menu->screen = "Level"; // Change the screen to level
				}
			}
		
			if (mouseState->X >= 256 && mouseState->X <= 756 && mouseState->Y >= 462 && mouseState->Y <= 562) // If the mouse is hovering over a button
			{
				SpriteBatch::Draw(_menu->backLead, _menu->backLeadPosition, _menu->backLeadRect); // Draw inner rect
				if (mouseState->LeftButton == Input::ButtonState::PRESSED) // If left click
				{
					_menu->leadState = "Main"; // Set lead state to main
					_menu->screen = "Leaderboard"; // Set screen to leaderboard
				}
			}
				

			SpriteBatch::DrawRectangle(new Rect(256, 154, 500, 100), Color::Black); // Draw rect
			SpriteBatch::DrawRectangle(new Rect(256, 308, 500, 100), Color::Black); // Draw rect
			SpriteBatch::DrawRectangle(new Rect(256, 466, 500, 100), Color::Black); // Draw rect
			SpriteBatch::Draw(_menu->Start, _menu->startPosition, _menu->startRect); // Draw start
			SpriteBatch::Draw(_menu->innerButton, _menu->innerButtonPosition1, _menu->innerButtonRect); // Draw button
			SpriteBatch::Draw(_menu->Level, _menu->levelPosition, _menu->levelRect); // Draw menu
			SpriteBatch::Draw(_menu->innerButton, _menu->innerButtonPosition2, _menu->innerButtonRect); // Draw button
			SpriteBatch::Draw(_menu->Leaderboard, _menu->leaderboardPosition, _menu->leaderboardRect); // Draw leaderboard
			SpriteBatch::Draw(_menu->innerButton, _menu->innerButtonPosition3, _menu->innerButtonRect); // Draw button

			// Changes title source rect image depedning on the counter
			if (_menu->titleCounter == 40)
				_menu->titleCounter = 0;
			if (_menu->titleCounter >= 20 && _menu->titleCounter < 40)
				SpriteBatch::Draw(_menu->pacmanOpen, _menu->pacmanPosition, _menu->pacmanRect);
			if (_menu->titleCounter < 20)
				SpriteBatch::Draw(_menu->pacmanClosed, _menu->pacmanPosition, _menu->pacmanRect);
			_menu->titleCounter++; // Increases counter
		}

		if (_menu->screen == "Level") // If the screen is level
		{
			// Draws another rect around the button depending on selected level
			if (_menu->map == 1)
				SpriteBatch::Draw(_menu->backStart, _menu->backStartPosition, _menu->backStartRect);
			if (_menu->map == 2)
				SpriteBatch::Draw(_menu->backLevel, _menu->backLevelPosition, _menu->backLevelRect);
			if (_menu->map == 3)
				SpriteBatch::Draw(_menu->backLead, _menu->backLeadPosition, _menu->backLeadRect);

			// Same code as used in main menu just slightly adapted
			Input::MouseState* mouseState = Input::Mouse::GetState();
			if (mouseState->X >= 256 && mouseState->X <= 756 && mouseState->Y >= 14 && mouseState->Y <= 114)
			{
				SpriteBatch::Draw(_menu->backLead, _menu->backBackPosition, _menu->backLeadRect);
				if(mouseState->LeftButton == Input::ButtonState::PRESSED)
				{
					_menu->screen = "Main";
				}
			}

			if (mouseState->X >= 256 && mouseState->X <= 756 && mouseState->Y >= 154 && mouseState->Y <= 254)
			{
				SpriteBatch::Draw(_menu->backStart, _menu->backStartPosition, _menu->backStartRect);
				if (mouseState->LeftButton == Input::ButtonState::PRESSED)
				{
					_menu->map = 1;
				}
			}
			if (mouseState->X >= 256 && mouseState->X <= 756 && mouseState->Y >= 308 && mouseState->Y <= 408)
			{
				SpriteBatch::Draw(_menu->backLevel, _menu->backLevelPosition, _menu->backLevelRect);
				if (mouseState->LeftButton == Input::ButtonState::PRESSED)
				{
					_menu->map = 2;
				}
			}
			if (mouseState->X >= 256 && mouseState->X <= 756 && mouseState->Y >= 462 && mouseState->Y <= 562)
			{
				SpriteBatch::Draw(_menu->backLead, _menu->backLeadPosition, _menu->backLeadRect);
				if (mouseState->LeftButton == Input::ButtonState::PRESSED)
				{
					_menu->map = 3;
				}
			}
				
			SpriteBatch::DrawRectangle(new Rect(256, 14, 500, 100), Color::Black);
			SpriteBatch::DrawRectangle(new Rect(256, 154, 500, 100), Color::Black);
			SpriteBatch::DrawRectangle(new Rect(256, 308, 500, 100), Color::Black);
			SpriteBatch::DrawRectangle(new Rect(256, 466, 500, 100), Color::Black);
			SpriteBatch::Draw(_menu->back, _menu->backPosition, _menu->levelNRect);
			SpriteBatch::Draw(_menu->innerButton, _menu->innerButtonPosition4, _menu->innerButtonRect);
			SpriteBatch::Draw(_menu->level1, _menu->level1Position, _menu->levelNRect);
			SpriteBatch::Draw(_menu->innerButton, _menu->innerButtonPosition1, _menu->innerButtonRect);
			SpriteBatch::Draw(_menu->level2, _menu->level2Position, _menu->levelNRect);
			SpriteBatch::Draw(_menu->innerButton, _menu->innerButtonPosition2, _menu->innerButtonRect);
			SpriteBatch::Draw(_menu->level3, _menu->level3Position, _menu->levelNRect);
			SpriteBatch::Draw(_menu->innerButton, _menu->innerButtonPosition3, _menu->innerButtonRect);
		}
		if (_menu->screen == "Game Over") // If screen is Game over
		{
			if (!_menu->leadSorted) // If lead sorted is false
			{
				int score;
				int counter = 0;
				ifstream infile("Leaderboard.txt"); // Opens file
				if (!infile) // If file cannot be opened
				{
					cerr << "File could not be opened!";
					exit(1);
				}
				else // If file is opened
				{
					string line;
					string names[10] = {}; // Creates array
					int values[10] = {}; // Creates array
					while (getline(infile, line)) // Loops through everything in file
					{
						string a;
						a = line;
						int posOfSpace = 0;
						for (int i = 0; i < a.length(); i++) // Loops through everything in the line
						{
							if (a[i] == ' ') // If there is a gap
							{
								posOfSpace = i; // Set pos of space to i
							}
						}
						if (counter == 10) // If counter is 10
							break; // Break loop
						names[counter] = line.substr(0, posOfSpace); // Sets name
						values[counter] = stoi(line.substr(posOfSpace + 1, line.length())); // Sets value
						counter++; // Increases counter
					}
					infile.close(); // Closes file
					int indexSmallestNum = indexofSmallestElement(values, 10); // Sets smallest number index
					if (values[9] < _menu->playerScore) //If player score is higher than the lowest score
					{
						names[indexSmallestNum] = _lead->playerName; // Adds name to array
						values[indexSmallestNum] = _menu->playerScore; // Adds score to array
					}
					int n = sizeof(values) / sizeof(values[0]); // Sets size
					int temp; 
					string temp2;

					// Loops through all values in values and orders them and also orders the names to correspond
					for (int i = 0; i < n; i++)
					{
						for (int j = i + 1; j < n; j++)
						{
							if (values[i] > values[j])
							{
								temp = values[i];
								temp2 = names[i];
								values[i] = values[j];
								names[i] = names[j];
								values[j] = temp;
								names[j] = temp2;
							}
						}
					}
					ofstream outFile("Leaderboard.txt", ios::trunc); // Opens file as trunc so clears it
					for (int i = 9; i > -1; i--) // Loops through array
					{
						outFile << names[i] << " " << values[i] << endl; // Adds names and values to file
					}
					outFile.close(); // Closes file
					_menu->leadSorted = true; // Sets sorted to true
				}
			}
			
			// Same code as main menu jsut slightly adapted
			Input::MouseState* mouseState = Input::Mouse::GetState();
			if (mouseState->X >= 256 && mouseState->X <= 756 && mouseState->Y >= 14 && mouseState->Y <= 114)
			{
				SpriteBatch::Draw(_menu->backLead, _menu->backBackPosition, _menu->backLeadRect);
				if (mouseState->LeftButton == Input::ButtonState::PRESSED)
				{
					exit(0);
				}
			}
			if (mouseState->X >= 256 && mouseState->X <= 756 && mouseState->Y >= 154 && mouseState->Y <= 254)
			{
				SpriteBatch::Draw(_menu->backStart, _menu->backStartPosition, _menu->backStartRect);
				if (mouseState->LeftButton == Input::ButtonState::PRESSED)
				{
					_menu->leadState = "Exit";
					_menu->screen = "Leaderboard";
				}
			}
			SpriteBatch::DrawRectangle(new Rect(256, 14, 500, 100), Color::Black);
			SpriteBatch::DrawRectangle(new Rect(256, 154, 500, 100), Color::Black);
			SpriteBatch::Draw(_menu->exit, _menu->backPosition, _menu->levelNRect);
			SpriteBatch::Draw(_menu->innerButton, _menu->innerButtonPosition4, _menu->innerButtonRect);
			SpriteBatch::Draw(_menu->Leaderboard, _menu->level1Position, _menu->levelNRect);
			SpriteBatch::Draw(_menu->innerButton, _menu->innerButtonPosition1, _menu->innerButtonRect);
			std::stringstream stream1, stream2;
			stream1 << "You died!"; 
			stream2 << "You scored: " << _menu->playerScore;
			SpriteBatch::DrawString(stream1.str().c_str(), _menu->stringPosition, Color::Yellow);
			SpriteBatch::DrawString(stream2.str().c_str(), _menu->stringPosition2, Color::Yellow);
		}
		if (_menu->screen == "Leaderboard") // If screen is leaderboard
		{
			// Same code as main menu jsut slightly adapted
			Input::MouseState* mouseState = Input::Mouse::GetState();
			if (mouseState->X >= 256 && mouseState->X <= 756 && mouseState->Y >= 14 && mouseState->Y <= 114)
			{
				SpriteBatch::Draw(_menu->backLead, _menu->backBackPosition, _menu->backLeadRect);
				if (mouseState->LeftButton == Input::ButtonState::PRESSED)
				{
					if (_menu->leadState == "Main")
						_menu->screen = "Main";
					if (_menu->leadState == "Exit")
						exit(0);
				}
			}
			SpriteBatch::DrawRectangle(new Rect(256, 14, 500, 100), Color::Black);
			if (_menu->leadState == "Main")
				SpriteBatch::Draw(_menu->back, _menu->backPosition, _menu->levelNRect);
			if (_menu->leadState == "Exit")
				SpriteBatch::Draw(_menu->exit, _menu->backPosition, _menu->levelNRect);	
			SpriteBatch::Draw(_menu->innerButton, _menu->innerButtonPosition4, _menu->innerButtonRect);

			std::stringstream leaderboard;
			vector <string> scores; // Creates vector

			ifstream infile("Leaderboard.txt"); // Open file
			if (!infile) // If file cannot be opened
			{
				cerr << "File could not be opened!";
				exit(1);
			}
			else // If file can be opened
			{
				infile.seekg(0, ios::end);
				int len = infile.tellg(); // Finds length 
				if (len == 0) // If empty
				{
					scores.push_back("There are no scores");
				}
				else // If not empty
				{
					infile.seekg(0, ios::beg);
					string line;
					while (getline(infile, line)) // Loops through all lines
					{
						scores.push_back(line); // Adds line to vector
					}
				}
			}
			// Sets stream as vector elements
			leaderboard << scores[0] << ' ' << endl << endl <<  scores[1] << ' ' << endl << endl <<  scores[2] << ' ' << endl << endl <<  scores[3] << ' ' << endl << endl <<  scores[4] << ' ' << endl << endl <<  scores[5] << ' ' << endl << endl <<  scores[6] << ' ' << endl << endl <<  scores[7] << ' ' << endl << endl <<  scores[8] << ' ' << endl << endl <<  scores[9];
			SpriteBatch::DrawString(leaderboard.str().c_str(), _menu->leadPosition, Color::Yellow); // Draws stream
			infile.close(); // Closes file
		}

		

	}
	
	SpriteBatch::EndDraw(); // Ends Drawing
}

