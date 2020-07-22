#include "../Headers/Game_items.h"
#include "../Headers/macros.h"

std::ostream &
operator<<
        (std::ostream &os,
         const typename Shot_marker<>::State &state)
{
    const auto &Hit = Shot_marker<>::State::Hit;
    const auto &Miss = Shot_marker<>::State::Miss;
    const auto &Destroy = Shot_marker<>::State::Destroy;

    os << (state == Hit ? 'H' : (state == Miss) ? 'M' : 'D');
    return os;
}

#ifndef DEBUG
template<class T>
constexpr
Shot_marker<T>
Board<T>::receive_attack
        (const T &x,
         const T &y)
{
    Point<T> point(x, y);
    auto point_iter = Positions.find(point);
    if (point_iter == Positions.end())
        return Shot_marker({x, y}, State::Miss);

    auto &[__, boat_list_iter] = *point_iter;

    auto &boat = *boat_list_iter;
    boat.receive_damage();

    if (!boat.alive())
    {
        Boat_list.erase(boat_list_iter);
        Positions.erase(point_iter);

        return Shot_marker({x, y}, State::Destroy);
    }
    return Shot_marker({x, y}, State::Hit);
}
#else
template<class T>
constexpr
Shot_marker<T>
Board<T>::receive_attack
        (const T &x,
         const T &y)
{
    Point<T> point(x, y);
    auto point_iter = Positions.find(point);
    if (point_iter == Positions.end())
        return (*shot_record.emplace(point, Shot_marker({x, y}, State::Miss)).first).second;

    auto &[__, boat_list_iter] = *point_iter;

    Boat<T> &boat = *boat_list_iter;
    boat.receive_damage();

    if (!boat.alive())
    {
        Boat_list.erase(boat_list_iter);
        Positions.erase(point_iter);

        return (*shot_record.emplace(point, Shot_marker({x, y}, State::Destroy)).first).second;
    }
    return (*shot_record.emplace(point, Shot_marker({x, y}, State::Hit)).first).second;
}

template<class T>
void
Board<T>::print_board()
{
    for (size_t y = 0; y < v_size; ++y)
    {
        for (size_t x = 0; x < h_size; ++x)
        {
            auto point = Point<T>(x, y);
            if (shot_record.find(point) != shot_record.end())
                std::cout << ' ' << shot_record[point].get();
            else
                std::cout << (Positions.find(point) == Positions.end() ? " ." : " #");
        }
        std::cout << '\n';
    }
}
#endif
