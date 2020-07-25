#pragma once
#include "Sprite.h"

class Projectile : public Sprite
{
private:

	float m_speed = 5.0f;
	float velX = 0;
	float velY = 0;

public:
	Projectile(SDL_Rect src, SDL_FRect dst, SDL_Renderer* r, SDL_Texture* t);
	~Projectile();
	void setVelX(float x) { velX = x; }
	void setVelY(float y) { velY = y; }
	void update();
	void render();
};