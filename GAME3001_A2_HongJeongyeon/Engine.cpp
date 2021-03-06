#include "Engine.h"
#include "CollisionManager.h"
#include "DebugManager.h"
#include "EventManager.h"
#include "FontManager.h"
#include "PathManager.h"
#include "SoundManager.h"
#include "StateManager.h"
#include "TextureManager.h"
#include <iostream>
#include <fstream>
#include <ctime>
#define WIDTH 1024
#define HEIGHT 768
#define FPS 60
using namespace std;

Engine::Engine() :m_running(false) { cout << "Engine class constructed!" << endl; }

bool Engine::Init(const char* title, int xpos, int ypos, int width, int height, int flags)
{
	srand(time(NULL));
	cout << "Initializing game..." << endl;
	// Attempt to initialize SDL.
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
	{
		// Create the window.
		m_pWindow = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
		if (m_pWindow != nullptr) // Window init success.
		{
			m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, 0);
			if (m_pRenderer != nullptr) // Renderer init success.
			{
				EVMA::Init();
				SOMA::Init();
				TEMA::Init();
			}
			else return false; // Renderer init fail.
		}
		else return false; // Window init fail.
	}
	else return false; // SDL init fail.
	// Example specific initialization.
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2"); // Call this before any textures are created.
	srand((unsigned)time(NULL));
	FOMA::RegisterFont("Img/LTYPE.TTF", "ltype", 20);
	FOMA::RegisterFont("Img/font.TTF", "font", 35);
	TEMA::RegisterTexture("Img/play.png", "play");
	TEMA::RegisterTexture("Img/Tiles.png", "bground");
	TEMA::RegisterTexture("Img/Maga.png", "player");
	TEMA::RegisterTexture("Img/enemy.png", "enemy");
	TEMA::RegisterTexture("Img/player.png", "player1");
	TEMA::RegisterTexture("Img/firing.png", "bullet");
	TEMA::RegisterTexture("Img/projectile.png", "projectile");
	TEMA::RegisterTexture("Img/Idle (1).png", "enemycount");
	TEMA::RegisterTexture("Img/Dead (12).png", "deadenemycount");
	TEMA::RegisterTexture("Img/health.png", "health");
	TEMA::RegisterTexture("Img/health1.png", "health1");
	SOMA::Load("Aud/engines.wav", "engine", SOUND_SFX);
	SOMA::Load("Aud/jump.wav", "beep", SOUND_SFX);
	SOMA::Load("Aud/Turtles.mp3", "bground", SOUND_MUSIC);
	SOMA::Load("Aud/boom.wav", "melee", SOUND_SFX);
	SOMA::Load("Aud/walk.wav", "walk", SOUND_SFX);
	SOMA::Load("Aud/zombie.wav", "zombie", SOUND_SFX);
	SOMA::Load("Aud/projectile.wav", "projectile", SOUND_SFX);
	SOMA::SetSoundVolume(50, -1);
	StateManager::PushState(new TitleState);
	// Final engine initialization calls.
	m_fps = (Uint32)round((1 / (double)FPS) * 1000); // Sets FPS in milliseconds and rounds.
	m_running = true; // Everything is okay, start the engine.
	cout << "Engine Init success!" << endl;
	return true;
}

void Engine::Wake()
{
	m_start = SDL_GetTicks();
}

void Engine::Sleep()
{
	m_end = SDL_GetTicks();
	m_delta = m_end - m_start;
	if (m_delta < m_fps) // Engine has to sleep.
		SDL_Delay(m_fps - m_delta);
}

void Engine::HandleEvents()
{
	EVMA::HandleEvents();
}

void Engine::Update()
{
	StateManager::Update();
}

void Engine::Render()
{
	SDL_SetRenderDrawColor(m_pRenderer, 0, 0, 0, 255);
	SDL_RenderClear(m_pRenderer); // Clear the screen with the draw color.
	StateManager::Render();
	SDL_RenderPresent(m_pRenderer);
}

void Engine::Clean()
{
	cout << "Cleaning game." << endl;
	StateManager::Quit();
	// Finish cleaning.
	SDL_DestroyRenderer(m_pRenderer);
	SDL_DestroyWindow(m_pWindow);
	DEMA::Quit();
	EVMA::Quit();
	FOMA::Quit();
	SOMA::Quit();
	STMA::Quit();
	TEMA::Quit();
	IMG_Quit();
	SDL_Quit();
}

int Engine::Run()
{
	if (m_running) // What does this do and what can it prevent?
		return -1;
	if (Init("GAME1017 Engine Template", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0) == false)
		return 1;
	while (m_running) // Main engine loop.
	{
		Wake();
		HandleEvents();
		Update();
		Render();
		if (m_running)
			Sleep();
	}
	Clean();
	return 0;
}

Engine& Engine::Instance()
{
	static Engine instance; // C++11 will prevent this line from running more than once. Magic statics.
	return instance;
}

SDL_Renderer* Engine::GetRenderer() { return m_pRenderer; }
bool& Engine::Running() { return m_running; }
