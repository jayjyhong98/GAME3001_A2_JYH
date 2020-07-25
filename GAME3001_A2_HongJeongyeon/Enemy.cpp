#include "Enemy.h"
#include "Player.h"
#include "Projectile.h"
#include "HealthBar.h"
#include "Engine.h"
#include "TextureManager.h"
#include "CollisionManager.h"
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
}

void Enemy::Update()
{
	if (m_bBeAlived == true)
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
				m_health -= 20;
				delete projectile[i];
				projectile.erase(iterBegin);
				break;
			}
		}
		if (m_health <= 0)
		{
			((GameState*)(STMA::GetStates().back()))->setEnemyDeadCount(((GameState*)(STMA::GetStates().back()))->getEnemyDeadCount() + 1);
			((GameState*)(STMA::GetStates().back()))->setEnemyCount(((GameState*)(STMA::GetStates().back()))->getEnemyCount() - 1);
			m_bBeAlived = false;
		}

		//if (((GameState*)(STMA::GetStates().back()))->getPatrol() == true)
		//{
		//	if (m_bHasFoundPass == false)
		//	{
		//		int xIdx = (m_dst.x + (m_dst.w / 2) / 32);
		//		int yIdx = (m_dst.y + (m_dst.h / 2) / 32);
		//		std::array<std::array<Tile*, COLS>, ROWS>& m_vLevel = ((GameState*)(STMA::GetStates().back()))->GetLevel();

		//		// 전체 위치의 가로
		//		for (int i = -2; i < 3; ++i)
		//		{
		//			if (i == 0)
		//			{
		//				continue;
		//			}

		//			if (m_vLevel[xIdx + i][yIdx]->IsObstacle() == false && m_vLevel[xIdx + i][yIdx]->IsHazard() == false)
		//			{
		//				m_vPatrolTile.push_back(m_vLevel[xIdx - i][yIdx]);
		//			}
		//		}

		//		// 서있는 위치의 왼쪽 세로
		//		for (int i = -1; i < 2; ++i)
		//		{
		//			if (i == 0)
		//			{
		//				continue;
		//			}

		//			if (m_vLevel[xIdx - 1][yIdx + i]->IsObstacle() == false && m_vLevel[xIdx - 1][yIdx + i]->IsHazard() == false)
		//			{
		//				m_vPatrolTile.push_back(m_vLevel[xIdx - 1][yIdx + i]);
		//			}
		//		}

		//		// 자기 자신의 세로
		//		for (int i = -1; i < 2; ++i)
		//		{
		//			if (i == 0)
		//			{
		//				continue;
		//			}

		//			if (m_vLevel[xIdx][yIdx + i]->IsObstacle() == false && m_vLevel[xIdx][yIdx + i]->IsHazard() == false)
		//			{
		//				m_vPatrolTile.push_back(m_vLevel[xIdx][yIdx + i]);
		//			}
		//		}

		//		// move to the right
		//		for (int i = -1; i < 2; ++i)
		//		{
		//			if (i == 0)
		//			{
		//				continue;
		//			}

		//			if (m_vLevel[xIdx + 1][yIdx + i]->IsObstacle() == false && m_vLevel[xIdx + 1][yIdx + i]->IsHazard() == false)
		//			{
		//				m_vPatrolTile.push_back(m_vLevel[xIdx + 1][yIdx + i]);
		//			}
		//		}
		//		srand(time(NULL));
		//		float level;
		//		level = rand() % m_vPatrolTile.size();

		//		if (m_vLevel[yIdx][xIdx]->IsObstacle() || m_vLevel[yIdx][xIdx]->IsHazard()) // Node() == nullptr;
		//			return; // We clicked on an invalid tile.
		//		PAMA::GetShortestPath(m_vLevel[yIdx][xIdx], m_vLevel[level][level], m_vEnemyPath);
		//	}
		//}
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

//void Enemy::MoveAlongPath()
//{
//
//	m_iDebugStartColIndex = xIdx;
//	m_iDebugStartRowIndex = yIdx;
//
//	m_iDebugGoalColIndex = ;
//	m_iDebugGoalRowIndex = ;
//
//	if (m_iDebugStartRowIndex != -1 && m_iDebugStartColIndex != -1)
//	{
//		//Now we can calculate the path. Note: I am not returning a path again, only generating one to be rendered.
//		PAMA::GetShortestPath(m_level[m_iDebugStartRowIndex][m_iDebugStartColIndex]->Node(),
//			m_level[m_iDebugGoalRowIndex][m_iDebugGoalColIndex]->Node());
//	}
//
//
//
//	if (m_bReadyToMove == true)
//	{
//		m_bReadyToMove = false;
//		NextNodeIdx++;
//		if (NextNodeIdx < PAMA::getPath().size())
//		{
//			NextDstX = PAMA::getPath()[NextNodeIdx]->GetToNode()->x + 16;
//			NextDstY = PAMA::getPath()[NextNodeIdx]->GetToNode()->y + 16;
//
//			movingVelocityX = (NextDstX - (m_dst.x + (m_dst.w / 2)));
//			movingVelocityY = (NextDstY - (m_dst.y + (m_dst.h / 2)));
//
//			Vector2D vector(movingVelocityX, movingVelocityY);
//			movingVelocityX = movingVelocityX / vector.length();
//			movingVelocityY = movingVelocityY / vector.length();
//		}
//		else
//		{
//			m_bIsEnemyMoved = false;
//			m_bReadyToMove = true;
//			NextNodeIdx = -1;
//			PAMA::ClearPath();
//			return;
//		}
//	}
//	m_dst.x = m_dst.x + movingVelocityX * SPEED;
//	m_dst.y = m_dst.y + movingVelocityY * SPEED;
//
//	float PlayerDstX = (m_dst.x + (m_dst.w / 2));
//	float PlayerDstY = (m_dst.y + (m_dst.h / 2));
//
//	if (abs(PlayerDstX - NextDstX) <= 0.1 && abs(PlayerDstY - NextDstY) <= 0.1)
//	{
//		m_bReadyToMove = true;
//	}
//}

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