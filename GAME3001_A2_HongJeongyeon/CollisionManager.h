#pragma once
#ifndef _COLLISIONMANAGER_H_
#define _COLLISIONMANAGER_H_

#include <iostream>
#include <SDL.h>
#include "MathManager.h"
#include <GLM/gtx/norm.hpp>

class CollisionManager
{
public:
	static bool AABBCheck(const SDL_FRect& object1, const SDL_FRect& object2);

	static bool CircleCircleCheck(const SDL_FPoint object1, const SDL_FPoint object2, const double r1, const double r2 = 0.0);
	static bool CircleAABBCheck(const SDL_FPoint object1, const double r, const SDL_FRect& object2);

	static bool LinePointCheck(const SDL_FPoint object1_start, const SDL_FPoint object1_end, const SDL_FPoint object2);

	static bool PlayerCollision(const SDL_Rect player, const int dX, const int dY);
	static bool lineLineCheck(const glm::vec2 line1_start, const glm::vec2 line1_end, const glm::vec2 line2_start, const glm::vec2 line2_end);
	static bool lineRectCheck(glm::vec2 line1_start, glm::vec2 line1_end, glm::vec2 rec_start, float rect_width, float rect_height);
	static bool LOSCheck(AnimatedSprite* from, AnimatedSprite* to, Sprite* obstacle);
private:
	CollisionManager() {}
};

typedef CollisionManager COMA;

#endif