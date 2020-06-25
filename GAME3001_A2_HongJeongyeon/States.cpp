#include "States.h"
#include "StateManager.h" // Make sure this is NOT in "States.h" or circular reference.
#include "Engine.h"
#include "EventManager.h"
#include "SoundManager.h"
#include "TextureManager.h"
#include "MathManager.h"
#include "PathManager.h"
#include "DebugManager.h"
#include "Label.h"
#include "Player.h"
#include <string>
#include <iostream>
#include <fstream>
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
	//SOMA::PlayMusic("bground", -1, 0);
	m_explanation = new Label("ltype", 0, 0, "press H for Debug View / left-click for selecting starting Tile / right-click for selecting Goal Tile / F for finding Shortest Path / M for Moving", col);
	m_pPlayer = new Player({ 0,0,32,32 }, { (float)(16) * 32, (float)(12) * 32, 32, 32 }, Engine::Instance().GetRenderer(), TEMA::GetTexture("player"), 0, 0, 0, 4);
	m_pBling = new Sprite({ 224,64,32,32 }, { (float)(16) * 32, (float)(4) * 32, 32, 32 }, Engine::Instance().GetRenderer(), TEMA::GetTexture("bground"));

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
}

void GameState::Update()
{
	m_pPlayer->Update(); // Just stops MagaMan from moving.
	if (EVMA::KeyPressed(SDL_SCANCODE_GRAVE)) // ~ or ` key. Toggle debug mode.
		m_showCosts = !m_showCosts;
	if (EVMA::KeyPressed(SDL_SCANCODE_SPACE)) // Toggle the heuristic used for pathfinding.
	{
		m_hEuclid = !m_hEuclid;
		std::cout << "Setting " << (m_hEuclid ? "Euclidian" : "Manhattan") << " heuristic..." << std::endl;
	}
	if (EVMA::MousePressed(1) || EVMA::MousePressed(3)) // If user has clicked.
	{
		if (m_bDebug == true)
		{
			int xIdx = (EVMA::GetMousePos().x / 32);
			int yIdx = (EVMA::GetMousePos().y / 32);
			if (m_level[yIdx][xIdx]->IsObstacle() || m_level[yIdx][xIdx]->IsHazard()) // Node() == nullptr;
				return; // We clicked on an invalid tile.

			if (EVMA::MousePressed(1))
			{
				m_iDebugStartColIndex = xIdx;
				m_iDebugStartRowIndex = yIdx;
			}
			else if (EVMA::MousePressed(3))
			{
				m_iDebugGoalColIndex = xIdx;
				m_iDebugGoalRowIndex = yIdx;
				if (m_iDebugStartRowIndex != -1 && m_iDebugStartColIndex != -1)
				{
					//Now we can calculate the path. Note: I am not returning a path again, only generating one to be rendered.
					PAMA::GetShortestPath(m_level[m_iDebugStartRowIndex][m_iDebugStartColIndex]->Node(),
						m_level[m_iDebugGoalRowIndex][m_iDebugGoalColIndex]->Node());
				}
			}
		}
		else
		{
			if (m_pPlayer->getIsPlayerMoved() == false)
			{
				int xIdx = (EVMA::GetMousePos().x / 32);
				int yIdx = (EVMA::GetMousePos().y / 32);
				if (m_level[yIdx][xIdx]->IsObstacle() || m_level[yIdx][xIdx]->IsHazard()) // Node() == nullptr;
					return; // We clicked on an invalid tile.
				if (EVMA::MousePressed(1)) // Move the player with left-click.
				{
					if (m_pBling->GetDstP()->x == (float)(xIdx * 32) && m_pBling->GetDstP()->y == (float)(yIdx * 32))
					{
						return;
					}
					m_pPlayer->GetDstP()->x = (float)(xIdx * 32);
					m_pPlayer->GetDstP()->y = (float)(yIdx * 32);
					PAMA::ClearEuclid_ManhatPath();
				}
				else if (EVMA::MousePressed(3)) // Else move the bling with right-click.
				{
					if (m_pPlayer->GetDstP()->x == (float)(xIdx * 32) && m_pPlayer->GetDstP()->y == (float)(yIdx * 32))
					{
						return;
					}
					m_pBling->GetDstP()->x = (float)(xIdx * 32);
					m_pBling->GetDstP()->y = (float)(yIdx * 32);
					PAMA::ClearEuclid_ManhatPath();
				}
				for (int row = 0; row < ROWS; row++) // "This is where the fun begins."
				{ // Update each node with the selected heuristic and set the text for debug mode.
					for (int col = 0; col < COLS; col++)
					{
						if (m_level[row][col]->Node() == nullptr)
							continue;
						if (m_hEuclid)
							m_level[row][col]->Node()->SetH(PAMA::HEuclid(m_level[row][col]->Node(), m_level[(int)(m_pBling->GetDstP()->y / 32)][(int)(m_pBling->GetDstP()->x / 32)]->Node()));
						else
							m_level[row][col]->Node()->SetH(PAMA::HManhat(m_level[row][col]->Node(), m_level[(int)(m_pBling->GetDstP()->y / 32)][(int)(m_pBling->GetDstP()->x / 32)]->Node()));
						m_level[row][col]->m_lCost->SetText(to_string((int)(m_level[row][col]->Node()->H())).c_str());
					}
				}
				//Now we can calculate the path. Note: I am not returning a path again, only generating one to be rendered.
				PAMA::GetShortestPath(m_level[(int)(m_pPlayer->GetDstP()->y / 32)][(int)(m_pPlayer->GetDstP()->x / 32)]->Node(),
					m_level[(int)(m_pBling->GetDstP()->y / 32)][(int)(m_pBling->GetDstP()->x / 32)]->Node());
			}
		}

	}
	if (EVMA::KeyPressed(SDL_SCANCODE_H))
	{
		if (m_bDebug == false)
		{
			m_bDebug = true;
		}
		else
		{
			m_bDebug = false;
			PAMA::ClearPath();
		}
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
		PAMA::GetShortestPath(m_level[(int)(m_pPlayer->GetDstP()->y / 32)][(int)(m_pPlayer->GetDstP()->x / 32)]->Node(),
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
		PAMA::GetShortestPath(m_level[(int)(m_pPlayer->GetDstP()->y / 32)][(int)(m_pPlayer->GetDstP()->x / 32)]->Node(),
			m_level[(int)(m_pBling->GetDstP()->y / 32)][(int)(m_pBling->GetDstP()->x / 32)]->Node());
	}
	if (EVMA::KeyPressed(SDL_SCANCODE_M) && m_bDebug == false)
	{
		m_pPlayer->setIsPlayerMoved(true);
	}

}

void GameState::CheckCollision()
{

}

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
				m_level[row][col]->m_lCost->Render();
				SDL_RenderDrawRectF(Engine::Instance().GetRenderer(), m_level[row][col]->GetDstP());
				m_level[row][col]->m_lX->Render();
				m_level[row][col]->m_lY->Render();
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
	m_explanation->Render();
	m_pPlayer->Render();
	m_pBling->Render();
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
	m_Label = new Label("ltype", 0, 0, "Jeongyeon Hong 101198653", col);
	m_playBtn = new PlayButton({ 0,0,400,100 }, { 312.0f,400.0f,400.0f,100.0f }, Engine::Instance().GetRenderer(), TEMA::GetTexture("play"));

}

void TitleState::Update()
{
	if (m_playBtn->Update() == 1)
		return;
	//if (m_quitBtn->Update() == 1)
		//return;
}

void TitleState::Render()
{
	m_Label->Render();
	m_playBtn->Render();
	//m_quitBtn->Render();
}

void TitleState::Exit()
{
	delete m_Label;
	delete m_playBtn;
	std::cout << "Exiting TitleState..." << endl;
}
// End TitleState.
