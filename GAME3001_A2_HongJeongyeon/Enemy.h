#pragma once
#ifndef _ENEMY_H_
#define _ENEMY_H_

#include "Sprite.h"
#include <vector>
#include "glm/vec2.hpp"

class Projectile;
class HealthBar;
class PathConnection;
class Tile;

class Enemy : public AnimatedSprite
{
public:
	enum state { idle, patrol, death, damage } m_state;
	void ChangeState(state s);
public:
	Enemy(SDL_Rect s, SDL_FRect d, SDL_Renderer* r, SDL_Texture* t, int sstart, int smin, int smax, int nf);
	//void MoveAlongPath();
	bool getIsEnemyMoved() { return m_bIsEnemyMoved; }
	void setIsEnemyMoved(bool s) { m_bIsEnemyMoved = s; }
	void Update();
	void Render();
	void DetectionRadius();
	//void Melee();
	//void createEnemyProjectile();
	//void EnemyProjectileCollision();
	//void EnemyProjectileBoundCheck();
	int getEnemyHealth() { return m_health; }
	void setEnemyHealth(int h) { m_health = h; }
private:
	bool m_dir;
	bool m_bIsEnemyMoved = false;
	bool m_bReadyToMove = true;
	bool m_bHasFoundPass = false;
	int NextNodeIdx = -1;
	int NextDstX = 0;
	int NextDstY = 0;
	int m_iSpriteY = 0;
	glm::vec2 m_vDirection;

	void SetState(int s);
	std::vector<Projectile*> m_vEProjectiles;
	std::vector<PathConnection*> m_vEnemyPath;
	std::vector<Tile*> m_vPatrolTile;
	HealthBar* m_eHealthbar;

};

#endif

