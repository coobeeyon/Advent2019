#include "range/v3/view/iota.hpp"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <numeric>
#include <range/v3/all.hpp>
#include <string>
#include <tuple>
#include <vector>

namespace
{
using StarMap = std::vector<std::string>;

StarMap crossMap = {".#.", "###", ".#."};

StarMap testMap = {".#..#", //
                   ".....", //
                   "#####", //
                   "....#", //
                   "...##"};

StarMap testMap2 = {"......#.#.", //
                    "#..#.#....", //
                    "..#######.", //
                    ".#.#.###..", //
                    ".#..#.....", //
                    "..#....#.#", //
                    "#..#....#.", //
                    ".##.#..###", //
                    "##...#..#.", //
                    ".#....####"};

StarMap mainMap = {
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

unsigned int
gcd(unsigned int u, unsigned int v)
{
	unsigned int shift = 0;

	/* GCD(0,v) == v; GCD(u,0) == u, GCD(0,0) == 0 */
	if (u == 0)
		return v;
	if (v == 0)
		return u;

	/* Let shift := lg K, where K is the greatest power of 2
	    dividing both u and v. */
	while (((u | v) & 1) == 0)
	{
		shift++;
		u >>= 1;
		v >>= 1;
	}

	while ((u & 1) == 0)
		u >>= 1;

	/* From here on, u is always odd. */
	do
	{
		/* remove all factors of 2 in v -- they are not common */
		/*   note: v is not zero, so while will terminate */
		while ((v & 1) == 0)
			v >>= 1;

		/* Now u and v are both odd. Swap if necessary so u <= v,
		    then set v = v - u (which is even). For bignums, the
		     swapping is just pointer movement, and the subtraction
		      can be done in-place. */
		if (u > v)
		{
			unsigned int t = v;
			v              = u;
			u              = t; // Swap u and v.
		}

		v -= u; // Here v >= u.
	} while (v != 0);

	/* restore common factors of 2 */
	return u << shift;
}

int
countAsteroids(const StarMap& m)
{
	int numAsteroids = ranges::accumulate(m, 0, [&](auto sum, auto& v) {
		return sum + ranges::count(v, '#');
	});
	return numAsteroids;
}

auto
enumerateAsteroids(const StarMap& m)
{
	std::vector<std::pair<int, int>> asteroids;
	int                              y = 0;
	for (const auto& row : m)
	{
		int x = 0;
		for (const auto& loc : row)
		{
			if (loc == '#')
				asteroids.emplace_back(x, y);
			++x;
		}
		++y;
	}
	return asteroids;
}

StarMap
findZapped(const StarMap& m, int sx, int sy)
{
	StarMap   sm = m;
	const int mw = sm.front().size();
	const int mh = sm.size();
	sm[sy][sx]   = '.';
	for (auto [ax, ay] : enumerateAsteroids(sm))
	{
		int          dx = ax - sx;
		int          dy = ay - sy;
		unsigned int g  = gcd(abs(dx), abs(dy));
		dy /= (int)g;
		dx /= (int)g;
		for (int y = ay + dy, x = ax + dx;
		     (y >= 0) && (y < mh) && (x >= 0) && (x < mw); y += dy, x += dx)
		{
			sm[y][x] = '.';
		}
	}
	return sm;
}

int
main()
{
	StarMap& m = mainMap;

	auto max_info = std::make_tuple(-1, -1, 0);
	for (auto [sx, sy] : enumerateAsteroids(m))
	{
		auto info =
		    std::make_tuple(sx, sy, countAsteroids(findZapped(m, sx, sy)));
		max_info = std::max(info, max_info, [](auto& ta, auto& tb) {
			return std::get<2>(ta) < std::get<2>(tb);
		});
	}
	int max_x, max_y, max_score;
	std::tie(max_x, max_y, max_score) = max_info;
	std::cout << max_x << " " << max_y << " " << max_score << std::endl;

	std::vector<std::pair<int, int>> zapOrder{};
	while (countAsteroids(m) > 1)
	{
		auto zapped = enumerateAsteroids(findZapped(m, max_x, max_y));
		ranges::sort(zapped, [max_x, max_y](auto& a, auto& b) {
			auto [ax, ay] = a;
			auto [bx, by] = b;
			return atan2(ax - max_x, ay - max_y) >
			       atan2(bx - max_x, by - max_y);
		});
		ranges::copy(zapped, ranges::back_inserter(zapOrder));
		ranges::for_each(zapped, [&m](auto& p) {
			auto [px, py] = p;
			m[py][px]     = '.';
		});
	}
	auto [ans_x, ans_y] = zapOrder[199];
	std::cout << '(' << ans_x << "," << ans_y << ")";
}
