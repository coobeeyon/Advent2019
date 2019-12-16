#include <iostream>
#include <string>
#include <vector>

namespace
{
std::vector<std::string> testMap = {".#..#", //
                                    ".....", //
                                    "#####", //
                                    "....#", //
                                    "...##"};

std::vector<std::string> rawMap = {
    "#..#.#.#.######..#.#...##", //
    "##.#..#.#..##.#..######.#", //
    ".#.##.#..##..#.#.####.#..", //
    ".#..##.#.#..#.#...#...#.#", //
    "#...###.##.##..##...#..#.", //
    "##..#.#.#.###...#.##..#.#", //
    "###.###.#.##.##....#####.", //
    ".#####.#.#...#..#####..#.", //
    ".#.##...#.#...#####.##...", //
    "######.#..##.#..#.#.#....", //
    "###.##.#######....##.#..#", //
    ".####.##..#.##.#.#.##...#", //
    "##...##.######..##..#.###", //
    "...###...#..#...#.###..#.", //
    ".#####...##..#..#####.###", //
    ".#####..#.#######.###.##.", //
    "#...###.####.##.##.#.##.#", //
    ".#.#.#.#.#.##.#..#.#..###", //
    "##.#.####.###....###..##.", //
    "#..##.#....#..#..#.#..#.#", //
    "##..#..#...#..##..####..#", //
    "....#.....##..#.##.#...##", //
    ".##..#.#..##..##.#..##..#", //
    ".##..#####....#####.#.#.#", //
    "#..#..#..##...#..#.#.#.##"  //
};
} // namespace

struct Coord
{
	int x{};
	int y{};
};

std::ostream&
operator<<(std::ostream& stream, const Coord& coord)
{
	std::cout << "(" << coord.x << "," << coord.y << ")";
	return stream;
}

using Asteroids = std::vector<Coord>;

Asteroids
getAsteroids(const std::vector<std::string>& rawMap)
{
	Coord     currentCoord{};
	Asteroids asteroids;
	for (const auto& row : rawMap)
	{
		currentCoord.x = 0;
		for (const auto val : row)
		{
			if (val == '#')
			{
				asteroids.push_back(currentCoord);
			}
			currentCoord.x++;
		}
		currentCoord.y++;
	}
	return asteroids;
}

bool
isBlocker(Coord c0, Coord c1)
{
	if ((c0.x * c1.x < 0) || (c0.y * c1.y < 0) ||
	    ((c1.x == 0) && (c1.y == 0)) || ((c0.x == c1.x) && (c0.y == c1.y)))
	{
		return false;
	}
	c0.x = abs(c0.x);
	c0.y = abs(c0.y);
	c1.x = abs(c1.x);
	c1.y = abs(c1.y);
	if (c0.x == c1.x)
	{
		std::swap(c0.x, c0.y);
		std::swap(c1.x, c1.y);
	}
	if (c0.x > c1.x)
	{
		std::swap(c0, c1);
	}
	if ((c0.x != 0) && (c1.x % c0.x == 0) && ((c1.x / c0.x) * c0.y == c1.y))
	{
		return true;
	}
	return false;
}

int
scoreAsteroid(Coord candidate, Asteroids asteroids)
{
	for (auto& asteroid : asteroids)
	{
		asteroid.x -= candidate.x;
		asteroid.y -= candidate.y;
	}
	for (const auto& a0 : asteroids)
	{
		for (const auto& a1 : asteroids)
		{
			if (isBlocker(a0, a1))
			{
				std::cout << a0 << "," << a1 << std::endl;
			}
		}
	}
	return 0;
}

int
main()
{
	auto asteroids = getAsteroids(testMap);
	std::cout << "Asteroid 0 " << asteroids[0] << std::endl;
	scoreAsteroid(asteroids[0], asteroids);
}
