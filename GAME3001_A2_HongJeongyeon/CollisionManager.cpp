#include "StateManager.h"
#include "States.h"
#include "CollisionManager.h"
#include "DebugManager.h"
#include "Engine.h"
//#include "GLM/vec2.hpp"

bool CollisionManager::AABBCheck(const SDL_FRect& object1, const SDL_FRect& object2)
{
	SDL_Rect temp1 = MAMA::ConvertFRect2Rect(object1);
	SDL_Rect temp2 = MAMA::ConvertFRect2Rect(object2);
	return SDL_HasIntersection(&temp1, &temp2);
}

bool CollisionManager::CircleCircleCheck(const SDL_FPoint object1, const SDL_FPoint object2, const double r1, const double r2)
{
	return (MAMA::Distance((double)object1.x, (double)object2.x, (double)object1.y, (double)object2.y) < (r1 + r2));
}

bool CollisionManager::CircleAABBCheck(const SDL_FPoint object1, const double r, const SDL_FRect& object2)
{
	double x1 = (double)object1.x;
	double y1 = (double)object1.y;
	if (object1.x < object2.x) // Circle center, rect left edge.
		x1 = (double)object2.x;
	else if (object1.x > object2.x + object2.w)
		x1 = (double)object2.x + (double)object2.w;
	if (object1.y < object2.y)
		y1 = (double)object2.y;
	else if (object1.y > object2.y + object2.h)
		y1 = (double)object2.y + (double)object2.h;

	return CircleCircleCheck({ (float)x1, (float)y1 }, { (float)object1.x, (float)object1.y }, r);
}

bool CollisionManager::LinePointCheck(const SDL_FPoint object1_start, const SDL_FPoint object1_end, const SDL_FPoint object2)
{
	double distToStart = MAMA::Distance((double)object1_start.x, (double)object2.x, (double)object1_start.y, (double)object2.y);
	double distToEnd = MAMA::Distance((double)object1_end.x, (double)object2.x, (double)object1_end.y, (double)object2.y);

	double lineLength = MAMA::Distance((double)object1_start.x, (double)object1_end.x, (double)object1_start.y, (double)object1_end.y);

	double buffer = 0.2; // Extra distance since line thickness is one pixel.

	if (distToStart + distToEnd <= lineLength + buffer)
		return true;
	return false;
}

bool CollisionManager::PlayerCollision(const SDL_Rect player, const int dX, const int dY)
{
	int playerX = player.x / 32;
	int playerY = player.y / 32;
	SDL_Rect p = { player.x + dX + 8 , player.y + dY + 6, player.w - 16, player.h - 10 }; // Adjusted bounding box.
	Tile* tiles[4] = { ((GameState*)(StateManager::GetStates().back()))->GetLevel()[playerY][playerX],																				// Player's tile.
					   ((GameState*)(StateManager::GetStates().back()))->GetLevel()[playerY][(playerX + 1 == COLS ? COLS - 1 : playerX + 1)],										// Right tile.
					   ((GameState*)(StateManager::GetStates().back()))->GetLevel()[(playerY + 1 == ROWS ? ROWS - 1 : playerY + 1)][(playerX + 1 == COLS ? COLS - 1 : playerX + 1)],	// Bottom-Right tile.
					   ((GameState*)(StateManager::GetStates().back()))->GetLevel()[(playerY + 1 == ROWS ? ROWS - 1 : playerY + 1)][playerX] };										// Bottom tile.
	for (int i = 0; i < 4; i++)
	{
		SDL_Rect t = MAMA::ConvertFRect2Rect(*(tiles[i]->GetDstP()));
		if (tiles[i]->IsObstacle() && SDL_HasIntersection(&p, &t))
		{ // Collision!
			return true;
			// Other potential code...
		}
	}
	return false;
}

bool CollisionManager::lineLineCheck(const glm::vec2 line1_start, const glm::vec2 line1_end, const glm::vec2 line2_start, const glm::vec2 line2_end)
{
	const auto x1 = line1_start.x;
	const auto x2 = line1_end.x;
	const auto x3 = line2_start.x;
	const auto x4 = line2_end.x;
	const auto y1 = line1_start.y;
	const auto y2 = line1_end.y;
	const auto y3 = line2_start.y;
	const auto y4 = line2_end.y;

	// calculate the distance to intersection point
	const auto uA = ((x4 - x3) * (y1 - y3) - (y4 - y3) * (x1 - x3)) / ((y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1));
	const auto uB = ((x2 - x1) * (y1 - y3) - (y2 - y1) * (x1 - x3)) / ((y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1));

	// if uA and uB are between 0-1, lines are colliding
	if (uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1)
	{
		return true;
	}

	return false;
}

bool CollisionManager::lineRectCheck(const glm::vec2 line1_start, const glm::vec2 line1_end, const glm::vec2 rec_start, const float rect_width, const float rect_height)
{
	const auto x1 = line1_start.x;
	const auto x2 = line1_end.x;
	const auto y1 = line1_start.y;
	const auto y2 = line1_end.y;
	const auto rx = rec_start.x;
	const auto ry = rec_start.y;
	const auto rw = rect_width;
	const auto rh = rect_height;

	// check if the line has hit any of the rectangle's sides
	// uses the Line/Line function below
	const auto left = lineLineCheck(glm::vec2(x1, y1), glm::vec2(x2, y2), glm::vec2(rx, ry), glm::vec2(rx, ry + rh));
	const auto right = lineLineCheck(glm::vec2(x1, y1), glm::vec2(x2, y2), glm::vec2(rx + rw, ry), glm::vec2(rx + rw, ry + rh));
	const auto top = lineLineCheck(glm::vec2(x1, y1), glm::vec2(x2, y2), glm::vec2(rx, ry), glm::vec2(rx + rw, ry));
	const auto bottom = lineLineCheck(glm::vec2(x1, y1), glm::vec2(x2, y2), glm::vec2(rx, ry + rh), glm::vec2(rx + rw, ry + rh));

	// if ANY of the above are true, the line
	// has hit the rectangle
	if (left || right || top || bottom) {
		return true;
	}

	return false;
}

bool CollisionManager::LOSCheck(AnimatedSprite* from, AnimatedSprite* to, Sprite* obstacle)
{
	const auto lineStart = glm::vec2(from->GetDstP()->x + (from->GetDstP()->w / 2), from->GetDstP()->y + (from->GetDstP()->h / 2));
	const auto lineEnd = glm::vec2(to->GetDstP()->x + (to->GetDstP()->w / 2), to->GetDstP()->y + (to->GetDstP()->h / 2));
	// aabb
	const auto boxWidth = obstacle->GetDstP()->w;
	//const int halfBoxWidth = boxWidth * 0.5f;
	const auto boxHeight = obstacle->GetDstP()->h;
	//const int halfBoxHeight = boxHeight * 0.5f;
	const auto boxStart = glm::vec2(obstacle->GetDstP()->x, obstacle->GetDstP()->y);

	if (lineRectCheck(lineStart, lineEnd, boxStart, boxWidth, boxHeight))
	{
		std::cout << "No LOS - Collision with Obstacle!" << std::endl;

		return true;
	}

	return false;
}