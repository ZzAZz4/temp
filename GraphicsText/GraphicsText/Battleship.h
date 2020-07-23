#pragma once
#include <random>
#include "Geometry.h"
#include "Directory.h"


namespace Battleship 
{
    using Geometry::Size;
    using Geometry::Point;
    using Direction = Geometry::Direction::DirectionType;
    using namespace std;

    string token;

    struct Ship
    {
        Point location;
        Direction orientation;
        int length;

        Ship(int l = 0, Point loc = Point(), Direction ori = Direction::Null)
            : location(loc), length(l), orientation(ori)
        {
            if (l < 1)
                throw;
        }

        bool isValid(Size boardSize)
        {
            if (location.x < 0 || location.y < 0)
                return false;
            

            if (orientation == Direction::Horizontal)
            {
                if (location.y >= boardSize.height || location.x + length > boardSize.width)
                {
                    return false;
                }
            }
            else
            {
                if (location.x >= boardSize.width || location.y + length > boardSize.height)
                {
                    return false;
                }
            }

            return true;
        }

        bool isAt(Point loc)
        {
            if (orientation == Direction::Horizontal)
            {
                return (location.y == loc.y) && (location.x <= loc.x) && (location.x + length > loc.x);
            }
            else
            {
                return (location.x == loc.x) && (location.y <= loc.y) && (location.y + length > loc.y);
            }
        }

        vector<Point> getAllLocations()
        {
            vector<Point> allocations;
            if (orientation == Direction::Horizontal)
            {
                for (int i = 0; i < length; i++)
                {
                    allocations.emplace_back(location.x + i, location.y);
                }
            }
            else
            {
                for (int i = 0; i < length; i++)
                {
                    allocations.emplace_back(location.x, location.y + i);
                }
            }
        }

        bool conflictsWith(Ship other)
        {
            if (orientation == Direction::Horizontal &&
                other.orientation == Direction::Horizontal)
            {
                if (location.y != other.location.y)
                {
                    return false;
                }

                return (other.location.x < (location.x + length)) &&
                    (location.x < (other.location.x + other.length));
            }
            else if (orientation == Direction::Vertical &&
                other.orientation == Direction::Vertical)
            {
                if (location.x != other.location.x)
                {
                    return false;
                }

                return (other.location.y < (location.y + length)) &&
                    (location.y < (other.location.y + other.length));
            }
            else
            {
                auto [h, v] = (orientation == Direction::Horizontal) ? make_tuple(*this, other) 
                                                                     : make_tuple(*this, other);

                return (h.location.y >= v.location.y) &&
                    (h.location.y < (v.location.y + v.length)) &&
                    (v.location.x >= h.location.x) &&
                    (v.location.x < (h.location.x + h.length));
            }
        }

        template <template <class, class ...> class Container, class T, class ..._>
        bool isSunk(Container<T, _...> shots)
        {
            for (auto point : getAllLocations()) 
            {
                if (find(begin(shots), end(shots), point) == end(shots))
                    return false;
            }
            return true;
        }
    };

	struct AI 
	{
        enum class State 
        { 
            Unknown, 
            Miss, 
            Hit 
        };
        struct NextShot
        {
        public:
            Point point;
            Direction direction;

            NextShot(Point p = Point(), Direction d = Direction::Null)
                : point(p), direction(d) {}
        };

        struct ScanShot
        {
            Point point;
            int openSpaces;

            ScanShot(Point p = Point(), int o = 0) : point(p), openSpaces(o)
            {}
        };

        Size gameSize;
        vector<int> shipLengths;
        vector<Point> scanShots;
        vector<NextShot> nextShots;

        Direction hitDirection, lastShotDirection;

        vector<vector<State>> board;

        AI(Size size = Size(10, 10), vector<int> sLengths = {}) : gameSize(size)
        {
            using namespace Directory;
            newGame(size, sLengths);
            Output::Sender::enqueue(
                Output::Message(
                    Instruction::Handshake, "BetaTester"
                )
            );
        }

        void newGame(Size size, vector<int> sLengths)
        {
            gameSize = size;
            shipLengths = sLengths;
            scanShots = vector<Point>();
            nextShots = vector<NextShot>();
            fillScanShots();
            hitDirection = Direction::Null;
            board = vector<vector<State>>(size.width);
            for (auto& row : board)
                row.resize(size.height);
        }

        void fillScanShots()
        {
            int x;
            for (x = 0; x < gameSize.width - 1; x++)
                scanShots.emplace_back(Point(x, x));
            

            if (gameSize.width == 10)
            {
                for (x = 0; x < 3; x++)
                {
                    scanShots.emplace_back(Point(9 - x, x));
                    scanShots.emplace_back(Point(x, 9 - x));
                }
            }
        }

        
        static inline vector<double> stratProb = { 
            0.115634768,	
            0.109404171,	
            0.094777777,	
            0.091133511,
            0.089185197,	
            0.089185197,	
            0.091133511,	
            0.094777777,	
            0.109404171,	
            0.115363919
        };

        std::default_random_engine rand_num{ static_cast<long unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()) };

        std::uniform_real_distribution<double> rangef{ 0.0, 1.0 };
        std::uniform_int_distribution<int> rangei{ 0, INT_MAX };

        vector<Ship> placeShips()
        {
            int xpos = -1, ypos = -1;
            for (auto f = rangef(rand_num); f >= 0;)
                f -= stratProb[++xpos];

            for (auto f = rangef(rand_num); f >= 0;)
                f -= stratProb[++ypos];

            vector<Ship> ships (shipLengths.size());

            for (unsigned i = 0; i < ships.size();) 
            {
                auto candidate = Ship(
                    shipLengths[i],
                    {xpos % gameSize.width, ypos % gameSize.height },
                    rangei(rand_num) % 2 == 0 ? Direction::Horizontal : Direction::Vertical);
                
                bool accepted = true;
                for (auto& ship : ships)
                    if (ship.conflictsWith(candidate))
                        accepted = false;

                if (accepted)
                    ships[i++] = candidate;
            }
            return ships;
        }

        Point getShot()
        {
            Point shot;

            if (nextShots.size() > 0)
            {
                if (hitDirection != Direction::Null)
                {
                    if (hitDirection == Direction::Horizontal)
                    {
                        sort(nextShots.begin(), nextShots.end(), [](NextShot s1, NextShot s2) { 
                            return s1.point.x < s2.point.x; 
                        });
                    }
                    else
                    {
                        sort(nextShots.begin(), nextShots.end(), [](NextShot s1, NextShot s2) { 
                            return s1.point.x >= s2.point.x; 
                        });
                    }
                }

                shot = nextShots.back().point;
                lastShotDirection = nextShots.back().direction;
                nextShots.pop_back();
                return shot;
            }

            vector<ScanShot> scanShots;
            for (int x = 0; x < gameSize.width; x++)
            {
                for (int y = 0; y < gameSize.height; y++)
                {
                    if (board[x][y] == State::Unknown)
                    {
                        scanShots.emplace_back(Point(x, y), openSpaces(x, y));
                    }
                }
            }
            sort(scanShots.begin(), scanShots.end(), [&](ScanShot s1, ScanShot s2) {
                return (abs(gameSize.width / 2 - s1.point.x) > abs(gameSize.width / 2 - s2.point.x));
            });

            stable_sort(scanShots.begin(), scanShots.end(), [&](ScanShot s1, ScanShot s2) {
                return (abs(gameSize.height / 2 - s1.point.y) > abs(gameSize.height / 2 - s2.point.y));
            });

            int maxOpenSpaces = 0;
            if (scanShots.size() > 0)
                maxOpenSpaces = scanShots.back().openSpaces;

            vector<ScanShot> scanShots2;
            for (auto& nShot : scanShots)
                if (nShot.openSpaces == maxOpenSpaces)
                    scanShots2.emplace_back(nShot);
            
            shot = scanShots2[rand() % scanShots2.size()].point;

            return shot;
        }

        int openSpaces(int x, int y)
        {
            int ctr = 0;
            int pX, pY;

            // spaces to the left
            for (pX = x - 1, pY = y; pX >= 0 && board[pX][pY] == State::Unknown; --pX)
                ctr++;
            
            // spaces to the right
            for (pX = x + 1, pY = y; pX < gameSize.width && board[pX][pY] == State::Unknown; ++pX)
                ctr++;

            // spaces to the top
            for (pX = x, pY = y - 1;  pY >= 0 && board[pX][pY] == State::Unknown; --pY)
                 ctr++;
            
            // spaces to the bottom
            for (pX = x, pY = y + 1; pY < gameSize.height && board[pX][pY] == State::Unknown; ++pY)
                ctr++;

            return ctr;
        }

        void ShotHit(Point shot)
        {
            board[shot.x][shot.y] = State::Hit;

            hitDirection = lastShotDirection;
            if (shot.x != 0)
                nextShots.emplace_back(Point(shot.x - 1, shot.y), Direction::Horizontal);

            if (shot.y != 0)
                nextShots.emplace_back(Point(shot.x, shot.x - 1), Direction::Vertical);

            if (shot.x != gameSize.width - 1)
                nextShots.emplace_back(Point(shot.x + 1, shot.y), Direction::Horizontal);

            if (shot.y != gameSize.height - 1)
                nextShots.emplace_back(Point(shot.x, shot.y + 1), Direction::Vertical);

        }
        
        void ShotMiss(Point shot)
        {
            board[shot.x][shot.y] = State::Miss;
        }

        void ShotSunk(Point shot) 
        {
            hitDirection = Direction::Null;
            nextShots.clear();
        }

        static string toString(Point p) 
        {
            vector<string> xmap = { "1", "2", "3", "4", "5", "6", "7", "8", "9", "10" };
            vector<string> ymap = { "A", "B", "C", "D", "E", "F", "G", "H", "I", "J" };

            auto sx = xmap[p.x], sy = ymap[p.y];
            return sx + sy;
            
        }

        void enqueueAttack() 
        {
            using namespace Directory;

            auto data = toString(getShot());

            auto message = Output::Message(token, Instruction::Attack, data);
            Output::Sender::enqueue(message);     
            return;
        }

        void messagePlacements() 
        {
            using namespace Directory;
            
            vector<string> sizeMap = { "T", "S", "B", "A" };
            auto placements = placeShips();

            for (const auto& placement : placements) 
            {
                auto data = sizeMap[placement.length] + "-" 
                       + toString(placement.location) + "-" 
                       + (placement.orientation == Direction::Horizontal ? "H" : "V");

                auto message = Output::Message(token, Instruction::Placefleet, data);
                Output::Sender::enqueue(message);
            }
        }
	};

    
}