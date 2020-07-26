#include "States.h"
#include "StateManager.h" // Make sure this is NOT in "States.h" or circular reference.
#include "Engine.h"
#include "EventManager.h"
#include "SoundManager.h"
#include "TextureManager.h"
#include "MathManager.h"
#include "PathManager.h"
#include "DebugManager.h"
#include "CollisionManager.h"
#include "Label.h"
#include "Player.h"
#include "Enemy.h"
#include "Util.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

// Begin State. CTRL+M+H and CTRL+M+U to turn on/off collapsed code.
void State::Render()
{
	SDL_RenderPresent(Engine::Instance().GetRenderer());
}
void State::Resume() {}
// End State.

void GameState::Enter()
{
	SDL_Color col;
	col.r = 255;
	col.g = 255;
	col.b = 255;
	col.a = 255;
	SOMA::PlayMusic("bground", -1, 0);
	SOMA::SetMusicVolume(5);
	ifstream inFile("Dat/Tiledata.txt");
	if (inFile.is_open())
	{ // Create map of Tile prototypes.
		char key;
		int x, y;
		bool o, h;
		int cost;
		while (!inFile.eof())
		{
			inFile >> key >> x >> y >> o >> h >> cost;
			m_tiles.emplace(key, new Tile({ x * 32, y * 32, 32, 32 }, { 0,0,32,32 }, Engine::Instance().GetRenderer(), TEMA::GetTexture("bground"), o, h, cost));
		}
	}
	inFile.close();
	inFile.open("Dat/Level1.txt");
	if (inFile.is_open())
	{ // Build the level from Tile prototypes.
		char key;
		for (int row = 0; row < ROWS; row++)
		{
			for (int col = 0; col < COLS; col++)
			{
				inFile >> key;
				m_level[row][col] = m_tiles[key]->Clone(); // Prototype design pattern used.
				m_level[row][col]->GetDstP()->x = (float)(32 * col);
				m_level[row][col]->GetDstP()->y = (float)(32 * row);
				m_level[row][col]->setTileRowIdx(row);
				m_level[row][col]->setTileColIdx(col);
				// Instantiate the labels for each tile.
				string str = to_string(m_level[row][col]->getTileCost());
				m_level[row][col]->m_lCost = new Label("ltype", m_level[row][col]->GetDstP()->x + 4, m_level[row][col]->GetDstP()->y + 18, str.c_str(), { 0,0,0,255 });
				m_level[row][col]->m_lX = new Label("ltype", m_level[row][col]->GetDstP()->x + 18, m_level[row][col]->GetDstP()->y + 2, to_string(col).c_str(), { 0,0,0,255 });
				m_level[row][col]->m_lY = new Label("ltype", m_level[row][col]->GetDstP()->x + 2, m_level[row][col]->GetDstP()->y + 2, to_string(row).c_str(), { 0,0,0,255 });
				// Construct the Node for a valid tile.
				if (!m_level[row][col]->IsObstacle() && !m_level[row][col]->IsHazard())
					m_level[row][col]->m_node = new PathNode((int)(m_level[row][col]->GetDstP()->x), (int)(m_level[row][col]->GetDstP()->y));

				if (m_level[row][col]->IsObstacle() == true)
				{
					m_vObstacles.push_back(m_level[row][col]);
				}
			}
		}
	}
	inFile.close();
	// Now build the graph from ALL the non-obstacle and non-hazard tiles. Only N-E-W-S compass points.
	for (int row = 0; row < ROWS; row++)
	{
		for (int col = 0; col < COLS; col++)
		{
			if (m_level[row][col]->Node() == nullptr) // Now we can test for nullptr.
				continue; // An obstacle or hazard tile has no connections.
			// Make valid connections. Inside map and a valid tile.
			if (row - 1 != -1 && m_level[row - 1][col]->Node() != nullptr) // Tile above. 
				m_level[row][col]->Node()->AddConnection(new PathConnection(m_level[row][col]->Node(), m_level[row - 1][col]->Node(),
					MAMA::Distance(m_level[row][col]->Node()->x, m_level[row - 1][col]->Node()->x, m_level[row][col]->Node()->y, m_level[row - 1][col]->Node()->y)));
			if (row + 1 != ROWS && m_level[row + 1][col]->Node() != nullptr) // Tile below.
				m_level[row][col]->Node()->AddConnection(new PathConnection(m_level[row][col]->Node(), m_level[row + 1][col]->Node(),
					MAMA::Distance(m_level[row][col]->Node()->x, m_level[row + 1][col]->Node()->x, m_level[row][col]->Node()->y, m_level[row + 1][col]->Node()->y)));
			if (col - 1 != -1 && m_level[row][col - 1]->Node() != nullptr) // Tile to Left.
				m_level[row][col]->Node()->AddConnection(new PathConnection(m_level[row][col]->Node(), m_level[row][col - 1]->Node(),
					MAMA::Distance(m_level[row][col]->Node()->x, m_level[row][col - 1]->Node()->x, m_level[row][col]->Node()->y, m_level[row][col - 1]->Node()->y)));
			if (col + 1 != COLS && m_level[row][col + 1]->Node() != nullptr) // Tile to right.
				m_level[row][col]->Node()->AddConnection(new PathConnection(m_level[row][col]->Node(), m_level[row][col + 1]->Node(),
					MAMA::Distance(m_level[row][col]->Node()->x, m_level[row][col + 1]->Node()->x, m_level[row][col]->Node()->y, m_level[row][col + 1]->Node()->y)));
		}
	}
	//m_explanation = new Label("ltype", 0, 0, "press H for Debug View / left-click for selecting starting Tile / right-click for selecting Goal Tile / F for finding Shortest Path / M for Moving", col);
	m_pPlayer = new Player({ 0,0,567,556 }, { (float)(16) * 32, (float)(12) * 32, 32, 32 }, Engine::Instance().GetRenderer(), TEMA::GetTexture("player1"), 0, 0, 0, 4);
	//m_pBling = new Sprite({ 224,64,32,32 }, { (float)(16) * 32, (float)(4) * 32, 32, 32 }, Engine::Instance().GetRenderer(), TEMA::GetTexture("bground"));
	//m_pEnemy
	s_enemies.push_back(new Enemy({ 0,0,430,519 }, { (float)(16) * 32, (float)(4) * 32, 32, 32 }, Engine::Instance().GetRenderer(), TEMA::GetTexture("enemy"), 0, 0, 0, 12));
	s_enemies.push_back(new Enemy({ 0,0,430,519 }, { (float)(5) * 32, (float)(12) * 32, 32, 32 }, Engine::Instance().GetRenderer(), TEMA::GetTexture("enemy"), 0, 0, 0, 12));
	s_enemies.push_back(new Enemy({ 0,0,430,519 }, { (float)(25) * 32, (float)(9) * 32, 32, 32 }, Engine::Instance().GetRenderer(), TEMA::GetTexture("enemy"), 0, 0, 0, 12));
	s_enemies.push_back(new Enemy({ 0,0,430,519 }, { (float)(17) * 32, (float)(15) * 32, 32, 32 }, Engine::Instance().GetRenderer(), TEMA::GetTexture("enemy"), 0, 0, 0, 12));
	s_enemies.push_back(new Enemy({ 0,0,430,519 }, { (float)(13) * 32, (float)(20) * 32, 32, 32 }, Engine::Instance().GetRenderer(), TEMA::GetTexture("enemy"), 0, 0, 0, 12));

	m_iEnemyCount = s_enemies.size();

	//Enemy UI
	m_pEnemyDeadUI = new Sprite({ 0, 0, 629, 526 }, { 70, 10, 60, 60 }, Engine::Instance().GetRenderer(), TEMA::GetTexture("deadenemycount"));
	m_pEnemyAliveUI = new Sprite({ 0, 0, 430, 519 }, { 0, 20, 40, 40 }, Engine::Instance().GetRenderer(), TEMA::GetTexture("enemycount"));
	words[0] = new Label("ltype", 40, 30, to_string((int)(m_iEnemyCount)).c_str(), { 255,255,255,0 });
	words[1] = new Label("ltype", 140, 30, to_string((int)(m_iEnemyDeadCount)).c_str(), { 255,255,255,0 });
	
}

void GameState::Update()
{
	m_pPlayer->Update(); // Just stops MagaMan from moving.

	for (int i = 0; i < s_enemies.size(); ++i)
	{
		s_enemies[i]->Update();
	}


	words[0]->SetText(to_string((int)(m_iEnemyCount)).c_str());
	words[1]->SetText(to_string((int)(m_iEnemyDeadCount)).c_str());


	if (EVMA::KeyPressed(SDL_SCANCODE_GRAVE)) // ~ or ` key. Toggle debug mode.
		m_showCosts = !m_showCosts;
	if (EVMA::KeyPressed(SDL_SCANCODE_SPACE)) // Toggle the heuristic used for pathfinding.
	{
		m_hEuclid = !m_hEuclid;
		std::cout << "Setting " << (m_hEuclid ? "Euclidian" : "Manhattan") << " heuristic..." << std::endl;
	}
	//if (EVMA::MousePressed(1)) // If user has clicked.
	//{
	//	m_pPlayer->Melee();
	//	m_pPlayer->ChangeState(Player::melee);
	//	SOMA::PlaySound("beep");
		//if (m_bDebug == true)
		//{
		//	int xIdx = (EVMA::GetMousePos().x / 32);
		//	int yIdx = (EVMA::GetMousePos().y / 32);
		//	if (m_level[yIdx][xIdx]->IsObstacle() || m_level[yIdx][xIdx]->IsHazard()) // Node() == nullptr;
		//		return; // We clicked on an invalid tile.

		//	if (EVMA::MousePressed(1))
		//	{
		//		//close-combat
		//		m_pPlayer->Melee();
		//		
		//		SOMA::PlaySound("beep");

		//		/*m_iDebugStartColIndex = xIdx;
		//		m_iDebugStartRowIndex = yIdx;
		//		if (m_pBling->GetDstP()->x == (float)(xIdx * 32) && m_pBling->GetDstP()->y == (float)(yIdx * 32))
		//		{
		//			return;
		//		}
		//		m_pPlayer->GetDstP()->x = (float)(xIdx * 32);
		//		m_pPlayer->GetDstP()->y = (float)(yIdx * 32);*/

		//		
		//	}
		//	else if (EVMA::MousePressed(3))
		//	{
		//		//¿ø°Å¸® ½´ÆÃ
		//		
		//		SOMA::PlaySound("beep");
		//		m_iDebugGoalColIndex = xIdx;
		//		m_iDebugGoalRowIndex = yIdx;
		//		if (m_iDebugStartRowIndex != -1 && m_iDebugStartColIndex != -1)
		//		{
		//			//Now we can calculate the path. Note: I am not returning a path again, only generating one to be rendered.
		//			PAMA::GetShortestPath(m_level[m_iDebugStartRowIndex][m_iDebugStartColIndex]->Node(),
		//				m_level[m_iDebugGoalRowIndex][m_iDebugGoalColIndex]->Node());
		//		}
		//		if (m_pPlayer->GetDstP()->x == (float)(xIdx * 32) && m_pPlayer->GetDstP()->y == (float)(yIdx * 32))
		//		{
		//			return;
		//		}
		//		/*m_pBling->GetDstP()->x = (float)(xIdx * 32);
		//		m_pBling->GetDstP()->y = (float)(yIdx * 32);*/
		//	}
		//}
		//else
		//{
		//	if (m_pPlayer->getIsPlayerMoved() == false)
		//	{
		//		int xIdx = (EVMA::GetMousePos().x / 32);
		//		int yIdx = (EVMA::GetMousePos().y / 32);
		//		if (m_level[yIdx][xIdx]->IsObstacle() || m_level[yIdx][xIdx]->IsHazard()) // Node() == nullptr;
		//			return; // We clicked on an invalid tile.
		//		if (EVMA::MousePressed(1)) // Move the player with left-click.
		//		{
		//			/*if (m_pBling->GetDstP()->x == (float)(xIdx * 32) && m_pBling->GetDstP()->y == (float)(yIdx * 32))
		//			{
		//				return;
		//			}*/
		//			/*m_pPlayer->GetDstP()->x = (float)(xIdx * 32);
		//			m_pPlayer->GetDstP()->y = (float)(yIdx * 32);*/
		//			//PAMA::ClearEuclid_ManhatPath();
		//		}
		//		else if (EVMA::MousePressed(3)) // Else move the bling with right-click.
		//		{
		//			//	if (m_pPlayer->GetDstP()->x == (float)(xIdx * 32) && m_pPlayer->GetDstP()->y == (float)(yIdx * 32))
		//			//	{
		//			//		return;
		//			//	}
		//			//	/*m_pBling->GetDstP()->x = (float)(xIdx * 32);
		//			//	m_pBling->GetDstP()->y = (float)(yIdx * 32);*/
		//			//	//PAMA::ClearEuclid_ManhatPath();
		//			//}
		//			//for (int row = 0; row < ROWS; row++) // "This is where the fun begins."
		//			//{ // Update each node with the selected heuristic and set the text for debug mode.
		//			//	for (int col = 0; col < COLS; col++)
		//			//	{
		//			//		if (m_level[row][col]->Node() == nullptr)
		//			//			continue;
		//			//		if (m_hEuclid)
		//			//			m_level[row][col]->Node()->SetH(PAMA::HEuclid(m_level[row][col]->Node(), m_level[(int)(m_pBling->GetDstP()->y / 32)][(int)(m_pBling->GetDstP()->x / 32)]->Node()));
		//			//		else
		//			//			m_level[row][col]->Node()->SetH(PAMA::HManhat(m_level[row][col]->Node(), m_level[(int)(m_pBling->GetDstP()->y / 32)][(int)(m_pBling->GetDstP()->x / 32)]->Node()));
		//			//		m_level[row][col]->m_lCost->SetText(to_string((int)(m_level[row][col]->Node()->H())).c_str());
		//			//	}
		//			//}
		//			////Now we can calculate the path. Note: I am not returning a path again, only generating one to be rendered.
		//			//PAMA::GetShortestPath(m_level[(int)(m_pPlayer->GetDstP()->y / 32)][(int)(m_pPlayer->GetDstP()->x / 32)]->Node(),
		//			//	m_level[(int)(m_pBling->GetDstP()->y / 32)][(int)(m_pBling->GetDstP()->x / 32)]->Node());
		//		}
		//	}
		//}

	//}
	/*if (EVMA::MousePressed(3))
	{
		m_pPlayer->ChangeState(Player::firing);
		m_pPlayer->createProjectile();
	}*/


	// H KEY section
	//if (!m_bDebugKeys[H_KEY])
	if (!m_bHpressed)
	{
		if (EVMA::KeyPressed(SDL_SCANCODE_H))
		{
			m_bDebug = !m_bDebug;

			m_bHpressed = true;

			if (m_bDebug)
			{
				std::cout << "Debug Mode On" << std::endl;
			}
			else if (!m_bDebug)
			{
				std::cout << "Debug Mode Off" << std::endl;
			}
		}

	}

	if (EVMA::KeyReleased(SDL_SCANCODE_H))
	{
		m_bHpressed = false;
	}

	// K KEY section
	if (!m_bKpressed)
	{
		if (EVMA::KeyPressed(SDL_SCANCODE_K))
		{
			SOMA::PlaySound("zombie", 1);
			for (int i = 0; i < s_enemies.size(); ++i)
			{
				s_enemies[i]->setEnemyHealth(s_enemies[i]->getEnemyHealth() - 10);
			}
			std::cout << "DEBUG: Enemy Takes Damage!!" << std::endl;
			m_bKpressed = true;
		}
	}


	if (EVMA::KeyReleased(SDL_SCANCODE_K))
	{
		m_bKpressed = false;
	}

	// P KEY section
	if (!m_bPpressed)
		//if (!m_bHpressed)
	{
		if (EVMA::KeyPressed(SDL_SCANCODE_P))
		{
			m_bPatrol = !m_bPatrol;

			m_bPpressed = true;

			if (m_bPatrol)
			{
				std::cout << "Debug Patrol Mode On" << std::endl;
				SOMA::PlaySound("walk", 1);
				for (int i = 0; i < s_enemies.size(); ++i)
				{
					s_enemies[i]->ChangeState(Enemy::patrol);
				}
			}
			else if (!m_bPatrol)
			{
				std::cout << "Debug Patrol Mode Off" << std::endl;
			}
		}

	}

	if (EVMA::KeyReleased(SDL_SCANCODE_P))
	{
		m_bPpressed = false;
	}


	//if (EVMA::KeyPressed(SDL_SCANCODE_F))
	//{
	//	SOMA::PlaySound("beep");
	//	for (int row = 0; row < ROWS; row++) // "This is where the fun begins."
	//	{ // Update each node with the selected heuristic and set the text for debug mode.
	//		for (int col = 0; col < COLS; col++)
	//		{
	//			if (m_level[row][col]->Node() == nullptr)
	//				continue;
	//			m_level[row][col]->Node()->SetH(PAMA::HEuclid(m_level[row][col]->Node(), m_level[(int)(m_pBling->GetDstP()->y / 32)][(int)(m_pBling->GetDstP()->x / 32)]->Node()));
	//			m_level[row][col]->m_lCost->SetText(to_string((int)(m_level[row][col]->Node()->H())).c_str());
	//		}
	//	}
	//	//Now we can calculate the path. Note: I am not returning a path again, only generating one to be rendered.
	//	PAMA::GetShortestPath(m_level[(int)(m_pPlayer->GetDstP()->y / 32)][(int)(m_pPlayer->GetDstP()->x / 32)]->Node(),
	//		m_level[(int)(m_pBling->GetDstP()->y / 32)][(int)(m_pBling->GetDstP()->x / 32)]->Node());

	//	for (int row = 0; row < ROWS; row++) // "This is where the fun begins."
	//	{ // Update each node with the selected heuristic and set the text for debug mode.
	//		for (int col = 0; col < COLS; col++)
	//		{
	//			if (m_level[row][col]->Node() == nullptr)
	//				continue;
	//			m_level[row][col]->Node()->SetH(PAMA::HManhat(m_level[row][col]->Node(), m_level[(int)(m_pBling->GetDstP()->y / 32)][(int)(m_pBling->GetDstP()->x / 32)]->Node()));
	//			m_level[row][col]->m_lCost->SetText(to_string((int)(m_level[row][col]->Node()->H())).c_str());
	//		}
	//	}
	//	//Now we can calculate the path. Note: I am not returning a path again, only generating one to be rendered.
	//	PAMA::GetShortestPath(m_level[(int)(m_pPlayer->GetDstP()->y / 32)][(int)(m_pPlayer->GetDstP()->x / 32)]->Node(),
	//		m_level[(int)(m_pBling->GetDstP()->y / 32)][(int)(m_pBling->GetDstP()->x / 32)]->Node());
	//}
	//if (EVMA::KeyPressed(SDL_SCANCODE_M) && m_bDebug == false)
	//{
	//	SOMA::PlaySound("beep");
	//	m_pPlayer->setIsPlayerMoved(true);
	//}

}

//void GameState::CheckCollision()
//{
//
//}

void GameState::Render()
{
	// Draw anew.
	for (int row = 0; row < ROWS; row++)
	{
		for (int col = 0; col < COLS; col++)
		{
			m_level[row][col]->Render(); // Render each tile.
			// Render the debug data...
			if (m_bDebug)
			{
				//m_level[row][col]->m_lCost->Render();
				SDL_RenderDrawRectF(Engine::Instance().GetRenderer(), m_level[row][col]->GetDstP());
				//m_level[row][col]->m_lX->Render();
				//m_level[row][col]->m_lY->Render();

				auto colour = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
				glm::vec2 level;
				level.x = m_level[row][col]->GetDstP()->x + (m_level[row][col]->GetDstP()->w / 2);
				level.y = m_level[row][col]->GetDstP()->y + (m_level[row][col]->GetDstP()->h / 2);
				Util::DrawRect(level, 5, 5, colour);
				//I am also rendering out each connection in blue. If this is a bit much for you, comment out the for loop below.

			   /*if (m_level[row][col]->Node() != nullptr)
			   {
				   for (unsigned i = 0; i < m_level[row][col]->Node()->GetConnections().size(); i++)
				   {
					   DEMA::QueueLine({ m_level[row][col]->Node()->GetConnections()[i]->GetFromNode()->x + 16, m_level[row][col]->Node()->GetConnections()[i]->GetFromNode()->y + 16 },
						   { m_level[row][col]->Node()->GetConnections()[i]->GetToNode()->x + 16, m_level[row][col]->Node()->GetConnections()[i]->GetToNode()->y + 16 }, { 0,0,255,255 });
				   }
			   }*/

			}

		}
	}
	//m_explanation->Render();
	m_pPlayer->Render();
	//m_pBling->Render();
	for (int i = 0; i < s_enemies.size(); ++i)
	{
		s_enemies[i]->Render();
	}
	m_pEnemyAliveUI->Render();
	m_pEnemyDeadUI->Render();
	for (int i = 0; i < 2; i++)
	{
		words[i]->Render();
	}
	if (EVMA::KeyPressed(SDL_SCANCODE_F))
	{
		for (int row = 0; row < ROWS; row++) // "This is where the fun begins."
		{ // Update each node with the selected heuristic and set the text for debug mode.
			for (int col = 0; col < COLS; col++)
			{
				if (m_level[row][col]->Node() == nullptr)
					continue;
				m_level[row][col]->Node()->SetH(PAMA::HEuclid(m_level[row][col]->Node(), m_level[(int)(m_pBling->GetDstP()->y / 32)][(int)(m_pBling->GetDstP()->x / 32)]->Node()));
				m_level[row][col]->m_lCost->SetText(to_string((int)(m_level[row][col]->Node()->H())).c_str());
			}
		}
		//Now we can calculate the path. Note: I am not returning a path again, only generating one to be rendered.
		PAMA::GetEuclidPath(m_level[(int)(m_pPlayer->GetDstP()->y / 32)][(int)(m_pPlayer->GetDstP()->x / 32)]->Node(),
			m_level[(int)(m_pBling->GetDstP()->y / 32)][(int)(m_pBling->GetDstP()->x / 32)]->Node());

		for (int row = 0; row < ROWS; row++) // "This is where the fun begins."
		{ // Update each node with the selected heuristic and set the text for debug mode.
			for (int col = 0; col < COLS; col++)
			{
				if (m_level[row][col]->Node() == nullptr)
					continue;
				m_level[row][col]->Node()->SetH(PAMA::HManhat(m_level[row][col]->Node(), m_level[(int)(m_pBling->GetDstP()->y / 32)][(int)(m_pBling->GetDstP()->x / 32)]->Node()));
				m_level[row][col]->m_lCost->SetText(to_string((int)(m_level[row][col]->Node()->H())).c_str());
			}
		}
		//Now we can calculate the path. Note: I am not returning a path again, only generating one to be rendered.
		PAMA::GetManhatPath(m_level[(int)(m_pPlayer->GetDstP()->y / 32)][(int)(m_pPlayer->GetDstP()->x / 32)]->Node(),
			m_level[(int)(m_pBling->GetDstP()->y / 32)][(int)(m_pBling->GetDstP()->x / 32)]->Node());
	}
	PAMA::DrawPath(); // I save the path in a static vector to be drawn here.
	PAMA::DrawEuclidPath();
	PAMA::DrawManhatPath();

	if (((GameState*)(STMA::GetStates().back()))->getDebug() == true)
	{
		for (int i = 0; i < s_enemies.size(); ++i)
		{
			SDL_RenderDrawLine(Engine::Instance().GetRenderer(), m_pPlayer->GetDstP()->x + (m_pPlayer->GetDstP()->w / 2), m_pPlayer->GetDstP()->y + (m_pPlayer->GetDstP()->h / 2), s_enemies[i]->GetDstP()->x + (m_pPlayer->GetDstP()->w / 2), s_enemies[i]->GetDstP()->y + (m_pPlayer->GetDstP()->h / 2));
		}
	}

	DEMA::FlushLines(); // And... render ALL the queued lines. Phew.
	// Flip buffers.
}

void GameState::Exit()
{
	// Example-specific cleaning.
	for (int row = 0; row < ROWS; row++)
	{
		for (int col = 0; col < COLS; col++)
		{
			delete m_level[row][col];
			m_level[row][col] = nullptr; // Wrangle your dangle.
		}
	}
	for (auto const& i : m_tiles)
		delete m_tiles[i.first];
	m_tiles.clear();
}

void GameState::Resume() { }
// End GameState.

// Begin TitleState.
void TitleState::Enter()
{
	SDL_Color col;
	col.r = 255;
	col.g = 255;
	col.b = 255;
	col.a = 255;
	m_Label = new Label("ltype", 0, 0, "Jeongyeon Hong(jayjyhonh98) 101198653", col);
	//m_lLabel = new Label("ltype", 0, 15, "Juan De Gouveia 101203253", col);
	m_playBtn = new PlayButton({ 0,0,400,100 }, { 312.0f,400.0f,400.0f,100.0f }, Engine::Instance().GetRenderer(), TEMA::GetTexture("play"));
	SOMA::PlaySound("beep");
}

void TitleState::Update()
{
	if (m_playBtn->Update() == 1)
		return;
}

void TitleState::Render()
{
	m_Label->Render();
	//m_lLabel->Render();
	m_playBtn->Render();
}

void TitleState::Exit()
{
	delete m_Label;
	//delete m_lLabel;
	delete m_playBtn;
	std::cout << "Exiting TitleState..." << endl;
}
// End TitleState.
