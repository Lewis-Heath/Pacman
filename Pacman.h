#pragma once

// If Windows and not in Debug, this will run without a console window
// You can use this to output information when debugging using cout or cerr
#ifdef WIN32 
	#ifndef _DEBUG
		#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
	#endif
#endif

// Just need to include main header file
#include "S2D/S2D.h"

// Define amount of muchies
#define MUNCHIECOUNT 768

// Define the amount of ghosts
#define GHOSTCOUNT 5

// Define the amount of walls
#define WALLCOUNT 500

// Reduces the amount of typing by including all classes in S2D namespace
using namespace S2D;

// Structure defentition
struct  Player
{
	// Data to represent Pacman
	Vector2* Position;
	Rect* SourceRect;
	Texture2D* Texture;
	bool Moving;
	int speedBoostTimer;
	float speed;
	float speedMultiplier;
	int Direction;
	int Frame;
	int CurrentFrameTime;
	int FrameTime;
	int speedBoostCooldownTimer;
	bool speedBoost;
	int barPerc;
	bool recharging;
	int cooldownCount;
	bool dead;
	Rect* HitRect;
	int tileMapX;
	int tileMapY;
	bool soundPlaying;
	int soundCounter;
};


struct Wall
{
	Vector2* Position;
	Rect* sourceRect;
	Texture2D* Texture;
	bool Gate;
};

struct Enemy
{
	// Data to represent Munchie
	int frameCount;
	Rect* Recta;
	Rect* SourceRect;
	Vector2* Position;
	Texture2D* Texture;
	int FrameTime;
	int CurrentFrameTime;
	bool dead;
	string type;
	int lifetime;
	int scoreProp;
	bool gotPosition;
};

struct Menu
{
	// Data for Menu
	Texture2D* Background;
	Texture2D* Start;
	Texture2D* Level;
	Texture2D* Leaderboard;
	Texture2D* pacmanOpen;
	Texture2D* pacmanClosed;
	Texture2D* backStart;
	Texture2D* backLevel;
	Texture2D* backLead;
	Texture2D* innerButton;
	Texture2D* thePacman;
	Texture2D* level1;
	Texture2D* level2;
	Texture2D* level3;
	Texture2D* back;
	Texture2D* pauseBackground;
	Texture2D* ghostBlue;
	Texture2D* ghostRed;
	Texture2D* ghostOrange;
	Texture2D* ghostGreen;
	Texture2D* ghostPurple;
	Texture2D* exit;
	Vector2* thePacmanPosition;
	Vector2* stringPosition;
	Vector2* stringPosition2;
	Vector2* startPosition;
	Vector2* levelPosition;
	Vector2* backPosition;
	Vector2* backBackPosition;
	Vector2* leaderboardPosition;
	Vector2* pacmanPosition;
	Vector2* backStartPosition;
	Vector2* backLevelPosition;
	Vector2* backLeadPosition;
	Vector2* innerButtonPosition1;
	Vector2* innerButtonPosition2;
	Vector2* innerButtonPosition3;
	Vector2* innerButtonPosition4;
	Vector2* level1Position;
	Vector2* level2Position;
	Vector2* level3Position;
	Vector2* scoreAddPosition;
	Vector2* scoreAddPosition2;
	Vector2* leadPosition;
	Rect* Rectangle;
	Rect* pacmanRect;
	Rect* thePacmanRect;
	Rect* startRect;
	Rect* levelRect;
	Rect* leaderboardRect;
	Rect* backStartRect;
	Rect* backLevelRect;
	Rect* backLeadRect;
	Rect* innerButtonRect;
	Rect* levelNRect;
	bool paused;
	bool pKeyDown;
	bool started;
	int wallCounter;
	int munchieCounter;
	int maxMunchieCounter;
	int ghostCounter;
	int titleCounter;
	string tileMap[24][32];
	string screen;
	int map;
	int pacmanCounter;
	bool ghostFright;
	int ghostFrightTimer;
	Texture2D* frightTexture;
	bool startPlayed;
	int playerScore;
	int ghostMultiplier;
	int scoreAdd;
	int scoreAdd2;
	int scoreAddTimer;
	int scoreAddTimer2;
	bool showingScore;
	bool addedOnce;
	bool leadSorted;
	string leadState;
	int musicCounter;
	bool musicPlaying;
	bool firstLoop;
};

struct Leaderboard
{
	string playerName;
};


struct MovingEnemy
{
	// Data for moving enemy
	Vector2* position;
	Texture2D* texture;
	Rect* sourceRect;
	int direction;
	float speed;
	bool moveUp;
	bool moveDown;
	bool moveLeft;
	bool moveRight;
	int moveCounter;
	bool gotDirection;
	int tileMapX;
	int tileMapY;
	int previousDirection;
	bool upOrDown;
	bool leftOrRight;
	Vector2* menuPosition;
	Rect* menuSourceRect;
	Vector2* namePosition;
	Vector2* spawnPosition;
	string name;
	bool destructable;
	int frightCounter;
	int frightCounter2;
	int frightCounter3;
	int randomCounter;
};

// Declares the Pacman class which inherits from the Game class.
// This allows us to overload the Game class methods to help us
// load content, draw and update our game.
class Pacman : public Game
{
private:

	Player* _pacman;
	Enemy* _munchies[MUNCHIECOUNT];
	MovingEnemy* _ghosts[GHOSTCOUNT];
	Wall* _walls[WALLCOUNT];
	Enemy* _cherry;
	Menu* _menu;
	Leaderboard* _lead;

	SoundEffect* _pop;
	SoundEffect* _coin;
	SoundEffect* _speed;
	SoundEffect* _inter;
	SoundEffect* _death;
	SoundEffect* _ghostdeath;
	SoundEffect* _pop2;
	SoundEffect* _music;
	
	void Input(int elapsedTime, Input::KeyboardState* state, Input::MouseState*mouseState);
	void CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey);
	void CheckViewportCollision();
	void CheckViewportCollision2(MovingEnemy*);
	void UpdatePacman(int elapsedTime);
	void UpdateMunchie(Enemy* munchie, int elapsedTime);
	void UpdateCherry(Enemy* cherry, int elapsedTime);
	void UpdatePerc(int length, int barLength);
	void CheckGhostCollisions();
	void UpdateGhost(MovingEnemy*, int elapsedTime, string movementType);
	void CheckMunchieCollision();
	void LoadMap(string nameOfMap);
	void CheckWallCollision(string direction);
	void CheckWallCollision2(string direction, MovingEnemy*);
	void displayMap(Menu* menu);
	int pickDirection(string type, MovingEnemy*);
	void updateGhostImage(MovingEnemy*);
	void resetGhostsTexture(MovingEnemy*);
	int indexofSmallestElement(int array[], int size);
	void CheckCherryCollision();
	void UpdateWalls();


	// Position for String
	Vector2* _stringPosition;
	Vector2* _textPosition1;
	Vector2* _textPosition2;
	Vector2* _textPosition3;

public:
	/// <summary> Constructs the Pacman class. </summary>
	Pacman(int argc, char* argv[]);

	/// <summary> Destroys any data associated with Pacman class. </summary>
	virtual ~Pacman();

	/// <summary> All content should be loaded in this method. </summary>
	void virtual LoadContent();

	/// <summary> Called every frame - update game logic here. </summary>
	void virtual Update(int elapsedTime);

	/// <summary> Called every frame - draw game here. </summary>
	void virtual Draw(int elapsedTime);
};