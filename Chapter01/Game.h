// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "SDL.h"
#include "SDL_ttf.h"
#include <string>
#include <string>
using namespace std;

// Vector2 struct just stores x/y coordinates
// (for now)
struct Vector2
{
	float x;
	float y;
};

// Score struct contem tudo que é necessário para
// posicionar e desenhar o score na tela
struct Score {
	int height; // Altura do score
	int width; // Largura do score
	int value = 0; // Valor do score
	string text = "Score: "; // Texto base do score
	TTF_Font* font = TTF_OpenFont("arial.ttf", 18); // Fonte usada no score
	SDL_Color color = { 215, 215, 215 }; // Cor do score na tela
};

// Game class
class Game
{
public:
	Game();
	// Initialize the game
	bool Initialize();
	// Runs the game loop until the game is over
	void RunLoop();
	// Shutdown the game
	void Shutdown();
private:
	// Helper functions for the game loop
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();
	void renderScore();
	// Window created by SDL
	SDL_Window* mWindow;
	// Renderer for 2D drawing
	SDL_Renderer* mRenderer;
	// Number of ticks since start of game
	Uint32 mTicksCount;
	// Game should continue to run
	bool mIsRunning;
	
	// Pong specific
	// Direction of paddle
	int mPaddleDir;
	int mPaddleDir2;
	// Position of paddle
	Vector2 mPaddlePos;
	Vector2 mPaddlePos2;
	// Position of ball
	Vector2 mBallPos;
	// Velocity of ball
	Vector2 mBallVel;
	//Score
	Score score;
};
