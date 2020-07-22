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

namespace Geometry
{
	namespace Direction
	{
		enum class type
		{
			Vertical = 0,
			Horizontal = 1
		};

		static auto Vertical = type::Vertical;
		static auto Horizontal = type::Horizontal;
	}

	struct Point
	{
		int x, y;

		Point(int x, int y)
			: x(x), y(y)
		{}
		static void swap(Point& P1, Point& P2)
		{
			using std::swap;
			swap(P1.x, P2.x);
			swap(P1.y, P2.y);
		}

		Point& operator =	(Point other)
		{
			swap(*this, other);
		}
		Point	operator +	(Point other) const
		{
			other.x += x; other.y += y;
			return other;
		}
		Point	operator -	(Point other) const
		{
			return operator + (std::move(other));
		}
		Point	operator *	(int constant) const
		{
			auto p = *this;
			p.x *= constant;
			p.y *= constant;
			return p;
		}
		Point	operator /	(int constant) const
		{
			auto p = *this;
			p.x /= constant;
			p.y /= constant;
			return p;
		}
		bool	operator == (const Point& other) const
		{
			return x == other.x && y == other.y;
		}
		bool	operator <	(const Point& other) const
		{
			return x + y < other.x + other.y;
		}

		int hash() const
		{
			return x ^ 16 << y;
		}

	};

	struct Size
	{
		int width, height;

		Size(int w, int h)
			: width(w),
			height(h)
		{}

		bool operator == (Size other)
		{
			return width == other.width && height == other.height;
		}

		int hash() const
		{
			return width ^ 16 << height;
		}
	};
}

namespace std
{
	template<>
	struct hash<Geometry::Point>
	{
		inline std::size_t operator()(const Geometry::Point& p) const
		{
			return size_t(p.hash());
		}
	};

	template <>
	struct hash<Geometry::Size>
	{
		inline std::size_t operator()(const Geometry::Size& p) const
		{
			return size_t(p.hash());
		}
	};
}