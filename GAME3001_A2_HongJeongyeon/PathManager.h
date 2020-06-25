#pragma once
#ifndef _PATHMANAGER_H_
#define _PATHMANAGER_H_
#include "Pathing.h"
#include <vector>

class PathManager
{
public:
	static void GetShortestPath(PathNode* start, PathNode* goal);
	static void GetEuclidPath(PathNode* start, PathNode* goal);
	static void GetManhatPath(PathNode* start, PathNode* goal);
	static NodeRecord* GetSmallestNode();
	static std::vector<NodeRecord*>& OpenList();
	static std::vector<NodeRecord*>& ClosedList();
	static bool ContainsNode(std::vector<NodeRecord*>& list, PathNode* n);
	static NodeRecord* GetNodeRecord(std::vector<NodeRecord*>& list, PathNode* n);
	static double HEuclid(const PathNode* start, const PathNode* goal);
	static double HManhat(const PathNode* start, const PathNode* goal);
	static void DrawPath();
	static void DrawEuclidPath();
	static void DrawManhatPath();
	static void ClearPath();
	static void ClearEuclid_ManhatPath();
	static std::vector<PathConnection*>& getPath() { return s_path; }
private:
	PathManager() {}
private:
	static std::vector<NodeRecord*> s_open;
	static std::vector<NodeRecord*> s_closed;
	static std::vector<PathConnection*> s_path;
	static std::vector<PathConnection*> s_euclid_path;
	static std::vector<PathConnection*> s_manhat_path;

};

typedef PathManager PAMA;

#endif
