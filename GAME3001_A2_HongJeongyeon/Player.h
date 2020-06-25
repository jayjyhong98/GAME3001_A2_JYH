#pragma once
#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "Sprite.h"

class Player : public AnimatedSprite
{
public:
	Player(SDL_Rect s, SDL_FRect d, SDL_Renderer* r, SDL_Texture* t, int sstart, int smin, int smax, int nf);
	void MoveAlongPath();
	bool getIsPlayerMoved() { return m_bIsPlayerMoved; }
	void setIsPlayerMoved(bool s) { m_bIsPlayerMoved = s; }
	void Update();
	void Render();
private:
	enum state { idle, running } m_state;
	bool m_dir;
	bool m_bIsPlayerMoved = false;
	bool m_bReadyToMove = true;
	int NextNodeIdx = -1;
	int NextDstX = 0;
	int NextDstY = 0;
	float movingVelocityX = 0;
	float movingVelocityY = 0;
	void SetState(int s);
};

#endif

