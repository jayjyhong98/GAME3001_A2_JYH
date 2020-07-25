#include "HealthBar.h"
#include "TextureManager.h"

HealthBar::HealthBar(SDL_Rect s, SDL_FRect d, SDL_Renderer* r, SDL_Texture* t)
	: Sprite(s, d, r, t) {}

void HealthBar::Render()
{
	float fHealthRatio = (float)m_pParent->getHealth() / (float)m_pParent->getHealthMax();
	m_healthFilled = m_dst;
	m_healthFilled.x += 2.5;
	m_healthFilled.y += 2.8;
	m_healthFilled.w = 28 * fHealthRatio;
	m_healthFilled.h = 10;
	SDL_RenderCopyExF(m_pRend, m_pText, GetSrcP(), GetDstP(), 0, 0, SDL_FLIP_NONE);
	SDL_RenderCopyExF(m_pRend, TEMA::GetTexture("health1"), GetSrcP(), &m_healthFilled, 0, 0, SDL_FLIP_NONE);
}

void HealthBar::Update()
{
	m_dst.x = m_pParent->GetDstP()->x;
	m_dst.y = m_pParent->GetDstP()->y - 10;

}