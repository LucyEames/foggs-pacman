#include "Pacman.h"
#include <sstream>

Pacman::Pacman(int argc, char* argv[]) : Game(argc, argv), _cPacmanSpeed(0.1f), _cPacmanFrameTime(125), _cMunchieFrameTime(500), _cGhostFrameTime(1000)
{
	// Initialise variables
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		_munchies[i] = new Enemy();
		_munchies[i]->currentFrameTime = 0;
	}
	for (int i = 0; i < CHERRYCOUNT; i++)
	{
		_cherries[i] = new Enemy();
		_cherries[i]->currentFrameTime = 0;
	}
	for (int i = 0; i < GHOSTCOUNT; i++)
	{
		_ghosts[i] = new MovingEnemy();
		_ghosts[i]->direction = i;
		_ghosts[i]->speed = 0.18f;
	}

	_pacman = new Player();
	_pacman->direction = 0;
	_pacman->currentFrameTime = 0;
	_pacman->frame = 0;
	_pacman->speedMultiplier = 1.0f;
	_pacman->dead = false;

	_paused = false;
	_pKeyDown = false;
	_start = true;

	_score = 0;

	_pop1 = new SoundEffect();
	_pop2 = new SoundEffect();
	_gameOver = new SoundEffect();
	_victory = new SoundEffect();

	// Initialise important Game aspects
	Audio::Initialise();
	Graphics::Initialise(argc, argv, this, 1024, 768, false, 25, 25, "Pacman", 60);
	Input::Initialise();

	// Start the Game Loop - This calls Update and Draw in game loop
	Graphics::StartGameLoop();
}

Pacman::~Pacman()
{
	delete _pacman->texture;
	delete _pacman->sourceRect;
	delete _pacman->position;
	delete _munchies[0]->texture;
	delete _cherries[0]->texture;
	delete _ghosts[0]->texture;
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		delete _munchies[i]->rect;
		delete _munchies[i]->position;
	}
	for (int i = 0; i < CHERRYCOUNT; i++)
	{
		delete _cherries[i]->rect;
		delete _cherries[i]->position;
	}
	for (int i = 0; i < GHOSTCOUNT; i++)
	{
		delete _ghosts[i]->position;
		delete _ghosts[i]->sourceRect;
	}
	delete _menuBackground;
	delete _menuRectangle;
	delete _menuStringPosition;
	delete _startBackground;
	delete _startRectangle;
	delete _startStringPosition;
	delete _pop1;
	delete _pop2;
	delete _gameOver;
	delete _victory;
	delete _pacman;
	delete _munchies;
	delete _cherries;
	delete _ghosts;
}

void Pacman::LoadContent()
{
	// Load Pacman
	_pacman->texture = new Texture2D();
	_pacman->texture->Load("Textures/Pacman.png", false);
	_pacman->position = new Vector2(350.0f, 350.0f);
	_pacman->sourceRect = new Rect(0.0f, 0.0f, 32, 32);

	// Load Munchie
	Texture2D* munchieTex = new Texture2D();
	munchieTex->Load("Textures/Munchie.png", false);

	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		_munchies[i]->texture = munchieTex;
		_munchies[i]->position = new Vector2(rand() % Graphics::GetViewportWidth(), rand() % Graphics::GetViewportHeight());
		_munchies[i]->rect = new Rect(0, 0, 12, 12);
	}

	// Load Cherry
	Texture2D* cherryTex = new Texture2D();
	cherryTex->Load("Textures/Cherry.png", false);

	for (int i = 0; i < CHERRYCOUNT; i++)
	{
		_cherries[i]->texture = cherryTex;
		_cherries[i]->position = new Vector2(rand() % Graphics::GetViewportWidth(), rand() % Graphics::GetViewportHeight());
		_cherries[i]->rect = new Rect(0, 0, 32, 32);
	}

	// Load Ghosts
	Texture2D* ghostTex = new Texture2D();
	ghostTex->Load("Textures/Ghosts.png", false);

	for (int i = 0; i < GHOSTCOUNT; i++)
	{
		_ghosts[i]->texture = ghostTex;
		_ghosts[i]->position = new Vector2(rand() % Graphics::GetViewportWidth(), rand() % Graphics::GetViewportHeight());
		_ghosts[i]->sourceRect = new Rect(20.0f * i, 20.0f * i, 20, 20);
	}

	// Score
	_scoreStringPosition = new Vector2(10.0f, 25.0f);

	// Set Sounds
	_pop1->Load("Sounds/pop1.wav");
	_pop2->Load("Sounds/pop2.wav");
	_gameOver->Load("Sounds/gameover.wav");
	_victory->Load("Sounds/victory.wav");

	// Set Menu Parameters
	_menuBackground = new Texture2D();
	_menuBackground->Load("Textures/Transparency.png", false);
	_menuRectangle = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());
	_menuStringPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f, Graphics::GetViewportHeight() / 2.0f);
	for (int i = 0; i < 3; i++)
	{
		_menuOptions[i] = new Rect(390.0f, 23.0f * i + 339.0f, 15, 15);
	}
	_menuSelect = 0;

	// Set Start Parameters
	_startBackground = new Texture2D();
	_startBackground->Load("Textures/Transparency.png", false);
	_startRectangle = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());
	_startStringPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f - 100.0f, Graphics::GetViewportHeight() / 4.0f);
}

void Pacman::Update(int elapsedTime)
{
	// Gets the current state of the keyboard
	Input::KeyboardState* keyboardState = Input::Keyboard::GetState();

	// Gets the current state of the mouse
	Input::MouseState* mouseState = Input::Mouse::GetState();

	CheckStart(mouseState);
	if (!_start)
	{
		CheckPaused(keyboardState, Input::Keys::P);
		if (!_paused && !_pacman->dead && !_win)
		{
			Input(elapsedTime, keyboardState);

			// Animations
			UpdatePacman(elapsedTime);
			for (int i = 0; i < MUNCHIECOUNT; i++)
			{
				UpdateMunchie(_munchies[i], elapsedTime);
			}
			for (int i = 0; i < CHERRYCOUNT; i++)
			{
				UpdateMunchie(_cherries[i], elapsedTime);
			}
			for (int i = 0; i < GHOSTCOUNT; i++)
			{
				UpdateGhost(_ghosts[i], elapsedTime, i);
			}
			
			// Checks Collisions
			CheckGhostCollisions();
			CheckMunchieCollisions();
			CheckCherryCollisions();
			CheckViewportCollision();
		}
	}
}

void Pacman::Draw(int elapsedTime)
{
	// Allows us to easily create a string
	std::stringstream scoreStream;
	scoreStream << "SCORE: " << _score;

	// Starts Drawing
	SpriteBatch::BeginDraw();

	// Main Menu
	if (_start)
	{
		SpriteBatch::Draw(_startBackground, _startRectangle, nullptr);
		if (_menuSelect == 0)
		{
			std::stringstream startStream;
			startStream << "PACMAN\n\n\n\n\n\n\nPLAY\nCONTROLS\nEXIT";
			SpriteBatch::DrawString(startStream.str().c_str(), _startStringPosition, Color::Yellow);
			for (int i = 0; i < 3; i++)
				SpriteBatch::DrawRectangle(_menuOptions[i], Color::Red);
		}
		else if (_menuSelect == 1)
		{
			_start = false;
		}
		else if (_menuSelect == 2)
		{
			std::stringstream controls;
			controls << "CONTROLS:\n\nWASD to Move\nSHIFT to Sprint\nP to Pause\n\n\nCollect all the munchies and cherries to win.\nAvoid ghosts at all costs.\n\n\nClick anywhere to return to menu.";
			SpriteBatch::DrawString(controls.str().c_str(), _startStringPosition, Color::Yellow);
		}
		else if (_menuSelect == 3) 
		{
			Graphics::Destroy();
		}
	}
	// Rest of the game
	else
	{
		// Draws Cherry
		for (int i = 0; i < CHERRYCOUNT; i++)
		{
			SpriteBatch::Draw(_cherries[i]->texture, _cherries[i]->position, _cherries[i]->rect, Vector2::Zero, 1.0f, 0.0f, Color::White, SpriteEffect::NONE);
		}
		// Draws Munchie
		for (int i = 0; i < MUNCHIECOUNT; i++)
		{
			SpriteBatch::Draw(_munchies[i]->texture, _munchies[i]->position, _munchies[i]->rect, Vector2::Zero, 1.0f, 0.0f, Color::White, SpriteEffect::NONE);
		}
		// Draws Ghost
		for (int i = 0; i < GHOSTCOUNT; i++)
		{
			SpriteBatch::Draw(_ghosts[i]->texture, _ghosts[i]->position, _ghosts[i]->sourceRect);
		}

		// Draws Pacman
		if (!_pacman->dead)
		{
			SpriteBatch::Draw(_pacman->texture, _pacman->position, _pacman->sourceRect);
		}

		// Draws Pause Menu
		if (_paused)
		{
			std::stringstream menuStream;
			menuStream << "PAUSED!";

			SpriteBatch::Draw(_menuBackground, _menuRectangle, nullptr);
			SpriteBatch::DrawString(menuStream.str().c_str(), _menuStringPosition, Color::Red);
		}
		// Draws Game Over
		if (_pacman->dead)
		{
			std::stringstream deadStream;
			deadStream << "GAME OVER";
			SpriteBatch::DrawString(deadStream.str().c_str(), _menuStringPosition, Color::Red);
		}
		// Draws Win
		if (_win)
		{
			std::stringstream winStream;
			winStream << "YOU WIN";
			SpriteBatch::DrawString(winStream.str().c_str(), _menuStringPosition, Color::Green);
		}

		// Draws Score
		SpriteBatch::DrawString(scoreStream.str().c_str(), _scoreStringPosition, Color::Green);
	}
	// Ends Drawing
	SpriteBatch::EndDraw(); 
}

void Pacman::Input(int elapsedTime, Input::KeyboardState* state)
{
	float pacmanSpeed = _cPacmanSpeed * elapsedTime * _pacman->speedMultiplier;

	// Checks if D key is pressed
	if (state->IsKeyDown(Input::Keys::D))
	{
		//Moves Pacman across X axis
		_pacman->position->X += pacmanSpeed;
		_pacman->direction = 0;
	}
	// Checks if A key is pressed
	else if (state->IsKeyDown(Input::Keys::A))
	{
		//Moves Pacman across X axis
		_pacman->position->X -= pacmanSpeed;
		_pacman->direction = 2;
	}
	// Checks if W key is pressed
	else if (state->IsKeyDown(Input::Keys::W))
	{
		//Moves Pacman across Y axis
		_pacman->position->Y -= pacmanSpeed;
		_pacman->direction = 3;
	}
	// Checks if S key is pressed
	else if (state->IsKeyDown(Input::Keys::S))
	{
		//Moves Pacman across Y axis
		_pacman->position->Y += pacmanSpeed;
		_pacman->direction = 1;
	}

	// Speed Multiplier
	if (state->IsKeyDown(Input::Keys::LEFTSHIFT))
	{
		// Apply multiplier
		_pacman->speedMultiplier = 2.0f;
	}
	else
	{
		// Reset multiplier
		_pacman->speedMultiplier = 1.0f;
	}
}

void Pacman::CheckStart(Input::MouseState* mouseState)
{
	// Checks if Mouse is pressed
	if (mouseState->LeftButton == Input::ButtonState::PRESSED)
	{
		CheckOptionSelect(mouseState->X, mouseState->Y);
	}
}

void Pacman::CheckPaused(Input::KeyboardState *state, Input::Keys pauseKey)
{
	// Checks if Pause key is pressed
	if (state->IsKeyDown(pauseKey) && !_pKeyDown)
	{
		_pKeyDown = true;
		_paused = !_paused;
	}
	if (state->IsKeyUp(pauseKey))
		_pKeyDown = false;
}

void Pacman::CheckOptionSelect(int mouseX, int mouseY)
{
	// Checks which main menu option was selected
	_menuSelect = 0;
	for (int i = 0; i < 3; i++)
		if (mouseX >= _menuOptions[i]->X && mouseY >= _menuOptions[i]->Y && mouseX < _menuOptions[i]->X + 15 && mouseY < _menuOptions[i]->Y + 15)
		{
			_menuSelect = i + 1;
		}
}

void Pacman::CheckViewportCollision()
{
	// Checks if Pacman is trying to disappear
	// Right wall
	if (_pacman->position->X > Graphics::GetViewportWidth())
		_pacman->position->X = 0.0f - _pacman->sourceRect->Width;
	
	// Left wall
	if (_pacman->position->X + _pacman->sourceRect->Width < 0)
		_pacman->position->X = Graphics::GetViewportWidth();
	
	// Top wall
	if (_pacman->position->Y + _pacman->sourceRect->Height < 0)
		_pacman->position->Y = Graphics::GetViewportHeight();

	// Bottom wall
	if (_pacman->position->Y > Graphics::GetViewportHeight())
		_pacman->position->Y = 0.0f - _pacman->sourceRect->Height;
}

void Pacman::CheckMunchieCollisions()
{
	// Local Variables
	int bottom1 = _pacman->position->Y + _pacman->sourceRect->Height;
	int bottom2 = 0;
	int left1 = _pacman->position->X;
	int left2 = 0;
	int right1 = _pacman->position->X + _pacman->sourceRect->Width;
	int right2 = 0;
	int top1 = _pacman->position->Y;
	int top2 = 0;

	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		// Populate variables with Munchie data
		bottom2 = _munchies[i]->position->Y + _munchies[i]->rect->Height;
		left2 = _munchies[i]->position->X;
		right2 = _munchies[i]->position->X + _munchies[i]->rect->Width;
		top2 = _munchies[i]->position->Y;

		// If collision, move it, add to score, and play audio
		if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2))
		{
			_score += 100;
			_munchies[i]->position->X = 2000.0f;
			Audio::Play(_pop1);
			if (_score >= 10000)
			{
				_win = true;
				Audio::Play(_victory);
			}
		}
	}
}

void Pacman::CheckCherryCollisions()
{
	// Local Variables
	int bottom1 = _pacman->position->Y + _pacman->sourceRect->Height;
	int bottom2 = 0;
	int left1 = _pacman->position->X;
	int left2 = 0;
	int right1 = _pacman->position->X + _pacman->sourceRect->Width;
	int right2 = 0;
	int top1 = _pacman->position->Y;
	int top2 = 0;
	
	for (int i = 0; i < CHERRYCOUNT; i++)
	{
		// Populate variables with Cherry data
		bottom2 = _cherries[i]->position->Y + _cherries[i]->rect->Height;
		left2 = _cherries[i]->position->X;
		right2 = _cherries[i]->position->X + _cherries[i]->rect->Width;
		top2 = _cherries[i]->position->Y;

		// If collision, move it, add to score, and play audio
		if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2))
		{
			_score += 500;
			_cherries[i]->position->X = 2000.0f;
			Audio::Play(_pop2);
			if (_score >= 10000)
			{
				_win = true;
				Audio::Play(_victory);
			}
		}
	}
}

void Pacman::CheckGhostCollisions()
{
	// Local Variables
	int bottom1 = _pacman->position->Y + _pacman->sourceRect->Height;
	int bottom2 = 0;
	int left1 = _pacman->position->X;
	int left2 = 0;
	int right1 = _pacman->position->X + _pacman->sourceRect->Width;
	int right2 = 0;
	int top1 = _pacman->position->Y;
	int top2 = 0;

	for (int i = 0; i < GHOSTCOUNT; i++)
	{
		// Populate variables with Ghost data
		bottom2 = _ghosts[i]->position->Y + _ghosts[i]->sourceRect->Height;
		left2 = _ghosts[i]->position->X;
		right2 = _ghosts[i]->position->X + _ghosts[i]->sourceRect->Width;
		top2 = _ghosts[i]->position->Y;

		if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2))
		{
			_pacman->dead = true;
			Audio::Play(_gameOver);
			i = GHOSTCOUNT;
		}
	}
}

void Pacman::UpdatePacman(int elapsedTime)
{
	// Animates Pacman
	_pacman->currentFrameTime += elapsedTime;
	if (_pacman->currentFrameTime > _cPacmanFrameTime)
	{
		_pacman->frame++;
		if (_pacman->frame >= 4)
			_pacman->frame = 0;
		_pacman->currentFrameTime = 0;
	}
	_pacman->sourceRect->X = _pacman->sourceRect->Width * _pacman->frame;
	_pacman->sourceRect->Y = _pacman->sourceRect->Height * _pacman->direction;
}

void Pacman::UpdateMunchie(Enemy* munchie, int elapsedTime)
{
	// Animates Munchies
	munchie->currentFrameTime += elapsedTime;
	if (munchie->currentFrameTime > _cMunchieFrameTime)
	{
		munchie->frameCount++;
		if (munchie->frameCount >= 4)
			munchie->frameCount = 0;
		munchie->currentFrameTime = 0;
	}
	munchie->rect->X = munchie->rect->Width * munchie->frameCount;
}

void Pacman::UpdateGhost(MovingEnemy* ghost, int elapsedTime, int type)
{
	switch (type)
	{
	// Blue Ghost (Inky)
	case 0:
		// Moves Right
		if (ghost->direction == 0) 
		{
			ghost->position->X += ghost->speed * elapsedTime;
		}
		// Moves Left
		else if (ghost->direction == 1) 
		{
			ghost->position->X -= ghost->speed * elapsedTime;
		}
		// If it hits Right edge, change direction
		if (ghost->position->X + ghost->sourceRect->Width >= Graphics::GetViewportWidth()) 
		{
			ghost->direction = 1;
		}
		// If it hits Left edge, change direction
		else if (ghost->position->X <= 0) 
		{
			ghost->direction = 0;
		}
		break;

	// Pink Ghost (Pinky)
	case 1:
		// Moves Right towards Pacman
		if (ceil(ghost->position->X) + 16 < ceil(_pacman->position->X) + 10)
		{
			ghost->direction = 0;
			ghost->position->X += ghost->speed * elapsedTime;
		}
		// Moves Left towards Pacman
		else if (ceil(ghost->position->X) + 16 > ceil(_pacman->position->X) + 22)
		{
			ghost->direction = 1;
			ghost->position->X -= ghost->speed * elapsedTime;
		}
		// Moves Down towards Pacman
		else if (ceil(ghost->position->Y) + 16 < ceil(_pacman->position->Y) + 10)
		{
			ghost->direction = 2;
			ghost->position->Y += ghost->speed * elapsedTime;
		}
		// Moves Up towards Pacman
		else if (ceil(ghost->position->Y) + 16 > ceil(_pacman->position->Y) + 22)
		{
			ghost->direction = 3;
			ghost->position->Y -= ghost->speed * elapsedTime;
		}
		break;

	// Red Ghost (Blinky)
	case 2:
		// Moves Down
		if (ghost->direction == 2)
		{
			ghost->position->Y += ghost->speed * elapsedTime;
		}
		// Moves Up
		else if (ghost->direction == 3)
		{
			ghost->position->Y -= ghost->speed * elapsedTime;
		}
		// If it hits Bottom edge, change direction
		if (ghost->position->Y + ghost->sourceRect->Height >= Graphics::GetViewportHeight()) 
		{
			ghost->direction = 3;
		}
		// If it hits Top edge, change direction
		else if (ghost->position->Y <= 0) 
		{
			ghost->direction = 2;
		}
		break;

	// Yellow Ghost (Clyde)
	case 3:
		ghost->currentFrameTime += elapsedTime;

		// Calculates random direction
		if (ghost->currentFrameTime > _cGhostFrameTime)
		{
			ghost->direction = rand() % 4;
			ghost->currentFrameTime = 0;
		}
		// Moves Right
		if (ghost->direction == 0)
		{
			ghost->position->X += ghost->speed * elapsedTime;
		}
		// Moves Left
		else if (ghost->direction == 1)
		{
			ghost->position->X -= ghost->speed * elapsedTime;
		}
		// Moves Down
		else if (ghost->direction == 2)
		{
			ghost->position->Y += ghost->speed * elapsedTime;
		}
		// Moves Up
		else if (ghost->direction == 3)
		{
			ghost->position->Y -= ghost->speed * elapsedTime;
		}

		// If it hits Right edge, change direction
		if (ghost->position->X + ghost->sourceRect->Width >= Graphics::GetViewportWidth()) 
		{
			ghost->direction = 1;
		}
		// If it hits Left edge, change direction
		if (ghost->position->X <= 0)
		{
			ghost->direction = 0;
		}
		// If it hits Bottom edge, change direction
		if (ghost->position->Y + ghost->sourceRect->Height >= Graphics::GetViewportHeight())
		{
			ghost->direction = 3;
		}
		// If it hits Top edge, change direction
		if (ghost->position->Y <= 0)
		{
			ghost->direction = 2;
		}
		break;

	default:
		break;
	}
	ghost->sourceRect->Y = ghost->sourceRect->Height * ghost->direction;
}