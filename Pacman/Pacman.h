#pragma once

// If Windows and not in Debug, this will run without a console window
// You can use this to output information when debugging using cout or cerr
#ifdef WIN32 
	#ifndef _DEBUG
		#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
	#endif
#endif

// States number of Munchies that should be onscreen
#define MUNCHIECOUNT 50
#define CHERRYCOUNT 10
#define GHOSTCOUNT 4

// Just need to include main header file
#include "S2D/S2D.h"

// Reduces the amount of typing by including all classes in S2D namespace
using namespace S2D;

// Structure to represent Pacman
struct Player
{
	bool dead;
	int direction;
	int frame;
	int currentFrameTime;
	float speedMultiplier;
	Vector2* position;
	Rect* sourceRect;
	Texture2D* texture;
};

// Structure to represent Munchies
struct Enemy
{
	int frameCount;
	int currentFrameTime;
	Vector2* position;
	Rect* rect;
	Texture2D* texture;
};

// Structure to represent Ghosts
struct MovingEnemy
{
	Vector2* position;
	Texture2D* texture;
	Rect* sourceRect;
	int direction;
	int currentFrameTime;
	float speed;
};

// Declares the Pacman class which inherits from the Game class.
// This allows us to overload the Game class methods to help us
// load content, draw and update our game.
class Pacman : public Game
{
private:
	// Prototypes
	void Input(int elapsedTime, Input::KeyboardState* state);
	void CheckStart(Input::MouseState* mouseState);
	void CheckPaused(Input::KeyboardState *state, Input::Keys pauseKey);
	void CheckOptionSelect(int mouseX, int mouseY);
	void CheckViewportCollision();
	void CheckGhostCollisions();
	void CheckMunchieCollisions();
	void CheckCherryCollisions();
	void UpdatePacman(int elapsedTime);
	void UpdateMunchie(Enemy* munchie, int elapsedTime);
	void UpdateGhost(MovingEnemy* ghost, int elapsedTime, int type);
	void LoadHighScores();

	// Data to represent Pacman
	Player* _pacman;

	// Data to represent Munchie
	Enemy* _munchies[MUNCHIECOUNT];

	// Data to represent Cherries
	Enemy* _cherries[CHERRYCOUNT];

	// Data to represent Ghosts
	MovingEnemy* _ghosts[GHOSTCOUNT];

	// Score
	int _score;
	bool _win;
	Vector2* _scoreStringPosition;

	// Sounds
	SoundEffect* _pop1;
	SoundEffect* _pop2;
	SoundEffect* _gameOver;
	SoundEffect* _victory;

	// Constant data for Game Variables
	const float _cPacmanSpeed;
	const int _cPacmanFrameTime;
	const int _cMunchieFrameTime;
	const int _cGhostFrameTime;

	// Data for Menu
	Texture2D* _menuBackground;
	Rect* _menuRectangle;
	Vector2* _menuStringPosition;
	Rect* _menuOptions[3];
	int _menuSelect;
	bool _paused;
	bool _pKeyDown;

	// Data for Start Menu
	Texture2D* _startBackground;
	Rect* _startRectangle;
	Vector2* _startStringPosition;
	bool _start;

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