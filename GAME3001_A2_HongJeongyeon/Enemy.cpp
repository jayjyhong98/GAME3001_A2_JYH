#include "Enemy.h"
#include "Player.h"
#include "Projectile.h"
#include "HealthBar.h"
#include "Engine.h"
#include "TextureManager.h"
#include "CollisionManager.h"
#include "SoundManager.h"
#include "EventManager.h"
#include "PathManager.h"
#include "Pathing.h"
#include "StateManager.h"
#include "States.h"
#include "Vector2D.h"
#include "Util.h"
#include <ctime>
#define SPEED 2
#define ELLIPSE 0.1

Enemy::Enemy(SDL_Rect s, SDL_FRect d, SDL_Renderer* r, SDL_Texture* t, int sstart, int smin, int smax, int nf)
	:AnimatedSprite(s, d, r, t, sstart, smin, smax, nf), m_state(state::idle), m_dir(0)
{
	m_eHealthbar = new HealthBar({ 0, 0, 193, 52 }, { m_dst.x, m_dst.y - 10, 32, 16 }, Engine::Instance().GetRenderer(), TEMA::GetTexture("health"));
	m_eHealthbar->setParent(this);
	m_health = 100;
	m_healthMax = 100;

	int xIdx = (m_dst.x + (m_dst.w / 2)) / 32;
	int yIdx = (m_dst.y + (m_dst.h / 2)) / 32;
	std::array<std::array<Tile*, COLS>, ROWS>& m_vLevel = ((GameState*)(STMA::GetStates().back()))->GetLevel();
	// 전체 위치의 가로
	for (int i = -2; i < 3; ++i)
	{
		if (i == 0)
		{
			continue;
		}

		if (m_vLevel[yIdx][xIdx + i]->IsObstacle() == false && m_vLevel[yIdx][xIdx + i]->IsHazard() == false)
		{
			m_vPatrolTile.push_back(m_vLevel[yIdx][xIdx + i]);
		}
	}

	// 서있는 위치의 왼쪽 세로
	for (int i = -1; i < 2; ++i)
	{
		if (i == 0)
		{
			continue;
		}

		if (m_vLevel[yIdx + i][xIdx - 1]->IsObstacle() == false && m_vLevel[yIdx + i][xIdx - 1]->IsHazard() == false)
		{
			m_vPatrolTile.push_back(m_vLevel[yIdx + i][xIdx - 1]);
		}
	}

	// 자기 자신의 세로
	for (int i = -1; i < 2; ++i)
	{
		if (i == 0)
		{
			continue;
		}

		if (m_vLevel[yIdx + i][xIdx]->IsObstacle() == false && m_vLevel[yIdx + i][xIdx]->IsHazard() == false)
		{
			m_vPatrolTile.push_back(m_vLevel[yIdx + i][xIdx]);
		}
	}

	// move to the right
	for (int i = -1; i < 2; ++i)
	{
		if (i == 0)
		{
			continue;
		}

		if (m_vLevel[yIdx + i][xIdx + 1]->IsObstacle() == false && m_vLevel[yIdx + i][xIdx + 1]->IsHazard() == false)
		{
			m_vPatrolTile.push_back(m_vLevel[yIdx + i][xIdx + 1]);
		}
	}
	ChangeState(idle);
}

void Enemy::ChangeState(state s)
{
	m_state = static_cast<state>(s);
	m_frame = 0;
	if (m_state == idle)
	{
		m_sprite = 0;
		m_spriteMin = 0;
		m_spriteMax = 6;
		m_iSpriteY = 4;
		m_src.x = 0;
		m_src.y = 1580;
		m_src.w = 290;
		m_src.h = 447;
	}

	if (m_state == patrol)
	{
		m_sprite = 0;
		m_spriteMin = 0;
		m_spriteMax = 5;
		m_iSpriteY = 6;
		m_src.x = 0;
		m_src.y = 2493;
		m_src.w = 290;
		m_src.h = 400;
	}

	if (m_state == death)
	{
		m_sprite = 0;
		m_spriteMin = 0;
		m_spriteMax = 4;
		m_iSpriteY = 0;
		m_src.x = 0;
		m_src.y = 0;
		m_src.w = 490;
		m_src.h = 275;
	}

	if (m_state == damage)
	{
		m_sprite = 0;
		m_spriteMin = 0;
		m_spriteMax = 1;
		m_iSpriteY = 2;
		m_src.x = 0;
		m_src.y = 670;
		m_src.w = 380;
		m_src.h = 430;
	}
}

void Enemy::Update()
{
	if (m_frame++ == m_frameMax) // Post-increment ensures m_frame starts at 0.
	{
		m_frame = 0;
		if (m_sprite++ == m_spriteMax)
		{
			m_sprite = m_spriteMin;

			if (m_state == death)
			{
				m_bBeAlived = false;
			}
		}
	}
	m_src.x = m_src.w * m_sprite;
	//m_src.y = m_src.h * m_iSpriteY;
	
	if (m_bBeAlived == true && m_state != death)
	{
		m_eHealthbar->Update();

		std::vector<Projectile*>& projectile = ((GameState*)(STMA::GetStates().back()))->getPlayer()->getProjectile();
		std::vector<Projectile*>::iterator iterBegin = projectile.begin();
		for (int i = 0; i < projectile.size(); ++i, ++iterBegin)
		{
			SDL_Rect projectileDst; // a. player bullet dst
			projectileDst.x = projectile[i]->GetDstP()->x;
			projectileDst.y = projectile[i]->GetDstP()->y;
			projectileDst.w = projectile[i]->GetDstP()->w;
			projectileDst.h = projectile[i]->GetDstP()->h;

			SDL_Rect enemyDst; // b. enemy dst
			enemyDst.x = m_dst.x;
			enemyDst.y = m_dst.y;
			enemyDst.w = m_dst.w;
			enemyDst.h = m_dst.h;

			SDL_Rect temp; // c. result - temp


			if (SDL_IntersectRect(&projectileDst, &enemyDst, &temp)) // a. player bullet dst b. enemy dst c. result - temp
			{
				SOMA::PlaySound("zombie", 1);
				ChangeState(Enemy::damage);
				m_health -= 20;
				delete projectile[i];
				projectile.erase(iterBegin);
				break;
			}
		}
		if (m_health <= 0)
		{
			SOMA::PlaySound("zombie", 1);
			ChangeState(Enemy::death);
			((GameState*)(STMA::GetStates().back()))->setEnemyDeadCount(((GameState*)(STMA::GetStates().back()))->getEnemyDeadCount() + 1);
			((GameState*)(STMA::GetStates().back()))->setEnemyCount(((GameState*)(STMA::GetStates().back()))->getEnemyCount() - 1);
			//m_bBeAlived = false;
		}

		if (((GameState*)(STMA::GetStates().back()))->getPatrol() == true)
		{
			if (m_bHasFoundPass == false)
			{
				int xIdx = (m_dst.x + (m_dst.w / 2)) / 32;
				int yIdx = (m_dst.y + (m_dst.h / 2)) / 32;
				std::array<std::array<Tile*, COLS>, ROWS>& m_vLevel = ((GameState*)(STMA::GetStates().back()))->GetLevel();
				int randomTileIdx = rand() % m_vPatrolTile.size();
				Tile* GoalTile = m_vPatrolTile[randomTileIdx];
				PAMA::GetShortestPath(m_vLevel[yIdx][xIdx]->Node(), GoalTile->Node(), m_vEnemyPath);
				m_bHasFoundPass = true;
			}
			if (m_bHasFoundPass == true)
			{
				if (m_bReadyToMove == true)
				{
					++NextNodeIdx;
					if (NextNodeIdx == m_vEnemyPath.size())
					{
						m_bHasFoundPass = false;
						NextNodeIdx = -1;
						return;
					}
					m_vDirection.x = m_vEnemyPath[NextNodeIdx]->GetToNode()->x - m_dst.x;
					m_vDirection.y = m_vEnemyPath[NextNodeIdx]->GetToNode()->y - m_dst.y;
					m_vDirection = Util::normalize(m_vDirection);
					m_bReadyToMove = false;
				}
				if (m_bReadyToMove == false)
				{
					glm::vec2 NextNode;
					NextNode.x = m_vEnemyPath[NextNodeIdx]->GetToNode()->x;
					NextNode.y = m_vEnemyPath[NextNodeIdx]->GetToNode()->y;

					float length = Util::distance(NextNode, { m_dst.x, m_dst.y });
					if (length <= 5)
					{
						m_bReadyToMove = true;
					}
					m_dst.x += m_vDirection.x;
					m_dst.y += m_vDirection.y;
				}
			}
		}
	}

	if (((GameState*)(STMA::GetStates().back()))->getDebug() == true)
	{
		DetectionRadius();

		std::vector<Tile*>& m_vObstacles = ((GameState*)(STMA::GetStates().back()))->getObstacles();
		for (int i = 0; i < m_vObstacles.size(); ++i)
		{
			if (COMA::LOSCheck(this, ((GameState*)(STMA::GetStates().back()))->getPlayer(), m_vObstacles[i]) == true)
			{
				break;
			}
		}
	}
}

void Enemy::Render()
{

	if (m_bBeAlived == true)
	{
		SDL_RenderCopyExF(m_pRend, m_pText, GetSrcP(), GetDstP(), m_angle, 0, static_cast<SDL_RendererFlip>(m_dir));
		m_eHealthbar->Render();

		if (((GameState*)(STMA::GetStates().back()))->getDebug() == true)
		{
			SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 255, 0, 0, 255);
			SDL_Rect eRect = { m_dst.x, m_dst.y, m_dst.w, m_dst.h };
			SDL_RenderDrawRect(Engine::Instance().GetRenderer(), &eRect);
		}
	}


	if (((GameState*)(STMA::GetStates().back()))->getDebug() == true)
	{
		Util::DrawCircle(glm::vec2(m_dst.x + (m_dst.w / 2), m_dst.y + (m_dst.h / 2)), 100);
	}
}

void Enemy::DetectionRadius()
{
	SDL_FPoint ePoint;
	ePoint.x = m_dst.x;
	ePoint.y = m_dst.y;

	SDL_FRect pPoint;
	pPoint.x = ((GameState*)(STMA::GetStates().back()))->getPlayer()->GetDstP()->x;
	pPoint.y = ((GameState*)(STMA::GetStates().back()))->getPlayer()->GetDstP()->y;
	pPoint.w = ((GameState*)(STMA::GetStates().back()))->getPlayer()->GetDstP()->w;
	pPoint.h = ((GameState*)(STMA::GetStates().back()))->getPlayer()->GetDstP()->h;

	if (COMA::CircleAABBCheck(ePoint, 100, pPoint) == true)
	{
		std::cout << "Player is within the circle!" << std::endl;
	}
}



void Enemy::SetState(int s)
{
	m_state = static_cast<state>(s);
	m_frame = 0;
	if (m_state == idle)
	{
		m_sprite = m_spriteMin = m_spriteMax = 0;
	}
}