//
// Created by esteb on 6/12/2020.
//

#ifndef FILES_GAME_ITEMS_H
#define FILES_GAME_ITEMS_H
#pragma once

#include "macros.h"
#include "class_names.h"
#include "Geometry.h"
#include <iostream>
#include <list>
#include <unordered_map>
#include <unordered_set>


template<class T>
class Boat : public BSegment<T>
{
private:
    using Point_t   =   Point<T>;
    using Set_t     =   std::unordered_set<Point<T>>;

    size_t _size;
    size_t _lives;

public:
    constexpr explicit
    Boat
            (Point_t spawn = {0, 0},
             Direction dir = Direction::H,
             size_t size = 0)
            : BSegment<T>(spawn, spawn + Point<T>(dir) * (size - 1)), _size(size), _lives(size)
    {}

    constexpr inline
    size_t size()
    { return _size; }

    constexpr inline
    void
    receive_damage()
    { --_lives; }

    constexpr inline
    bool
    alive() const
    { return _lives > 0; }
};

template<class T>
class Shot_marker : public Point<T>
{
public:
    enum class State
    {
        Destroy, Hit, Miss
    };

    constexpr explicit
    Shot_marker
            (const Point<T> &spawn = {0, 0},
             const State &s = State::Miss)
            : Point<T>(spawn), _state(s)
    {}

    constexpr
    Shot_marker
            (const T &x,
             const T &y,
             const State &s)
            : Point<T>(x, y), _state(s)
    {}

    template<class U>
    constexpr inline
    bool
    hits(const Boat<U> &boat)
    { return boat.checkImpact(*this); }

    constexpr
    const State &
    get()
    { return _state; }

private:
    State _state;
};

template<class T>
class Board
{
    using Boat_list_t   =   std::list<Boat<T>>;
    using list_iter     =   typename Boat_list_t::iterator;
    using State         =   typename Shot_marker<T>::State;
    template<class U, class V>
    using Map           =   std::unordered_map<U, V>;
    template<class U>
    using Set           = std::unordered_set<U>;

    Boat_list_t Boat_list;
    Map<Point<T>, list_iter> Positions;
    size_t v_size, h_size;

#ifdef DEBUG
    Map<Point<T>, Shot_marker<T>> shot_record;
#endif

public:

    constexpr
    Board
            (const size_t &_vSize,
             const size_t &_hSize)
            : v_size(_vSize), h_size(_hSize)
    {}

    constexpr
    bool
    add_boat
            (Boat<T> nBoat)
    {
        if (nBoat.begin().x < 0 || nBoat.begin().x >= h_size ||
            nBoat.begin().y < 0 || nBoat.begin().y >= v_size ||
            nBoat.end().x < 0 || nBoat.end().x >= h_size ||
            nBoat.end().y < 0 || nBoat.end().y >= v_size)
            return false;

        for (const auto &boat : Boat_list)
            if (nBoat.intersects(boat))
                return false;

        size_t size = nBoat.size() - 1;
        auto dir = (nBoat.end() - nBoat.begin()) / size;
        Boat_list.emplace_back(std::move(nBoat));

        for (size_t i = 0; i <= size; ++i)
        {
            auto shift = dir * i;
            auto point = Boat_list.back().begin() + shift;
            Positions[point] = --Boat_list.end();
        }
        return true;
    }

    constexpr
    bool
    add_boat
            (const T &x,
             const T &y,
             const Direction &dir,
             const size_t &size)
    {
        Point<T> nP(x, y);
        Boat<T> nBoat(nP, dir, size);
        return add_boat(nBoat);
    }

    constexpr
    bool
    add_boat
            (const Point<T> &nP,
             const Direction &dir,
             const size_t &size)
    {
        Boat<T> nBoat(nP, dir, size);
        return add_boat(nBoat);
    }

    constexpr
    Shot_marker<T>
    receive_attack
            (const T &x,
             const T &y);

    constexpr inline
    Shot_marker<T>
    receive_attack
            (const Point<T> &shot)
    { return receive_attack(shot.x, shot.y); }

    constexpr inline
    State
    send_attack
            (Board &other,
             const T &x,
             const T &y)
    { return other.receive_attack(x, y); }

    constexpr inline
    State
    send_attack
            (Board &other,
             const Point<T> &position)
    { return other.receive_attack(position.x, position.y); }

#ifdef DEBUG

    void print_board();

#endif
};

std::ostream &
operator<<
        (std::ostream &os,
         const typename Shot_marker<>::State &state);

//#include "../Definitions/Game_items.cpp"


#endif //FILES_GAME_ITEMS_H
