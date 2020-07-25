#include "Projectile.h"
#include "Sprite.h"
#include "Engine.h"


Projectile::Projectile(SDL_Rect src, SDL_FRect dst, SDL_Renderer* r, SDL_Texture* t) :
	Sprite(src, dst, r, t) {}

Projectile::~Projectile()
{

}

void Projectile::update()
{
	m_dst.x += velX * 10;
	m_dst.y += velY * 10;
}

void Projectile::render()
{
	SDL_RenderCopyF(m_pRend, m_pText, &m_src, &m_dst);
}