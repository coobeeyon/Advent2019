#include <iostream>
#include <iterator>
#include <vector>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <array>

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

	const int numLayers = pixels.size() / layerPixels;
	std::array<int, layerPixels> image;
	image.fill(2);
	for (int layerPixel = 0; layerPixel < layerPixels ; layerPixel++)
	{
		int layerNum = 0;
		while ( (image[layerPixel] == 2) && ( layerNum < numLayers ))
		{
			image[layerPixel] = pixels[layerNum * layerPixels + layerPixel];
			layerNum++;
		}
	}
	for (int row = 0; row < layerHeight ; ++row)
	{
		for ( int col = 0 ; col < layerWidth ; ++col )
		{
			std::cout << ((image[row * layerWidth + col] == 0) ? ' ' : '*');
		}
		std::cout << std::endl;
	}
}
