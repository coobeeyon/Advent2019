#include <algorithm>
#include <array>
#include <cassert>
#include <charconv>
#include <fstream>
#include <iostream>
#include <iterator>
#include <numeric>
#include <optional>
#include <queue>
#include <sstream>
#include <string>
#include <vector>

using ProgramValue = long long;
using Program      = std::vector<ProgramValue>;

int shama = 5;
int lama  = 2;

enum class OpCode : uint8_t
{
	Add               = 1,
	Mult              = 2,
	Input             = 3,
	Output            = 4,
	JumpTrue          = 5,
	JumpFalse         = 6,
	LessThan          = 7,
	Equals            = 8,
	NudgeRelativeBase = 9,
	Halt              = 99
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
	case OpCode::NudgeRelativeBase:
		ostream << "NudgeRelativeBase";
		break;
	case OpCode::Halt:
		ostream << "Halt";
		break;
	default:
		ostream << "UNKNOWN(" << (int)opCode << ")";
	}
	return ostream;
}

int
numParams(OpCode opCode)
{
	switch (opCode)
	{
	case OpCode::Halt:
		return 0;
	case OpCode::Input:
	case OpCode::Output:
	case OpCode::NudgeRelativeBase:
		return 1;
	case OpCode::JumpTrue:
	case OpCode::JumpFalse:
		return 2;
	case OpCode::Add:
	case OpCode::Mult:
	case OpCode::LessThan:
	case OpCode::Equals:
		return 3;
	}
	return 0;
}

enum class ParameterMode : uint8_t
{
	Position         = 0,
	Value            = 1,
	RelativePosition = 2
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
	case ParameterMode::RelativePosition:
		ostream << "RelativePosition";
		break;
	}
	return ostream;
}

struct Parameter
{
	ProgramValue  m_value{};
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
	OpCode     m_opCode{OpCode::Halt};
	Parameters m_parameters;
};

std::ostream&
operator<<(std::ostream& ostream, const Instruction& instruction)
{
	ostream << "[ " << instruction.m_opCode;
	for (const auto& param : instruction.m_parameters)
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
	Runtime(Program program)
	    : m_program(std::move(program))
	{
	}
	void                        run();
	bool                        isHalted() const;
	void                        addInput(ProgramValue);
	std::optional<ProgramValue> getOutput();

	friend std::ostream& operator<<(std::ostream& stream, const Runtime& runtime);

private:
	Instruction  nextInstruction();
	void         executeInstruction(const Instruction& instruction);
	ProgramValue getParameter(const Parameter& parameter);
	void         setParameter(const Parameter& parameter, ProgramValue value);
	void         growProgramToIndex(int index);

	Program                   m_program{};
	int                       m_instructionPointer{};
	int                       m_relativeBase{};
	State                     m_state{State::Initialized};
	std::vector<ProgramValue> m_inputQueue;
	std::vector<ProgramValue> m_outputQueue;
};

std::ostream&
operator<<(std::ostream& stream, const Runtime& runtime)
{
	stream << "IP: " << runtime.m_instructionPointer << std::endl;
	stream << "RB: " << runtime.m_relativeBase << std::endl;
	stream << "State: " << (int)runtime.m_state << std::endl;
	stream << "Input: ";
	for (auto code : runtime.m_inputQueue)
	{
		stream << code << ",";
	}
	stream << std::endl;
	stream << "Output: ";
	for (auto code : runtime.m_outputQueue)
	{
		stream << code << ",";
	}
	stream << std::endl;
	stream << "Program: ";
	for (auto code : runtime.m_program)
	{
		stream << code << ",";
	}
	stream << std::endl;
	return stream;
}

void
Runtime::run()
{
	m_state = State::Running;
	while (m_state == State::Running)
	{
		Instruction instruction = nextInstruction();
		executeInstruction(instruction);
	}
}

bool
Runtime::isHalted() const
{
	return m_state == State::Halted;
}

void
Runtime::addInput(ProgramValue value)
{
	m_inputQueue.push_back(value);
}

std::optional<ProgramValue>
Runtime::getOutput()
{
	if (!m_outputQueue.empty())
	{
		ProgramValue value = m_outputQueue.front();
		m_outputQueue.erase(m_outputQueue.begin());
		return value;
	}
	return {};
}

Instruction
Runtime::nextInstruction()
{
	Instruction  ret{};
	ProgramValue instruction = m_program[m_instructionPointer++];
	ret.m_opCode             = static_cast<OpCode>(instruction % 100);
	instruction              = instruction / 100;
	ret.m_parameters.resize(numParams(ret.m_opCode));
	for (auto& param : ret.m_parameters)
	{
		ProgramValue  value         = m_program[m_instructionPointer++];
		ParameterMode parameterMode = static_cast<ParameterMode>(instruction % 10);
		param                       = Parameter{value, parameterMode};
		instruction                 = instruction / 10;
	}
	return ret;
}

void
Runtime::executeInstruction(const Instruction& instruction)
{
	auto& params = instruction.m_parameters;
	switch (instruction.m_opCode)
	{
	case OpCode::Add:
		setParameter(params[2],
		             getParameter(params[0]) + getParameter(params[1]));
		break;
	case OpCode::Mult:
		setParameter(params[2],
		             getParameter(params[0]) * getParameter(params[1]));
		break;
	case OpCode::Input:
		if (m_inputQueue.empty())
		{
			m_instructionPointer -= 2;
			m_state = State::AwaitingInput;
		}
		else
		{
			setParameter(params[0], m_inputQueue.front());
			m_inputQueue.erase(m_inputQueue.begin());
		}
		break;
	case OpCode::Output:
		m_outputQueue.push_back(getParameter(params[0]));
		break;
	case OpCode::JumpTrue:
		if (getParameter(params[0]) != 0)
		{
			m_instructionPointer = getParameter(params[1]);
		}
		break;
	case OpCode::JumpFalse:
		if (getParameter(params[0]) == 0)
		{
			m_instructionPointer = getParameter(params[1]);
		}
		break;
	case OpCode::LessThan:
		setParameter(params[2],
		             (getParameter(params[0]) < getParameter(params[1])) ? 1 : 0);
		break;
	case OpCode::Equals:
		setParameter(params[2],
		             (getParameter(params[0]) == getParameter(params[1])) ? 1 : 0);
		break;
	case OpCode::NudgeRelativeBase:
		m_relativeBase += getParameter(params[0]);
		break;
	case OpCode::Halt:
		m_state = State::Halted;
		break;
	}
}

void
Runtime::growProgramToIndex(int index)
{
	if (m_program.size() < index + 1)
	{
		m_program.resize(index + 1);
	}
}

void
Runtime::setParameter(const Parameter& parameter, ProgramValue value)
{
	int index = parameter.m_value;

	if (parameter.m_mode == ParameterMode::RelativePosition)
	{
		index += m_relativeBase;
	}
	growProgramToIndex(index);
	m_program[index] = value;
}

ProgramValue
Runtime::getParameter(const Parameter& parameter)
{
	if (parameter.m_mode != ParameterMode::Value)
	{
		int index = parameter.m_value;

		if (parameter.m_mode == ParameterMode::RelativePosition)
		{
			index += m_relativeBase;
		}
		growProgramToIndex(index);

		return m_program[index];
	}
	else
	{
		return parameter.m_value;
	}
}

Program
loadProgram(const std::string& fileName)
{
	Program       program;
	std::ifstream programFile(fileName);
	std::string   programLine;
	std::getline(programFile, programLine);
	std::istringstream   programLineStream(programLine);
	std::array<char, 64> token;
	while (programLineStream.getline(&token[0], 64, ','))
	{
		program.push_back(std::stoll(&token[0]));
		std::cout << &token[0] << ",";
	}
	std::cout << std::endl;
	return program;
}

int
main()
{
	auto    program = loadProgram("Day9.input.txt");
	Runtime runtime(program);
	runtime.addInput(2);
	runtime.run();
	while (auto output = runtime.getOutput())
	{
		std::cout << *output << ",";
	}
	std::cout << std::endl;
}
