#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"

#include "../Headers/Hash_functions.h"

template<class T>
constexpr inline
void
hash_combine(std::size_t &seed, const T &v)
{
    std::hash<T> THash{};
    seed ^= THash(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template<typename T>
struct std::hash<Shot_marker<T>>
{
    inline std::size_t operator()(const Shot_marker<T> &p) const
    {
        auto &D = Shot_marker<T>::State::Destroy;
        auto &M = Shot_marker<T>::State::Miss;
        auto &H = Shot_marker<T>::State::Hit;

        size_t seed = hash(Point<T>(p));
        hash_combine(seed, seed << (p._state == H ? 1 : p._state == M ? 2 : 3));
        return seed;
    }

};

template<typename T>
struct std::hash<Point<T>>
{
    inline std::size_t operator()(const Point<T> &p) const
    {
        return size_t(p.y << 16 ^ p.x);
    }
};

template<typename C>
struct hash_container
{
    typedef typename C::value_type value_type;

    inline size_t operator()(const C &c) const
    {
        size_t seed = 0;
        for (typename C::const_iterator it = c.begin(), end = c.end(); it != end; ++it)
        {
            hash_combine<value_type>(seed, *it);
        }
        return seed;
    }
};


#pragma clang diagnostic pop