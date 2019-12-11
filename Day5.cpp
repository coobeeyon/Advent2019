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

std::ostream&
operator<<(std::ostream& ostream, const OpCode& opCode)
{
	switch (opCode)
	{
	case OpCode::Add:
		ostream << "Add";
		break;
	case OpCode::Mult:
		ostream << "Mult";
		break;
	case OpCode::Input:
		ostream << "Input";
		break;
	case OpCode::Output:
		ostream << "Output";
		break;
	case OpCode::Halt:
		ostream << "Halt";
		break;
	}
	return ostream;
}

int
numParams( OpCode opCode )
{
	switch (opCode)
	{
	case OpCode::Add:
	case OpCode::Mult:
		return 3;
	case OpCode::Input:
	case OpCode::Output:
		return 1;
	case OpCode::Halt:
		return 0;
	}
}

enum class ParameterMode : uint8_t
{
	Position = 0,
	Value    = 1
};

std::ostream&
operator<<(std::ostream& ostream, const ParameterMode& mode)
{
	switch (mode)
	{
	case ParameterMode::Position:
		ostream << "Position";
		break;
	case ParameterMode::Value:
		ostream << "Value";
		break;
	}
	return ostream;
}

struct Parameter
{
	int           m_value{};
	ParameterMode m_mode{ParameterMode::Position};
};

std::ostream&
operator<<(std::ostream& ostream, const Parameter& parameter)
{
	ostream << "(" << parameter.m_value << "," << parameter.m_mode << ")";
	return ostream;
}
using Parameters = std::vector<Parameter>;

struct Instruction
{
	OpCode m_opCode{OpCode::Halt};
	Parameters m_parameters;
};

std::ostream&
operator<<(std::ostream& ostream, const Instruction& instruction)
{
	ostream << "[ " << instruction.m_opCode ;
	for ( const auto& param : instruction.m_parameters )
	{
		ostream << param << " ";
	}
	ostream << "]";

	return ostream;
}

class Runtime
{
public:
	Runtime(Program&& program):
		m_program(std::move(program)),
		m_instructionPointer(m_program.begin()),
		m_isRunning(false)
	{}
	void run();
private:
	Instruction nextInstruction();
	void executeInstruction(const Instruction& instruction);
	int getParameter(const Parameter& parameter);

	Program m_program{};
	Program::iterator m_instructionPointer{};
	bool m_isRunning{};
};

void
Runtime::run()
{
	m_isRunning = true;
	while( m_isRunning )
	{
		Instruction instruction = nextInstruction();
		std::cout << instruction << std::endl;
		executeInstruction(instruction);
	}
}

Instruction
Runtime::nextInstruction()
{
	Instruction ret{};
	int instruction = *m_instructionPointer++;
	ret.m_opCode = static_cast<OpCode>(instruction % 100);
	instruction = instruction / 100;
	ret.m_parameters.resize( numParams(ret.m_opCode) );
	for ( auto& param : ret.m_parameters )
	{
		int value = *m_instructionPointer++;
		ParameterMode parameterMode = static_cast<ParameterMode>(instruction % 10);
		param = Parameter{value, parameterMode};
		instruction = instruction / 10;
	}
	return ret;
}

void
Runtime::executeInstruction(const Instruction& instruction)
{
	switch ( instruction.m_opCode )
	{
	case OpCode::Add:
		m_program[ instruction.m_parameters[2].m_value ] =
			getParameter(instruction.m_parameters[0]) + getParameter(instruction.m_parameters[1]);
		break;
	case OpCode::Mult:
		m_program[ instruction.m_parameters[2].m_value ] =
			getParameter(instruction.m_parameters[0]) * getParameter(instruction.m_parameters[1]);
		break;
	case OpCode::Input:
		std::cin >> m_program[ instruction.m_parameters[0].m_value ];
		break;
	case OpCode::Output:
		std::cout << getParameter(instruction.m_parameters[0]) << std::endl;
		break;
	case OpCode::Halt:
		m_isRunning = false;
		break;
	}
}

int
Runtime::getParameter(const Parameter& parameter)
{
	if ( parameter.m_mode == ParameterMode::Position )
	{
		return m_program[parameter.m_value];
	}
	else
	{
		return parameter.m_value;
	}
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
