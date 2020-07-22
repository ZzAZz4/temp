#ifndef FILES_GEOMETRY_H
#define FILES_GEOMETRY_H
#pragma once

#include <utility>
#include <cmath>
#include "class_names.h"
#include "macros.h"


enum class Direction
{
    H, V
};

template<class T>
struct Point
{
    int x, y;

    constexpr
    Point
            (const T &_x, const T &_y)
            : x(_x), y(_y)
    {}

    constexpr explicit
    Point
            (const Direction &dir)
            : x(dir == Direction::H ? 1 : 0), y(dir == Direction::V ? 1 : 0)
    {}

    template<class U>
    constexpr explicit
    Point(std::pair<U, U> pair)
    {
        x = static_cast<T>(pair.first);
        y = static_cast<T>(pair.second);
    }

    template<class U>
    constexpr explicit
    Point(const Point<U> &other)
    {
        x = static_cast<T>(other.x);
        y = static_cast<T>(other.y);
    }


    template<class U>
    constexpr explicit
    operator Point<U>() const
    {
        return Point<U>(*this);
    }

    constexpr
    Point
    operator+
            (Point other) const
    {
        other.x += x;
        other.y += y;
        return other;
    }

    constexpr
    Point
    operator-
            (Point other) const
    {
        auto r = *this;
        r.x -= other.x;
        r.y -= other.y;
        return r;
    }

    constexpr
    Point
    operator*
            (const T &scalar) const
    {
        auto r = *this;
        r.x *= scalar;
        r.y *= scalar;
        return r;
    }

    constexpr
    Point
    operator/
            (const T &scalar) const
    {
        auto r = *this;
        r.x /= scalar;
        r.y /= scalar;
        return r;
    }

    constexpr
    bool
    operator==
            (const Point &other) const
    { return x == other.x && y == other.y; }

    constexpr
    T
    dist_m(const Point &other = {0, 0})
    { return std::abs(x - other.x) + std::abs(y - other.y); }

    constexpr
    T
    dist_sqr(const Point &p = {0, 0})
    { return (x - p.x) * (x - p.x) + (y - p.y) * (y - p.y); }

    template<class U>
    constexpr explicit
    operator BSegment<U>()
    { return BSegment<U>(static_cast<Point<U>>(*this), static_cast<Point<U>>(*this)); }
};

template<class T>
struct BSegment
{
    using Point_t = Point<T>;
    Point_t _begin;
    Point_t _end;

public:
    constexpr
    BSegment
            (Point_t begin, Point_t end)
            : _begin(begin), _end(end)
    {}

    constexpr
    const Point_t &
    begin() const
    { return _begin; }

    constexpr
    Point_t
    end() const
    { return _end; }

    constexpr
    bool intersects
            (const BSegment &other) const;

    constexpr
    bool checkImpact
            (const Shot_marker<T> &shot) const;
};

//#include "../Definitions/Geometry.cpp"


#endif //FILES_GEOMETRY_H
