#include <algorithm>
#include <array>
#include <charconv>
#include <fstream>
#include <iterator>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <cassert>

using Program = std::vector<int>;

enum class OpCode : uint8_t
{
	Add    = 1,
	Mult   = 2,
	Input  = 3,
	Output = 4,
	Halt   = 99
};

enum class ParameterMode : uint8_t
{
	Position = 0,
	Value    = 1
};

struct Parameter
{
	int           m_value;
	ParameterMode m_mode;
};

class Runtime
{
public:
	Runtime(Program&& program):
		m_program(std::move(program)),
		m_currentPos(m_program.begin()),
		m_isRunning(false)
	{}
	void run();
private:
	Program m_program{};
	Program::iterator m_currentPos{};
	bool m_isRunning{};
};

void
Runtime::run()
{
	m_isRunning = true;
	std::copy(m_program.begin(), m_program.end(),
			  std::ostream_iterator<int>(std::cout));
	m_isRunning = false;
}

std::vector<int> loadProgram(const std::string& fileName)
{
	Program program;
	std::ifstream programFile(fileName);
	std::string programLine;
	std::getline(programFile, programLine);
	std::istringstream programLineStream(programLine);
	std::array<char, 64> token;
	while (programLineStream.getline(&token[0], 64, ','))
	{
		program.push_back( std::stoi(&token[0] ));
	}
	return program;
}

int
main()
{
	auto program = loadProgram("Day5.input.txt");
	Runtime runtime(std::move(program));
	runtime.run();
}
