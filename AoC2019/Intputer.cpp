#include "stdafx.h"
#include "Intputer.h"
#include <Utils.h>

Intputer::Intputer(std::istream& input)
{
	ReadProgram(input);
}

Intputer::Intputer(const Intputer& other)
	: Program(other.Program)
	, PC(other.PC)
	, RB(other.RB)
	, ReadQueue(nullptr)
	, WriteQueue(nullptr)
{
}

void Intputer::ReadProgram(std::istream& input)
{
	Program.clear();
	for (int64_t inst : ReadAsVectorOfNumbers(ReadSingleLine(input)))
	{
		Program.insert({ (int64_t)Program.size(), inst });
	}
}

void Intputer::CopyProgram(const std::vector<int64_t>& program)
{
	Program.clear();
	for (int64_t i = 0; i < (int64_t)program.size(); i++)
	{
		Program.insert({ i, program[i] });
	}
}

void Intputer::Reset(const std::vector<int64_t>& program)
{
	CopyProgram(program);
	PC = 0;
	RB = 0;
	ReadQueue->clear();
	WriteQueue->clear();
}

int64_t Intputer::ReadParam(int64_t parameter, ParameterMode mode)
{
	switch (mode)
	{
	case ParameterMode::Position:
		return Program[parameter];

	case ParameterMode::Immediate:
		return parameter;

	case ParameterMode::Relative:
		return Program[parameter + RB];
	}

	return std::numeric_limits<int64_t>::min();
}

void Intputer::WriteParam(int64_t parameter, ParameterMode mode, int64_t value)
{
	switch (mode)
	{
	case ParameterMode::Position:
		Program[parameter] = value;
		break;

	case ParameterMode::Immediate:
		assert(false);
		break;

	case ParameterMode::Relative:
		Program[parameter + RB] = value;
		break;
	}


}

Intputer::ExecutionResult Intputer::Execute(int64_t breakAfter)
{
	int64_t instructionsExecuted = 0;
	while (true)
	{
		if (instructionsExecuted++ == breakAfter)
			return ExecutionResult::Breakpoint;

		int64_t instruction = Program[PC] % 100;
		ParameterMode param1Mode = ParameterMode((Program[PC] / 100) % 10);
		ParameterMode param2Mode = ParameterMode((Program[PC] / 1000) % 10);
		ParameterMode param3Mode = ParameterMode((Program[PC] / 10000) % 10);
		(void)param3Mode;

		switch (instruction)
		{
		case 99:
			return ExecutionResult::Finished;

		case 1:
			{
				int64_t a = ReadParam(Program[PC + 1], param1Mode);
				int64_t b = ReadParam(Program[PC + 2], param2Mode);
				WriteParam(Program[PC + 3], param3Mode, a + b);
				PC += 4;
			}
			break;

		case 2:
			{
				int64_t a = ReadParam(Program[PC + 1], param1Mode);
				int64_t b = ReadParam(Program[PC + 2], param2Mode);
				WriteParam(Program[PC + 3], param3Mode, a * b);
				PC += 4;
			}
			break;

		case 3:
			{
				if (ReadQueue == nullptr)
					return ExecutionResult::Exception;

				if (ReadQueue->empty())
					return ExecutionResult::PendingIo;

				int64_t inValue = ReadQueue->front();
				ReadQueue->pop_front();
				WriteParam(Program[PC + 1], param1Mode, inValue);
				PC += 2;
			}
			break;

		case 4:
			{
				if (WriteQueue == nullptr)
					return ExecutionResult::Exception;

				int64_t a = ReadParam(Program[PC + 1], param1Mode);
				WriteQueue->push_back(a);
				PC += 2;
			}
			break;

		case 5:
			{
				int64_t a = ReadParam(Program[PC + 1], param1Mode);
				int64_t b = ReadParam(Program[PC + 2], param2Mode);
				if (a != 0)
				{
					PC = b;
				}
				else
				{
					PC += 3;
				}
			}
			break;

		case 6:
			{
				int64_t a = ReadParam(Program[PC + 1], param1Mode);
				int64_t b = ReadParam(Program[PC + 2], param2Mode);
				if (a == 0)
				{
					PC = b;
				}
				else
				{
					PC += 3;
				}
			}
			break;

		case 7:
			{
				int64_t a = ReadParam(Program[PC + 1], param1Mode);
				int64_t b = ReadParam(Program[PC + 2], param2Mode);
				WriteParam(Program[PC + 3], param3Mode, (a < b));
				PC += 4;
			}
			break;

		case 8:
			{
				int64_t a = ReadParam(Program[PC + 1], param1Mode);
				int64_t b = ReadParam(Program[PC + 2], param2Mode);
				WriteParam(Program[PC + 3], param3Mode, (a == b));
				PC += 4;
			}
			break;

		case 9:
			{
				int64_t a = ReadParam(Program[PC + 1], param1Mode);
				RB += a;
				PC += 2;
			}
			break;

		default:
			return ExecutionResult::Exception;
		}
	}

	return ExecutionResult::Exception;
}

std::deque<int64_t>* Intputer::GetReadQueue()
{
	return ReadQueue;
}

std::deque<int64_t>* Intputer::GetWriteQueue()
{
	return WriteQueue;
}

void Intputer::SetReadQueue(std::deque<int64_t>* readQueue)
{
	ReadQueue = readQueue;
}

void Intputer::SetWriteQueue(std::deque<int64_t>* writeQueue)
{
	WriteQueue = writeQueue;
}

void Intputer::SetReadWriteQueues(std::deque<int64_t>* readQueue, std::deque<int64_t>* writeQueue)
{
	SetReadQueue(readQueue);
	SetWriteQueue(writeQueue);
}

void Intputer::SetReadWriteQueues(IntputerIO* io)
{
	SetReadWriteQueues(&io->Read, &io->Write);
}

IntputerWithIO::IntputerWithIO()
{
	SetReadWriteQueues(&Io);
}

IntputerWithIO::IntputerWithIO(const IntputerWithIO& other)
	: Intputer(other)
{
	SetReadWriteQueues(&Io);
}

IntputerWithIO::IntputerWithIO(IntputerWithIO&& other)
	: Intputer(other)
	, Io(std::move(other.Io))
{
	SetReadWriteQueues(&Io);
}

IntputerWithIO::IntputerWithIO(std::istream& input)
	: Intputer(input)
{
	SetReadWriteQueues(&Io);
}

// ----------------------------------------------------------------------------

template<typename REGISTER_TYPE>
uIntputer<REGISTER_TYPE>::uIntputer(size_t memorySize)
{
	ReadProgramFromInput(memorySize);
}

template<typename REGISTER_TYPE>
uIntputer<REGISTER_TYPE>::uIntputer(const uIntputer& other)
	: Program(other.Program)
	, PC(other.PC)
	, RB(other.RB)
	, ReadQueue(nullptr)
	, WriteQueue(nullptr)
{
}

template<typename REGISTER_TYPE>
void uIntputer<REGISTER_TYPE>::ReadProgramFromInput(size_t memorySize)
{
	Program.clear(); // Do we need this?
	assert(Program.empty()); // Check

	Program.reserve(memorySize);
	while (PuzzleInput::NextLine())
	{
		REGISTER_TYPE inst = Parse::GetInt32();
		Program.push_back(inst);
	}
}

template<typename REGISTER_TYPE>
void uIntputer<REGISTER_TYPE>::CopyProgram(const std::vector<REGISTER_TYPE>& program)
{
	Program = program;
}

template<typename REGISTER_TYPE>
void uIntputer<REGISTER_TYPE>::Reset(const std::vector<REGISTER_TYPE>& program)
{
	CopyProgram(program);
	PC = 0;
	RB = 0;
	ReadQueue->clear();
	WriteQueue->clear();
}

template<typename REGISTER_TYPE>
REGISTER_TYPE uIntputer<REGISTER_TYPE>::ReadParam(REGISTER_TYPE parameter, ParameterMode mode)
{
	switch (mode)
	{
	case ParameterMode::Position:
		return Program[parameter];

	case ParameterMode::Immediate:
		return parameter;

	case ParameterMode::Relative:
		return Program[parameter + RB];
	}

	return std::numeric_limits<REGISTER_TYPE>::min();
}

template<typename REGISTER_TYPE>
void uIntputer<REGISTER_TYPE>::WriteParam(REGISTER_TYPE parameter, ParameterMode mode, REGISTER_TYPE value)
{
	switch (mode)
	{
	case ParameterMode::Position:
		Program[parameter] = value;
		break;

	case ParameterMode::Immediate:
		assert(false);
		break;

	case ParameterMode::Relative:
		Program[parameter + RB] = value;
		break;
	}


}

template<typename REGISTER_TYPE>
uIntputer<REGISTER_TYPE>::ExecutionResult uIntputer<REGISTER_TYPE>::Execute(REGISTER_TYPE breakAfter)
{
	REGISTER_TYPE instructionsExecuted = 0;
	while (true)
	{
		if (instructionsExecuted++ == breakAfter)
			return ExecutionResult::Breakpoint;

		REGISTER_TYPE instruction = Program[PC] % 100;
		ParameterMode param1Mode = ParameterMode((Program[PC] / 100) % 10);
		ParameterMode param2Mode = ParameterMode((Program[PC] / 1000) % 10);
		ParameterMode param3Mode = ParameterMode((Program[PC] / 10000) % 10);
		(void)param3Mode;

		switch (instruction)
		{
		case 99:
			return ExecutionResult::Finished;

		case 1:
			{
				REGISTER_TYPE a = ReadParam(Program[PC + 1], param1Mode);
				REGISTER_TYPE b = ReadParam(Program[PC + 2], param2Mode);
				WriteParam(Program[PC + 3], param3Mode, a + b);
				PC += 4;
			}
			break;

		case 2:
			{
				REGISTER_TYPE a = ReadParam(Program[PC + 1], param1Mode);
				REGISTER_TYPE b = ReadParam(Program[PC + 2], param2Mode);
				WriteParam(Program[PC + 3], param3Mode, a * b);
				PC += 4;
			}
			break;

		case 3:
			{
				if (ReadQueue == nullptr)
					return ExecutionResult::Exception;

				if (ReadQueue->empty())
					return ExecutionResult::PendingIo;

				REGISTER_TYPE inValue = ReadQueue->front();
				ReadQueue->pop_front();
				WriteParam(Program[PC + 1], param1Mode, inValue);
				PC += 2;
			}
			break;

		case 4:
			{
				if (WriteQueue == nullptr)
					return ExecutionResult::Exception;

				REGISTER_TYPE a = ReadParam(Program[PC + 1], param1Mode);
				WriteQueue->push_back(a);
				PC += 2;
			}
			break;

		case 5:
			{
				REGISTER_TYPE a = ReadParam(Program[PC + 1], param1Mode);
				REGISTER_TYPE b = ReadParam(Program[PC + 2], param2Mode);
				if (a != 0)
				{
					PC = b;
				}
				else
				{
					PC += 3;
				}
			}
			break;

		case 6:
			{
				REGISTER_TYPE a = ReadParam(Program[PC + 1], param1Mode);
				REGISTER_TYPE b = ReadParam(Program[PC + 2], param2Mode);
				if (a == 0)
				{
					PC = b;
				}
				else
				{
					PC += 3;
				}
			}
			break;

		case 7:
			{
				REGISTER_TYPE a = ReadParam(Program[PC + 1], param1Mode);
				REGISTER_TYPE b = ReadParam(Program[PC + 2], param2Mode);
				WriteParam(Program[PC + 3], param3Mode, (a < b));
				PC += 4;
			}
			break;

		case 8:
			{
				REGISTER_TYPE a = ReadParam(Program[PC + 1], param1Mode);
				REGISTER_TYPE b = ReadParam(Program[PC + 2], param2Mode);
				WriteParam(Program[PC + 3], param3Mode, (a == b));
				PC += 4;
			}
			break;

		case 9:
			{
				REGISTER_TYPE a = ReadParam(Program[PC + 1], param1Mode);
				RB += a;
				PC += 2;
			}
			break;

		default:
			return ExecutionResult::Exception;
		}
	}

	return ExecutionResult::Exception;
}

template<typename REGISTER_TYPE>
std::deque<REGISTER_TYPE>* uIntputer<REGISTER_TYPE>::GetReadQueue()
{
	return ReadQueue;
}

template<typename REGISTER_TYPE>
std::deque<REGISTER_TYPE>* uIntputer<REGISTER_TYPE>::GetWriteQueue()
{
	return WriteQueue;
}

template<typename REGISTER_TYPE>
void uIntputer<REGISTER_TYPE>::SetReadQueue(std::deque<REGISTER_TYPE>* readQueue)
{
	ReadQueue = readQueue;
}

template<typename REGISTER_TYPE>
void uIntputer<REGISTER_TYPE>::SetWriteQueue(std::deque<REGISTER_TYPE>* writeQueue)
{
	WriteQueue = writeQueue;
}

template<typename REGISTER_TYPE>
void uIntputer<REGISTER_TYPE>::SetReadWriteQueues(std::deque<REGISTER_TYPE>* readQueue, std::deque<REGISTER_TYPE>* writeQueue)
{
	SetReadQueue(readQueue);
	SetWriteQueue(writeQueue);
}

template<typename REGISTER_TYPE>
void uIntputer<REGISTER_TYPE>::SetReadWriteQueues(uIntputerIO<REGISTER_TYPE>* io)
{
	SetReadWriteQueues(&io->Read, &io->Write);
}

template<typename REGISTER_TYPE>
uIntputerWithIO<REGISTER_TYPE>::uIntputerWithIO()
{
	uIntputer<REGISTER_TYPE>::SetReadWriteQueues(&Io);
}

template<typename REGISTER_TYPE>
uIntputerWithIO<REGISTER_TYPE>::uIntputerWithIO(const uIntputerWithIO<REGISTER_TYPE>& other)
	: uIntputer<REGISTER_TYPE>(other)
{
	uIntputer<REGISTER_TYPE>::SetReadWriteQueues(&Io);
}

template<typename REGISTER_TYPE>
uIntputerWithIO<REGISTER_TYPE>::uIntputerWithIO(uIntputerWithIO<REGISTER_TYPE>&& other)
	: uIntputer<REGISTER_TYPE>(other)
	, Io(std::move(other.Io))
{
	uIntputer<REGISTER_TYPE>::SetReadWriteQueues(&Io);
}

template<typename REGISTER_TYPE>
uIntputerWithIO<REGISTER_TYPE>::uIntputerWithIO(size_t memorySize)
	: uIntputer<REGISTER_TYPE>(memorySize)
{
	uIntputer<REGISTER_TYPE>::SetReadWriteQueues(&Io);
}

template struct uIntputerIO<int32_t>;
template class uIntputer<int32_t>;
template class uIntputerWithIO<int32_t>;
