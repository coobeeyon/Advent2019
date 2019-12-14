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
#include <queue>

using Program = std::vector<int>;

enum class OpCode : uint8_t
{
	Add    = 1,
	Mult   = 2,
	Input  = 3,
	Output = 4,
	JumpTrue = 5,
	JumpFalse = 6,
	LessThan = 7,
	Equals = 8,
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
	case OpCode::JumpTrue:
		ostream << "JumpTrue";
		break;
	case OpCode::JumpFalse:
		ostream << "JumpFalse";
		break;
	case OpCode::LessThan:
		ostream << "LessThan";
		break;
	case OpCode::Equals:
		ostream << "Equals";
		break;
	case OpCode::Halt:
		ostream << "Halt";
		break;
	default:
		ostream << "UNKNOWN("  << (int)opCode << ")";
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
	case OpCode::JumpTrue:
	case OpCode::JumpFalse:
		return 2;
	case OpCode::LessThan:
	case OpCode::Equals:
		return 3;
	case OpCode::Halt:
		return 0;
	}
	return 0;
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
	enum class State
	{
		Initialized,
		Running,
		AwaitingInput,
		Halted
	};
	Runtime(Program program):
		m_program(std::move(program))
	{}
	void run();
	bool isHalted() const;
	void addInput(int);
	int  getOutput();
private:
	Instruction nextInstruction();
	void executeInstruction(const Instruction& instruction);
	int getParameter(const Parameter& parameter);

	Program m_program{};
	int m_instructionPointer{};
	State m_state{State::Initialized};
	std::vector<int> m_inputQueue;
	std::vector<int> m_outputQueue;
};

void
Runtime::run()
{
	m_state = State::Running;
	while( m_state == State::Running )
	{
		Instruction instruction = nextInstruction();
		std::cout << instruction << std::endl;
		executeInstruction(instruction);
	}
}

bool
Runtime::isHalted() const
{
	return m_state == State::Halted;
}

void
Runtime::addInput(int value)
{
	m_inputQueue.push_back(value);
}

int
Runtime::getOutput()
{
	int value = m_outputQueue.front();
	m_outputQueue.erase(m_outputQueue.begin());
	return value;
}

Instruction
Runtime::nextInstruction()
{
	Instruction ret{};
	int instruction = m_program[m_instructionPointer++];
	ret.m_opCode = static_cast<OpCode>(instruction % 100);
	instruction = instruction / 100;
	ret.m_parameters.resize( numParams(ret.m_opCode) );
	for ( auto& param : ret.m_parameters )
	{
		int value = m_program[m_instructionPointer++];
		ParameterMode parameterMode = static_cast<ParameterMode>(instruction % 10);
		param = Parameter{value, parameterMode};
		instruction = instruction / 10;
	}
	return ret;
}

void
Runtime::executeInstruction(const Instruction& instruction)
{
	auto& params = instruction.m_parameters;
	switch ( instruction.m_opCode )
	{
	case OpCode::Add:
		m_program[ params[2].m_value ] =
			getParameter(params[0]) + getParameter(params[1]);
		break;
	case OpCode::Mult:
		m_program[ params[2].m_value ] =
			getParameter(params[0]) * getParameter(params[1]);
		break;
	case OpCode::Input:
		if ( m_inputQueue.empty() )
		{
			m_instructionPointer -= 2;
			m_state = State::AwaitingInput;
		}
		else
		{
			m_program[ params[0].m_value ] = m_inputQueue.front();
			m_inputQueue.erase(m_inputQueue.begin());
		}
		break;
	case OpCode::Output:
		m_outputQueue.push_back(getParameter(params[0]));
		break;
	case OpCode::JumpTrue:
		if ( getParameter(params[0]) != 0 )
		{
			m_instructionPointer = getParameter(params[1]);
		}
		break;
	case OpCode::JumpFalse:
		if ( getParameter(params[0]) == 0 )
		{
			m_instructionPointer = getParameter(params[1]);
		}
		break;
	case OpCode::LessThan:
		m_program[params[2].m_value] =
			( getParameter(params[0]) < getParameter(params[1]) ) ? 1 : 0;
		break;
	case OpCode::Equals:
		m_program[params[2].m_value] =
			( getParameter(params[0]) == getParameter(params[1]) ) ? 1 : 0;
		break;
	case OpCode::Halt:
		m_state = State::Halted;
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
		std::cout << &token[0] << ",";
	}
	std::cout << std::endl;
	return program;
}

using Phases = std::vector<int>;

int
getSignal(const Program& program, Phases phases)
{
	std::vector<Runtime> amplifiers(5, program);
	for ( auto& amplifier : amplifiers )
	{
		amplifier.addInput(phases.front());
		phases.erase(phases.begin());
	}
	int nextInput = 0;
	while (! amplifiers[1].isHalted() )
	{
		for ( auto& amplifier : amplifiers )
		{
			amplifier.addInput(nextInput);
			amplifier.run();
			nextInput = amplifier.getOutput();
		}
	}
	return nextInput;
}

int
main()
{
	auto program = loadProgram("Day7.input.txt");
	Phases phases = {5,6,7,8,9};
	Phases maxPhases = phases;
	int maxSignal = 0;
	do {
		int signal = getSignal(program, phases);
		if ( signal >= maxSignal )
		{
			maxPhases = phases;
			maxSignal = signal;
		}
	} while (std::next_permutation(phases.begin(), phases.end()));
	std::cout << "Phases ";
	for ( int phase : maxPhases )
	{
		std::cout << phase << ",";
	}
	std::cout << std::endl;
	std::cout << "Signal " << maxSignal << std::endl;
}
