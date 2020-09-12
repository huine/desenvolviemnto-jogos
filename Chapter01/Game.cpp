// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Game.h"

const int thickness = 20;
const float paddleH = 100.0f;//tamanho da raquete

Game::Game()
:mWindow(nullptr)//para criar uma janela
,mRenderer(nullptr)//para fins de renderização na tela
,mTicksCount(0)//para guardar o tempo decorrido no jogo
,mIsRunning(true)//verificar se o jogo ainda deve continuar sendo executado
,mPaddleDir(0)
,mPaddleDir2(0)
{
	
}

bool Game::Initialize()
{
	// Initialize SDL
	int sdlResult = SDL_Init(SDL_INIT_VIDEO);
	if (sdlResult != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}
	
	// Create an SDL Window
	mWindow = SDL_CreateWindow(
		"Game Programming in C++ (Chapter 1)", // Window title
		100,	// Top left x-coordinate of window
		100,	// Top left y-coordinate of window
		1024,	// Width of window
		768,	// Height of window
		0		// Flags (0 for no flags set)
	);

	if (!mWindow)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}
	
	//// Create SDL renderer
	mRenderer = SDL_CreateRenderer(
		mWindow, // Window to create renderer for
		-1,		 // Usually -1
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);

	if (!mRenderer)
	{
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return false;
	}
	
	mPaddlePos.x = 10.0f;//posição inicial da raquete eixo x
	mPaddlePos.y = 768.0f/2.0f;//posição inicial da raquee eixo y
	mPaddlePos2.x = 1014.0f - thickness;//posição inicial da raquete eixo x
	mPaddlePos2.y = 768.0f / 2.0f;//posição inicial da raquee eixo y
	mBallPos.x = 1024.0f/2.0f;//posição da bola eixo x
	mBallPos.y = 768.0f/2.0f;//posição da bola eixo y
	mBallVel.x = -200.0f;//velocidade de movimentação da bola no eixo x
	mBallVel.y = 235.0f;//velocidade de movimentação da bola no eixo y
	return true;
}

void Game::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::ProcessInput()
{
	SDL_Event event;//evento, inputs do jogador são armazenados aqui
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			// If we get an SDL_QUIT event, end loop
			case SDL_QUIT:
				mIsRunning = false;
				break;
		}
	}
	
	// Get state of keyboard - podemos buscar por alguma tecla específica pressionada no teclado, nesse caso, Escape
	const Uint8* state = SDL_GetKeyboardState(NULL);
	// If escape is pressed, also end loop
	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}
	
	// Update paddle direction based on W/S keys - atualize a direção da raquete com base na entrada do jogador
	// W -> move a raquete para cima, Y -> move a raquete para baixo
	mPaddleDir = 0;
	if (state[SDL_SCANCODE_W])
	{
		mPaddleDir -= 1;
	}
	if (state[SDL_SCANCODE_S])
	{
		mPaddleDir += 1;
	}
}

void Game::UpdateGame()
{
	// Wait until 16ms has elapsed since last frame - limitando os frames
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
		;

	// Delta time is the difference in ticks from last frame
	// (converted to seconds) - calcula o delta time para atualização do jogo
	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	
	// Clamp maximum delta time value - valores máximos de delta time
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}

	// Update tick counts (for next frame) - atualize o tempo corrente do jogo
	mTicksCount = SDL_GetTicks();
	// Update paddle position based on direction - atualiza a posição da raquete
	if (mPaddleDir != 0)
	{
		mPaddlePos.y += mPaddleDir * 300.0f * deltaTime * (1 + score.value / 25.0f);
		// Make sure paddle doesn't move off screen!
		if (mPaddlePos.y < (paddleH / 2.0f + thickness))
		{
			mPaddlePos.y = paddleH / 2.0f + thickness;
		}
		else if (mPaddlePos.y > (768.0f - paddleH / 2.0f - thickness))
		{
			mPaddlePos.y = 768.0f - paddleH / 2.0f - thickness;
		}
	}

	// Tenta prever a posição que a bola vai estar.
	double diff = static_cast<double>(mPaddlePos2.y) - static_cast<double>(mBallPos.y) + static_cast<double>(mBallVel.y) * static_cast<double>(deltaTime);
	// Update paddle position based on direction - atualiza a posição da raquete
	mPaddleDir2 = 0;
	// Se o diff for < 0 então a bola está para baixo da raquete
	if (diff < 0.0f) mPaddleDir2 += 1;
	// Se o diff for > 0 então a bola está para cima da raquete
	if (diff > 0.0f) mPaddleDir2 += -1;
	if (mPaddleDir2 != 0)
	{
		mPaddlePos2.y += mPaddleDir2 * 300.0f * deltaTime * (1 + score.value / 25.0f);
		// Make sure paddle doesn't move off screen!
		if (mPaddlePos2.y < (paddleH / 2.0f + thickness))
		{
			mPaddlePos2.y = paddleH / 2.0f + thickness;
		}
		else if (mPaddlePos2.y > (768.0f - paddleH / 2.0f - thickness))
		{
			mPaddlePos2.y = 768.0f - paddleH / 2.0f - thickness;
		}
	}
	
	// Update ball position based on ball velocity - atualiza a posição da bola com base na sua velocidade
	mBallPos.x += mBallVel.x * deltaTime;
	mBallPos.y += mBallVel.y * deltaTime;
	
	//Atuliza a posição da bola se ela colidiu com a raquete
	// Bounce if needed
	// Did we intersect with the paddle?
	// Take absolute value of difference
	//verifica se a bola está na area da raquete e na mesma posição no eixo x
	diff = fabs(static_cast<double>(mPaddlePos.y) - static_cast<double>(mBallPos.y));
	// Calcula a diferença absoluta entra a cordenada x da raquete e a cordenada x da bola
	// Usando isso para calcular a colisão é mais intuitivo e permite melhor ajuste 
	// no tamanho do erro que seria aceitavel
	double diff_x = fabs((static_cast<double>(mPaddlePos.x) + static_cast<double>(thickness)) - static_cast<double>(mBallPos.x));
	if (
		// Our y-difference is small enough
		diff <= paddleH / 2.0f &&
		// We are in the correct x-position
		diff_x >= 0 && diff_x <= 5 &&
		// The ball is moving to the left
		mBallVel.x < 0.0f)
	{
		mBallVel.x *= -1.0f;
		score.value += 1;
		//Aumenta a velocidade da bolinha de acordo com a pontução
		if (fabs(mBallVel.x) <= 900.0f) {
			mBallVel.x *= (1 + score.value / 50.0f);
			mBallVel.y *= (1 + score.value / 50.0f);
		}
	}

	diff = fabs(static_cast<double>(mPaddlePos2.y) - static_cast<double>(mBallPos.y));
	//como a cordenada é da esquerda para direita, não precisamos considerar a largura da raquete nesse caso
	diff_x = fabs(static_cast<double>(mPaddlePos2.x) - static_cast<double>(mBallPos.x));
	if (
		// Our y-difference is small enough
		diff <= paddleH / 2.0f &&
		// We are in the correct x-position
		diff_x >= 0 && diff_x <= 5 &&
		// The ball is moving to the right
		mBallVel.x > 0.0f)
	{
		mBallVel.x *= -1.0f;
	}

	//Verifica se a bola saiu da tela (no lado esquerdo ou direito)
	//Se sim, encerra o jogo
	// Did the ball go off the screen? (if so, end game)
	else if (mBallPos.x <= 0.0f || mBallPos.x >= 1024)
	{
		mIsRunning = false;
	}

	//Atulize a velocidade da bola se ela colidir com a parede do lado direito
	// Did the ball collide with the right wall?
	//else if (mBallPos.x >= (1024.0f - thickness) && mBallVel.x > 0.0f)
	//{
		//mBallVel.x *= -1.0f;
	//}
	
	//Atualize a posição da bola se ela colidir com a parede de cima
	// Did the ball collide with the top wall?
	if (mBallPos.y <= thickness && mBallVel.y < 0.0f)
	{
		mBallVel.y *= -1;
	}

	//Atualize a posição da bola se ela colidiu com a parede de baixo
	// Did the ball collide with the bottom wall?
	else if (mBallPos.y >= (768 - thickness) &&
		mBallVel.y > 0.0f)
	{
		mBallVel.y *= -1;
	}	
}

//Desenhando a tela do jogo
void Game::GenerateOutput()
{
	// Set draw color to blue - Setamos a cor original para o back buffer
	SDL_SetRenderDrawColor(
		mRenderer,
		0,		// R
		0,		// G 
		0,	// B
		0	// A
	);
	
	// Clear back buffer - passamos a cor da tela para o back buffer
	SDL_RenderClear(mRenderer);

	// Draw walls - mudamos a cor de mRenderer para o desenho dos retangulos que formaram as paredes
	SDL_SetRenderDrawColor(mRenderer, 100, 100, 100, 100);
	
	// Draw top wall - desenhando um retangulo no topo da tela, coordenada x e y = 0, 0 representa o topo esquerdo
	//primeiro definimos as coordenadas e tamanhos do triangulo
	SDL_Rect wall{
		0,			// Top left x
		0,			// Top left y
		1024,		// Width
		thickness	// Height
	};
	SDL_RenderFillRect(mRenderer, &wall);//finalmente, desenhamos um retangulo no topo
	
	//desenhamos as outras paredes apenas mudando as coordenadas de wall
	// Draw bottom wall
	wall.y = 768 - thickness;
	SDL_RenderFillRect(mRenderer, &wall);
	
	// Draw right wall
	//wall.x = 1024 - thickness;
	//wall.y = 0;
	//wall.w = thickness;
	//wall.h = 1024;
	//SDL_RenderFillRect(mRenderer, &wall);
	
	//como as posições da raquete e da boal serão atualizadas a cada iteração do game loop, criamos "membros" na classe
	//Game.h para tal

	//desenhando a raquete - usando mPaddlePos que é uma struc de coordenada que foi definida em Game.h
	// Draw paddle
	SDL_Rect paddle{
		static_cast<int>(mPaddlePos.x),//static_cast converte de float para inteiros, pois SDL_Rect trabalha com inteiros
		static_cast<int>(mPaddlePos.y - paddleH/2),
		thickness,
		static_cast<int>(paddleH)
	};
	SDL_RenderFillRect(mRenderer, &paddle);

	paddle.x = static_cast<int>(mPaddlePos2.x);
	paddle.y = static_cast<int>(mPaddlePos2.y - paddleH / 2);
	SDL_RenderFillRect(mRenderer, &paddle);
	
	//desenhando a bola - usando mBallPos que é uma struc de coordenadas definida como membro em Game.h
	// Draw ball
	SDL_Rect ball{	
		static_cast<int>(mBallPos.x - thickness/2),
		static_cast<int>(mBallPos.y - thickness/2),
		thickness,
		thickness
	};
	SDL_RenderFillRect(mRenderer, &ball);

	SDL_Rect middleLine{
		static_cast<int>(1024/2.0f - 1),
		0,
		2,
		1024
	};
	SDL_RenderFillRect(mRenderer, &middleLine);
	//Render do score
	renderScore();
	// Swap front buffer and back buffer
	SDL_RenderPresent(mRenderer);
}

//Para encerrar o jogo
void Game::Shutdown()
{
	SDL_DestroyRenderer(mRenderer);//encerra o renderizador
	SDL_DestroyWindow(mWindow);//encerra a janela aberta
	SDL_Quit();//encerra o jogo
}

void Game::renderScore() {
	// Cria a superficie e renderiza o text
	SDL_Surface* scoreSurface = TTF_RenderText_Solid(
		score.font,
		(score.text + to_string(score.value)).c_str(),
		score.color
	);

	if (scoreSurface == NULL)
	{
		throw SDL_GetError();
	}
	else
	{
		//Cria a textura com o score a partir dos pixels da superficie
		SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(mRenderer, scoreSurface);
		if (scoreTexture == NULL)
		{
			throw SDL_GetError();
		}
		else
		{
			//Pega as dimenssões do score (WxH) e coloca na struct score
			TTF_SizeText(
				score.font,
				(score.text + to_string(score.value)).c_str(),
				&score.width,
				&score.height
			);
		}
		// Agora que tema textura, pode descartar a superficie
		SDL_FreeSurface(scoreSurface);
		// Gera o retangulo para exibir o score
		SDL_Rect scoreRect{
			static_cast<int>(1024 / 2 - score.width/2), //Exibe score no meio da tela horizontalmente
			0, //Coloca o score no topo da tela
			score.width, //Largura do score
			score.height // Altura do score
		};
		//Renderiza a textura no retangulo
		SDL_RenderCopy(mRenderer, scoreTexture, NULL, &scoreRect);
		//Agora que já renderizou a textura, pode descartar
		SDL_DestroyTexture(scoreTexture);
	}
}