#include <iostream>
#include <string>
#include <vector>

namespace
{

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
}

struct Coord
{
	int x{};
	int y{};
};
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
sameLine(const Coord& c0, const Coord& c1)
{
	return false;
}

int
scoreAsteroid(const Coord& candidate, Asteroids asteroids)
{
	for (auto& asteroid : asteroids)
	{
		asteroid.x -= candidate.x;
		asteroid.y -= candidate.y;
	}
	return 0;
}

int
main()
{
	auto asteroids = getAsteroids(rawMap);
	for (auto targetAsteroid : asteroids)
	{
	}
}
