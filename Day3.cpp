#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cassert>
#include <optional>

using TokenVec = std::vector<std::string>;

TokenVec
lineTokens(const std::string& line)
{
	TokenVec tokens;
	std::istringstream stringStream(line);
	std::array<char, 64> tokenChars;
	while (stringStream.getline(&tokenChars[0], 64, ','))
	{
		tokens.emplace_back(tokenChars.data());
	}
	return tokens;
}

std::vector<TokenVec>
streamTokens(std::istream& inStream)
{
	std::vector<TokenVec> ret;
	std::string line;
	while (getline(inStream, line))
	{
		ret.push_back(lineTokens(line));
	}
	return ret;
}

struct Point
{
	Point() = default;
	Point(int x, int y) : x(x), y(y){}
	int x{};
	int y{};
};
bool
operator==(const Point& p0, const Point& p1)
{
	return (p0.x == p1.x) && (p0.y == p1.y);
}
std::ostream& operator<<(std::ostream& stream, const Point& p)
{
	stream << "(" << p.x << "," << p.y << ")";
	return stream;
}

struct Edge
{
	Point start{};
	Point end{};
};
std::ostream& operator<<(std::ostream& stream, const Edge& e)
{
	stream << "(" << e.start << "=>" << e.end << ")";
	return stream;
}


using Edges = std::vector<Edge>;

Edges
parsePath(const TokenVec& tokens)
{
	Edges edges;
	Point curPoint{};
	for (auto& token : tokens)
	{
		Edge newEdge{curPoint};
		int direction = token[0];
		int distance = stoi(token.substr(1));
		switch (direction)
		{
		case 'R':
			curPoint.x += distance;
			break;
		case 'L':
			curPoint.x -= distance;
			break;
		case 'U':
			curPoint.y += distance;
			break;
		case 'D':
			curPoint.y -= distance;
			break;
		default:
			assert("Unknown direction");
		}
		newEdge.end = curPoint;
		if (( newEdge.start.x > newEdge.end.x ) || ( newEdge.start.y > newEdge.end.y ))
		{
			std::swap(newEdge.start, newEdge.end);
		}
		edges.push_back(newEdge);
	}
	return edges;
}

bool isHorizontal(const Edge& edge)
{
	return edge.start.y == edge.end.y;
}

bool isVertical(const Edge& edge)
{
	return edge.start.x == edge.end.x;
}

std::vector<Point>
intersectEdges(Edge edge0, Edge edge1)
{
	std::vector<Point> intersections;
	if ( isHorizontal(edge0) && isHorizontal(edge1) && ( edge0.start.y == edge1.start.y ) )
	{
		if ( edge0.start.x > edge1.start.x )
		{
			std::swap(edge0, edge1);
		}
		if ((edge0.end.x >= edge1.start.x) || (edge1.end.x >= edge0.start.x))
		{
			int left = std::max(edge0.start.x, edge1.start.x);
			int right = std::min(edge0.end.x, edge0.end.x);
			for(int x = left; x <= right; ++x)
			{
				intersections.emplace_back(x, edge0.start.y);
			}
		}
	}
	else if ( isVertical(edge0) && isVertical(edge1) && ( edge0.start.x == edge1.start.x ) )
	{
		if ( edge0.start.y > edge1.start.y )
		{
			std::swap(edge0, edge1);
		}
		if ((edge0.end.y >= edge1.start.y) || (edge1.end.y >= edge0.start.y))
		{
			int bottom = std::max(edge0.start.y, edge1.start.y);
			int top = std::min(edge0.end.y, edge0.end.y);
			for(int y = bottom; y <= top; ++y)
			{
				intersections.emplace_back(edge0.start.x, y);
			}
		}
	}
	else
	{
		if (isHorizontal(edge1))
		{
			std::swap(edge0, edge1);
		}
		int horizY = edge0.start.y;
		int vertX = edge1.start.x;
		if (((edge0.start.x <= vertX) && (edge0.end.x >= vertX)) &&
			((edge1.start.y <= horizY) && (edge1.end.y >= horizY)))
		{
			intersections.emplace_back(vertX, horizY);
		}
	}
	return intersections;
}

int
main()
{
	std::ifstream dataStream{"./Day3Input.txt"};
	std::vector<TokenVec> tokens = streamTokens(dataStream);
	Edges path1 = parsePath(tokens[0]);
	Edges path2 = parsePath(tokens[1]);
#if 0
	std::cout << "Path1:\n";
	for (auto& edge : path1)
	{
		std::cout << edge << std::endl;
	}
	std::cout << "Path2:\n";
	for (auto& edge : path2)
	{
		std::cout << edge << std::endl;
	}
#endif
	std::vector<Point> intersections;
	for (auto& edge0 : path1)
	{
		for ( auto& edge1 : path2 )
		{
			auto edgeIntersections = intersectEdges(edge0, edge1);
			std::move(edgeIntersections.begin(), edgeIntersections.end(),
					  std::back_inserter(intersections));
		}
	}
	std::cout << "Intersections:\n";
	intersections.erase(std::remove(intersections.begin(), intersections.end(), Point{}), intersections.end());
	for (auto& point : intersections)
	{
		std::cout << point << std::endl;
	}
	std::cout << "Furthest:\n";
	Point furthest = *std::max_element(intersections.begin(), intersections.end(),
									   [](const Point& p0, const Point& p1)
									   {
										   int d0 = abs(p0.x) + abs(p0.y);
										   int d1 = abs(p1.x) + abs(p1.y);
										   return d0 > d1;
									   });
	std::cout << furthest << ": " << abs(furthest.x) + abs(furthest.y);
}
