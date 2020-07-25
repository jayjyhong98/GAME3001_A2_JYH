#pragma once
#ifndef _STATES_H_
#define _STATES_H_

#include <SDL.h>
#include "Button.h"
#include "Sprite.h"
#include <map>
#include <array>
#include <ctime>
#include <vector>

#define ROWS 24
#define COLS 32

class Label;
class Sprite;
class Player;
class Tile;
class Enemy;
//class DebugKeys;

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
	Sprite* m_pEnemyDeadUI;
	Sprite* m_pEnemyAliveUI;
	Label* words[2]; // 1 : Enemy Dead Count, 2 : Enemy Alive Count
	std::vector<Enemy*> s_enemies;
	// Example-specific properties.
	SDL_Texture* m_pTileText, * m_pPlayerText;
	std::map<char, Tile*> m_tiles;
	std::array<std::array<Tile*, COLS>, ROWS> m_level; // Fixed-size STL array of Tile pointers.
	std::vector<Tile*> m_vObstacles;
	bool m_showCosts = false, m_hEuclid = true;
	bool m_bDebug = false;
	bool m_bPatrol = false;
	//bool m_bDebugKeys[DebugKeys::NUM_OF_DEBUG_KEYS];
	bool m_bHpressed;
	bool m_bKpressed;
	bool m_bPpressed;
	int m_iEnemyCount = 0;
	int m_iEnemyDeadCount = 0;
	int m_iDebugStartRowIndex = -1;
	int m_iDebugStartColIndex = -1;
	int m_iDebugGoalRowIndex = -1;
	int m_iDebugGoalColIndex = -1;

public:
	std::array<std::array<Tile*, COLS>, ROWS>& GetLevel() { return m_level; }
	std::vector<Enemy*>& getEnemies() { return s_enemies; }
	std::vector<Tile*>& getObstacles() { return m_vObstacles; }
	Player* getPlayer() { return m_pPlayer; }
	void Update();
	//void CheckCollision();
	void Render();
	void Enter();
	void Exit();
	void Resume();
	int getEnemyCount() { return m_iEnemyCount; }
	int getEnemyDeadCount() { return m_iEnemyDeadCount; }
	void setEnemyCount(int ec) { m_iEnemyCount = ec; }
	void setEnemyDeadCount(int edc) { m_iEnemyDeadCount = edc; }
	bool getDebug() { return m_bDebug; }
	bool getPatrol() { return m_bPatrol; }
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