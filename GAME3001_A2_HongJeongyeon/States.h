#pragma once
#ifndef _STATES_H_
#define _STATES_H_

#include <SDL.h>
#include "Button.h"
#include "Sprite.h"
#include <map>
#include <array>
#include <ctime>

#define ROWS 24
#define COLS 32

class Label;
class Sprite;
class Player;
class Tile;

class State // This is the abstract base class for all specific states.
{
public: // Public methods.
	virtual void Update() = 0; // Having at least one 'pure virtual' method like this, makes a class abtract.
	virtual void Render();     // Meaning we cannot create objects of the class.
	virtual void Enter() = 0;  // Virtual keyword means we can override in derived class.
	virtual void Exit() = 0;
	virtual void Resume();

protected: // Private but inherited.
	State() {}
};

class GameState : public State
{
private:
	Label* m_explanation;
	Player* m_pPlayer;
	Sprite* m_pBling;
// Example-specific properties.
	SDL_Texture* m_pTileText, * m_pPlayerText;
	std::map<char, Tile*> m_tiles;
	std::array<std::array<Tile*, COLS>, ROWS> m_level; // Fixed-size STL array of Tile pointers.
	bool m_showCosts = false, m_hEuclid = true;
	bool m_bDebug = false;
	int m_iDebugStartRowIndex = -1;
	int m_iDebugStartColIndex = -1;
	int m_iDebugGoalRowIndex = -1;
	int m_iDebugGoalColIndex = -1;

public:
	std::array<std::array<Tile*, COLS>, ROWS>& GetLevel() { return m_level; }
	void Update();
	void CheckCollision();
	void Render();
	void Enter();
	void Exit();
	void Resume();
};

class TitleState : public State
{
public:
	void Update();
	void Render();
	void Enter();
	void Exit();
private:
	Label* m_Label;
	Label* m_lLabel;
	Button* m_playBtn;
	Button* m_quitBtn;

};

#endif