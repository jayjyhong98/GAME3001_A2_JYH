#pragma once
#ifndef _TILE_H_
#define _TILE_H_
#include "Sprite.h"
#include "Pathing.h"
#include "Label.h"
#include "States.h"

class Tile : public Sprite
{
public:
	Tile(SDL_Rect s, SDL_FRect d, SDL_Renderer* r, SDL_Texture* t, const bool o, const bool h, int cost)
		:Sprite(s, d, r, t), m_obstacle(o), m_hazard(h), m_iCost(cost)
	{
		m_node = nullptr;
		m_lCost = m_lX = m_lY = nullptr;
	}
	~Tile()
	{
		delete m_node; // Will also invoke PathNode destructor. Deletes connections.
		delete m_lCost;
		delete m_lX;
		delete m_lY;
	}
	bool& IsObstacle() { return m_obstacle; }
	bool IsHazard() { return m_hazard; }
	Tile* Clone() { return new Tile(m_src, m_dst, m_pRend, m_pText, m_obstacle, m_hazard, m_iCost); }
	PathNode* Node() { return m_node; }
	std::vector<Tile*> getNeighbour();
	int getTileRowIdx() { return m_iTileRowIdx; }
	int getTileColIdx() { return m_iTileColIdx; }
	void setTileRowIdx(int rowidx);
	void setTileColIdx(int colidx);
public:
	PathNode* m_node;
	Label *m_lCost, *m_lX, *m_lY;
private:
	bool m_obstacle, m_hazard;
	int m_iTileRowIdx, m_iTileColIdx;
	int m_iCost;
};
#endif
