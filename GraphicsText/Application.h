#pragma once

#include <iostream>
#include <chrono>
#include <stack>
#include <optional>
#include <string>
#include <fstream>
#include <thread>
#include <atomic>
#include <mutex>
#include <cassert>
#include <queue>
#include <map>
#include <set>
#include <unordered_map>
#include <cassert>
#include <unordered_set>
#include "Battleship.h"

#include "olcPixelGameEngine.h"
using namespace std::chrono;


std::ostream& operator <<
(std::ostream& os, const Battleship::Ship& s)
{
	os << "Ship({" << s.location.x << ' ' << s.location.y << "}, "
		<< (s.direction == Geometry::Direction::Horizontal ? "H, " : "V, ")
		<< s.length << ") ";
	return os;
}

template <class T>
std::ostream& operator <<
(std::ostream& os, const std::vector<T>& v)
{
	os << "[";
	for (const auto& i : v)
		os << i << ", ";
	os << "]";
	return os;
}


class Application : public olc::PixelGameEngine
{
public:
	Application()
	{
		sAppName = "Application";
	}

private:
	using Matrix = std::vector<std::vector<int>>;

	int		m_nMazeWidth;
	int		m_nMazeHeight;
	Matrix  m_maze;


	// Some bit fields for convenience
	enum
	{
		CELL_PATH_N = 0x01,
		CELL_PATH_E = 0x02,
		CELL_PATH_S = 0x04,
		CELL_PATH_W = 0x08,
		CELL_VISITED = 0x10,
	};


	// Algorithm variables
	int  m_nVisitedCells;
	std::stack<std::pair<int, int>> m_stack;	// (x, y) coordinate pairs
	int  m_nPathWidth;


protected:
	// Called by olcConsoleGameEngine
	virtual bool OnUserCreate()
	{
		// Maze parameters
		m_nMazeWidth = 10;
		m_nMazeHeight = 10;
		m_maze = Matrix(m_nMazeHeight, std::vector<int>(m_nMazeWidth, 0));
		m_nPathWidth = 5;

		return true;
	}

	// Called by olcConsoleGameEngine
	virtual bool OnUserUpdate(float fElapsedTime)
	{

		// Clear Screen by drawing 'spaces' everywhere
		FillRect(0, 0, ScreenWidth(), ScreenHeight(), olc::BLACK);

		// Draw Maze
		for (int x = 0; x < m_nMazeWidth; x++)
		{
			for (int y = 0; y < m_nMazeHeight; y++)
			{
				// Each cell is inflated by m_nPathWidth, so fill it in
				for (int py = 0; py < m_nPathWidth; py++)
					for (int px = 0; px < m_nPathWidth; px++)
					{
						if (m_maze[y][x] & CELL_VISITED)
							Draw(x * (m_nPathWidth + 1) + px, y * (m_nPathWidth + 1) + py, olc::WHITE); // Draw Cell
						else
							Draw(x * (m_nPathWidth + 1) + px, y * (m_nPathWidth + 1) + py, olc::BLUE); // Draw Cell
					}

				// Draw passageways between cells
				for (int p = 0; p < m_nPathWidth; p++)
				{
					if (m_maze[y][x] & CELL_PATH_S)
						Draw(x * (m_nPathWidth + 1) + p, y * (m_nPathWidth + 1) + m_nPathWidth); // Draw South Passage

					if (m_maze[y][x] & CELL_PATH_E)
						Draw(x * (m_nPathWidth + 1) + m_nPathWidth, y * (m_nPathWidth + 1) + p); // Draw East Passage
				}
			}
		}

		return true;
	}
};