#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
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
calcScore(const StarMap& m, int sx, int sy)
{
	StarMap   sm = m;
	const int mw = sm.front().size();
	const int mh = sm.size();
	sm[sy][sx]   = '.';
	for (int ay = 0; ay < mh; ++ay)
	{
		for (int ax = 0; ax < mw; ++ax)
		{
			if (sm[ay][ax] == '#')
			{
				int          dx = ax - sx;
				int          dy = ay - sy;
				unsigned int g  = gcd(abs(dx), abs(dy));
				dy /= (int)g;
				dx /= (int)g;
				for (int y = ay + dy, x = ax + dx;
				     (y >= 0) && (y < mh) && (x >= 0) && (x < mw);
				     y += dy, x += dx)
				{
					sm[y][x] = '.';
				}
			}
		}
	}

	int score =
	    std::accumulate(sm.begin(), sm.end(), 0, [&](auto sum, auto& v) {
		    return sum + std::count(v.begin(), v.end(), '#');
	    });
	return score;
}

int
main()
{
	const StarMap& m  = mainMap;
	const int      mw = m.front().size();
	const int      mh = m.size();

	int maxScore = 0;
	for (int sy = 0; sy < mh; ++sy)
	{
		for (int sx = 0; sx < mw; ++sx)
		{
			if (m[sy][sx] == '#')
			{
				int score = calcScore(m, sx, sy);
				maxScore  = std::max(score, maxScore);
			}
		}
	}
	std::cout << maxScore << std::endl;
}
