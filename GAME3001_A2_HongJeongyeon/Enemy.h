#pragma once
#ifndef _ENEMY_H_
#define _ENEMY_H_

#include "Sprite.h"
#include <vector>

class Projectile;
class HealthBar;

class Enemy : public AnimatedSprite
{
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
	enum state { idle } m_state;
	bool m_dir;
	bool m_bIsEnemyMoved = false;
	bool m_bReadyToMove = true;
	bool m_bHasFoundPass = false;
	int NextNodeIdx = -1;
	int NextDstX = 0;
	int NextDstY = 0;
	float movingVelocityX = 0;
	float movingVelocityY = 0;
	int m_iDebugStartRowIndex = -1;
	int m_iDebugStartColIndex = -1;
	int m_iDebugGoalRowIndex = -1;
	int m_iDebugGoalColIndex = -1;

	void SetState(int s);
	std::vector<Projectile*> m_vEProjectiles;
	/*std::vector<PathConnection*> m_vEnemyPath;
	std::vector<Tile*> m_vPatrolTile;*/
	HealthBar* m_eHealthbar;

};

#endif

