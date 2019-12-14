#include <iostream>
#include <iterator>
#include <vector>
#include <fstream>
#include <algorithm>
#include <numeric>

int
main()
{
	std::vector<int> pixels;
	std::ifstream inStream("Day8.input.txt");
	while (inStream.good())
	{
		char next = inStream.get();
		if ( (next >= '0') && (next <= '9') )
			pixels.push_back(next - '0');
	}
	constexpr int layerWidth = 25;
	constexpr int layerHeight = 6;
	constexpr int layerPixels = layerWidth * layerHeight;

	auto minIter = pixels.begin();
	auto currentIter = pixels.begin();
	int minZeros = layerPixels + 1;
	while (currentIter != pixels.end())
	{
		int numZeros = std::count(currentIter, currentIter + layerPixels, 0);
		if (numZeros <= minZeros)
		{
			minZeros = numZeros;
			minIter = currentIter;
		}
		currentIter += layerPixels;
	}
	std::cout << std::count(minIter, minIter + layerPixels, 1) *
		std::count(minIter, minIter + layerPixels, 2) << std::endl;
}
