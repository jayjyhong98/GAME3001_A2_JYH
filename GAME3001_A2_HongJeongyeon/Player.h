#pragma once
#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "Sprite.h"
#include <vector>

class Projectile;
class HealthBar;

class Player : public AnimatedSprite
{
public:
	Player(SDL_Rect s, SDL_FRect d, SDL_Renderer* r, SDL_Texture* t, int sstart, int smin, int smax, int nf);
	void MoveAlongPath();
	bool getIsPlayerMoved() { return m_bIsPlayerMoved; }
	void setIsPlayerMoved(bool s) { m_bIsPlayerMoved = s; }
	void Update();
	void Render();
	void Melee();
	void createProjectile();
	std::vector<Projectile*>& getProjectile() { return m_vPProjectiles; }
private:
	enum state { idle, running } m_state;
	bool m_dir;
	bool m_bIsPlayerMoved = false;
	bool m_bReadyToMove = true;
	int NextNodeIdx = -1;
	int NextDstX = 0;
	int NextDstY = 0;
	int m_distance;
	float movingVelocityX = 0;
	float movingVelocityY = 0;
	bool m_facingRight = true;
	void SetState(int s);
	std::vector<Projectile*> m_vPProjectiles;
	HealthBar* m_pHealthbar;

};

#endif

