#include <array>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <numeric>
#include <vector>
#include <algorithm>

struct OrbitalSystem
{
	using BodyName = std::string;
	using OrbitMap = std::unordered_map<BodyName, BodyName>;
	using OrbitChain = std::vector<std::string>;
	using OrbitCounts = std::unordered_map<BodyName, int>;

	void loadOrbits(std::ifstream& inputStream);
	int totalOrbits();

	OrbitChain orbitChain( const BodyName& body );

private:
	int numOrbits(const BodyName& orbiter);

    OrbitMap m_orbitMap{};
	OrbitCounts m_orbitCounts{OrbitCounts::value_type{"COM",0}};
};

void
OrbitalSystem::loadOrbits(std::ifstream& inputStream)
{
	std::string line;
	while( std::getline(inputStream, line) )
	{
		BodyName orbited = line.substr(0,3);
		BodyName orbiter = line.substr(4,7);
		m_orbitMap[orbiter] = orbited;
	}
}

OrbitalSystem::OrbitChain
OrbitalSystem::orbitChain( const BodyName& body )
{
	if ( auto it = m_orbitMap.find(body) ; it != m_orbitMap.end() )
	{
		auto chain = orbitChain(it->second);
		chain.push_back(body);
		return chain;
	}
	return {};
}

int
OrbitalSystem::numOrbits(const std::string& orbiter)
{
	if ( auto countIt = m_orbitCounts.find(orbiter) ; countIt != m_orbitCounts.end() )
	{
		return countIt->second;
	}

	int ret = 1 + numOrbits( m_orbitMap[orbiter] );
	m_orbitCounts[orbiter] = ret;
	return ret;
}

int
OrbitalSystem::totalOrbits()
{
	int sum{};
	for ( const auto& p : m_orbitMap )
	{
		sum += numOrbits(p.first);
	}
	return sum;
}

int
main()
{
	std::ifstream inputStream("Day6.input.txt");
	OrbitalSystem orbitalSystem;
	orbitalSystem.loadOrbits(inputStream);
	std::cout << orbitalSystem.totalOrbits() << std::endl;

	auto myChain = orbitalSystem.orbitChain("YOU");
	for ( const auto& body : myChain )
	{
		std::cout << body << " ";
	}
	std::cout << std::endl;
	auto santaChain = orbitalSystem.orbitChain("SAN");
	for ( const auto& body : santaChain )
	{
		std::cout << body << " ";
	}
	auto myIt = myChain.begin();
	auto santaIt = santaChain.begin();
	do
	{
		++myIt;
		++santaIt;
	} while (*myIt == *santaIt);
	std::cout << std::endl << "Distance" << std::distance(myIt, myChain.end()) + std::distance(santaIt, santaChain.end()) - 2 << std::endl;
}
