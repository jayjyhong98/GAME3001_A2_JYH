#pragma once
#ifndef _HEALTHBAR_H_
#define _HEALTHBAR_H_

#include "Sprite.h"
#include <vector>

class HealthBar : public Sprite
{
public:

	HealthBar(SDL_Rect s, SDL_FRect d, SDL_Renderer* r, SDL_Texture* t);
	void setParent(AnimatedSprite* p) { m_pParent = p; }
	void Render();
	void Update();

private:

	AnimatedSprite* m_pParent;
	SDL_FRect m_healthFilled;
};

#endif