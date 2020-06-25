#include "Tile.h"
#include "StateManager.h"
#include "States.h"
#include <vector>

std::vector<Tile*> Tile::getNeighbour()
{
    //Make a set of row, col idx for neighbour(up, right, down, and left sides)
    int rowIdxArray[4] = { m_iTileRowIdx - 1, m_iTileRowIdx , m_iTileRowIdx + 1, m_iTileRowIdx };
    int colIdxArray[4] = { m_iTileColIdx, m_iTileColIdx + 1 , m_iTileColIdx, m_iTileColIdx - 1 };

    std::vector<Tile*> neighbours;


    //iterator through idx array to check neighbour
    for (int i = 0; i < 4; ++i)
    {
        int neighbourRowIdx = rowIdxArray[i];
        int neighbourColIdx = colIdxArray[i];


        //Check row and col bound
        if (neighbourColIdx < 0 || neighbourColIdx >= COLS ||
            neighbourRowIdx < 0 || neighbourRowIdx >= ROWS)
        {
            continue;
        }

        //Check if neighbor has player
        Tile* potentialNode = (((GameState*)(StateManager::GetStates().back()))->GetLevel()[neighbourRowIdx])[neighbourColIdx];
        if (potentialNode == nullptr)
        {
            continue;
        }

        //If node has actor.. check if it is player
        if (potentialNode->IsObstacle() != true)
        {
            neighbours.push_back(potentialNode);
        }
    }

    return neighbours;
}

void Tile::setTileRowIdx(int rowidx)
{
    m_iTileRowIdx = rowidx;

}

void Tile::setTileColIdx(int colidx)
{
    m_iTileColIdx = colidx;
}