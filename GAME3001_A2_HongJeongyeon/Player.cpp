#include "Player.h"
#include "Enemy.h"
#include "CollisionManager.h"
#include "EventManager.h"
#include "PathManager.h"
#include "Pathing.h"
#include "StateManager.h"
#include "Engine.h"
#include "HealthBar.h"
#include "Projectile.h"
#include "TextureManager.h"
#include "States.h"
#include "Vector2D.h"
#include "glm/vec2.hpp"
#define SPEED 2
#define ELLIPSE 0.1

Player::Player(SDL_Rect s, SDL_FRect d, SDL_Renderer* r, SDL_Texture* t, int sstart, int smin, int smax, int nf)
	:AnimatedSprite(s, d, r, t, sstart, smin, smax, nf), m_state(state::idle), m_dir(0)
{
	m_pHealthbar = new HealthBar({ 0, 0, 193, 52 }, { m_dst.x, m_dst.y - 10, 32, 16 }, Engine::Instance().GetRenderer(), TEMA::GetTexture("health"));
	m_pHealthbar->setParent(this);
	m_health = 30;
	m_healthMax = 100;
}

void Player::Update()
{
	switch (m_state)
	{
	case idle:
		if (EVMA::KeyHeld(SDL_SCANCODE_W) || EVMA::KeyHeld(SDL_SCANCODE_S) ||
			EVMA::KeyHeld(SDL_SCANCODE_A) || EVMA::KeyHeld(SDL_SCANCODE_D))
		{
			SetState(running);
		}
		break;
	case running:
		if (EVMA::KeyReleased(SDL_SCANCODE_W) || EVMA::KeyReleased(SDL_SCANCODE_S) ||
			EVMA::KeyReleased(SDL_SCANCODE_A) || EVMA::KeyReleased(SDL_SCANCODE_D))
		{
			SetState(idle);
			break; // Skip movement parsing below.
		}
		if (EVMA::KeyHeld(SDL_SCANCODE_W))
		{
			if (m_dst.y > 0 && !COMA::PlayerCollision({ (int)m_dst.x, (int)(m_dst.y), (int)32, (int)32 }, 0, -SPEED))
			{
				m_dst.y += -SPEED;
			}
		}
		else if (EVMA::KeyHeld(SDL_SCANCODE_S))
		{
			if (m_dst.y < 768 - 32 && !COMA::PlayerCollision({ (int)m_dst.x, (int)(m_dst.y), (int)32, (int)32 }, 0, SPEED))
			{
				m_dst.y += SPEED;
			}
		}
		if (EVMA::KeyHeld(SDL_SCANCODE_A))
		{
			if (m_dst.x > 0 && !COMA::PlayerCollision({ (int)m_dst.x, (int)m_dst.y, (int)32, (int)32 }, -SPEED, 0))
			{
				m_dst.x += -SPEED;
				m_dir = 1;
			}
		}
		else if (EVMA::KeyHeld(SDL_SCANCODE_D))
		{
			if (m_dst.x < 1024 - 32 && !COMA::PlayerCollision({ (int)m_dst.x, (int)m_dst.y, (int)32, (int)32 }, SPEED, 0))
			{
				m_dst.x += SPEED;
				m_dir = 0;
			}
		}
		break;
	}
	Animate();
	if (m_bIsPlayerMoved == true)
	{
		MoveAlongPath();
	}
	m_pHealthbar->Update();

	// projectile update
	for (int i = 0; i < m_vPProjectiles.size(); ++i)
	{
		m_vPProjectiles[i]->update();
	}
}

void Player::MoveAlongPath()
{
	if (m_bReadyToMove == true)
	{
		m_bReadyToMove = false;
		NextNodeIdx++;
		if (NextNodeIdx < PAMA::getPath().size())
		{
			NextDstX = PAMA::getPath()[NextNodeIdx]->GetToNode()->x + 16;
			NextDstY = PAMA::getPath()[NextNodeIdx]->GetToNode()->y + 16;

			movingVelocityX = (NextDstX - (m_dst.x + (m_dst.w / 2)));
			movingVelocityY = (NextDstY - (m_dst.y + (m_dst.h / 2)));

			Vector2D vector(movingVelocityX, movingVelocityY);
			movingVelocityX = movingVelocityX / vector.length();
			movingVelocityY = movingVelocityY / vector.length();
		}
		else
		{
			m_bIsPlayerMoved = false;
			m_bReadyToMove = true;
			NextNodeIdx = -1;
			PAMA::ClearPath();
			return;
		}
	}
	m_dst.x = m_dst.x + movingVelocityX * SPEED;
	m_dst.y = m_dst.y + movingVelocityY * SPEED;

	float PlayerDstX = (m_dst.x + (m_dst.w / 2));
	float PlayerDstY = (m_dst.y + (m_dst.h / 2));

	if (abs(PlayerDstX - NextDstX) <= 0.1 && abs(PlayerDstY - NextDstY) <= 0.1)
	{
		m_bReadyToMove = true;
	}
}

void Player::Render()
{
	SDL_RenderCopyExF(m_pRend, m_pText, GetSrcP(), GetDstP(), m_angle, 0, static_cast<SDL_RendererFlip>(m_dir));
	m_pHealthbar->Render();

	if (((GameState*)(STMA::GetStates().back()))->getDebug() == true)
	{
		SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 255, 0, 0, 255);
		SDL_Rect pRect = { m_dst.x, m_dst.y, m_dst.w, m_dst.h };
		SDL_RenderDrawRect(Engine::Instance().GetRenderer(), &pRect);
	}
	// projectile render
	for (int i = 0; i < m_vPProjectiles.size(); ++i)
	{
		m_vPProjectiles[i]->render();
	}
}

void Player::SetState(int s)
{
	m_state = static_cast<state>(s);
	m_frame = 0;
	if (m_state == idle)
	{
		m_sprite = m_spriteMin = m_spriteMax = 0;
	}
	else // Only other is running for now...
	{
		m_sprite = m_spriteMin = 1;
		m_spriteMax = 4;
	}
}

void Player::Melee()
{
	float PlayerX = m_dst.x + (m_dst.w * 0.5);
	float PlayerY = m_dst.y + (m_dst.h * 0.5);

	for (int i = 0; i < ((GameState*)(STMA::GetStates().back()))->getEnemies().size(); ++i)
	{
		SDL_FRect* EnemyDstP = ((GameState*)(STMA::GetStates().back()))->getEnemies()[i]->GetDstP();
		float EnemyX = EnemyDstP->x + (EnemyDstP->w * 0.5);
		float EnemyY = EnemyDstP->y + (EnemyDstP->h * 0.5);

		m_distance = sqrtf(((PlayerX - EnemyX) * (PlayerX - EnemyX)) + ((PlayerY - EnemyY) * (PlayerY - EnemyY)));

		if (m_distance < 100)
		{
			Enemy* Enemy = ((GameState*)(STMA::GetStates().back()))->getEnemies()[i];
			Enemy->setEnemyHealth(Enemy->getEnemyHealth() - 10);
		}
	}
	std::cout << "Enemy's Attacked!!" << std::endl;

}

void Player::createProjectile()
{
	Projectile* projectile = new Projectile({ 0,0,36,36 }, { m_dst.x, m_dst.y, 32, 32 }, Engine::Instance().GetRenderer(), TEMA::GetTexture("player"));

	glm::vec2 projectileDir(EVMA::GetMousePos().x - projectile->GetDstP()->x, EVMA::GetMousePos().y - projectile->GetDstP()->y);
	float length = sqrtf(projectileDir.x * projectileDir.x + projectileDir.y * projectileDir.y);
	projectileDir.x /= length;
	projectileDir.y /= length;

	if (m_facingRight == true)
	{

		projectile->setVelX(projectileDir.x);
		projectile->setVelY(projectileDir.y);

	}
	else
	{
		projectile->setVelX(projectileDir.x);
		projectile->setVelY(projectileDir.y);
	}

	m_vPProjectiles.push_back(projectile);
}

