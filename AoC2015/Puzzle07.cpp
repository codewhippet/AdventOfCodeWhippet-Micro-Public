#include "stdafx.h"

using namespace std;

namespace Puzzle07_2015_Types
{
	enum class Operation : uint8_t
	{
		Copy,
		And,
		Or,
		Not,
		LeftShift,
		RightShift,
	};

	struct Gate
	{
		uint32_t A;
		uint32_t B;
		Operation Op;
		uint32_t Out;
	};

	bool operator<(const Gate& a, const Gate& b)
	{
		return a.Out < b.Out;
	}

	struct Value
	{
		uint32_t Id;
		uint16_t Value;
	};

	bool operator<(const Value& a, const Value& b)
	{
		return a.Id < b.Id;
	}

	struct Circuit
	{
		vector<Gate> Gates;
		vector<Value> Values;

		Gate* FindGate(uint32_t outId)
		{
			Gate searchDummy;
			searchDummy.Out = outId;

			auto gateIt = ranges::lower_bound(Gates, searchDummy, less{});
			assert(gateIt != Gates.end());
			assert(gateIt->Out == outId);

			return &(*gateIt);
		}

		Value* FindValue(uint32_t id)
		{
			Value searchDummy{ id, 0 };
			auto valueIt = ranges::lower_bound(Values, searchDummy, less{});
			if (valueIt == Values.end())
			{
				return nullptr;
			}

			return (valueIt->Id == id ? &(*valueIt) : nullptr);
		}

		void SetValue(uint32_t id, uint16_t value);
	};
}

using namespace Puzzle07_2015_Types;

static uint32_t MakeId(const char* token)
{
	if (isdigit(token[0]))
	{
		return (uint32_t)atoll(token);
	}

	uint32_t id = 0;
	while (isalpha(token[0]))
	{
		id = (id << 8) | token[0];
		token++;
	}

	return id << 16;
}

static bool IsIdNumeric(uint32_t id)
{
	return id < 0x10000;
}

static uint16_t ExtractNumber(uint32_t id)
{
	return id & 0xffff;
}

void Circuit::SetValue(uint32_t id, uint16_t value)
{
	Value valuePair{ id, value };

	auto valueIt = ranges::lower_bound(Values, valuePair, less{});
	if ((valueIt == Values.end()) || (valueIt->Id != id))
	{
		Values.insert(valueIt, valuePair);
	}
}

static Gate ParseNot()
{
	const char* assignedFrom = strtok(nullptr, " ");
	const char* assignment = strtok(nullptr, " ");
	const char* assignedTo = strtok(nullptr, " ");

	assert(strcmp(assignment, "->") == 0);
	(void)assignment;

	return { MakeId(assignedFrom), 0, Operation::Not, MakeId(assignedTo) };
}

static Gate ParseCopy(const char* firstOperand)
{
	const char* assignedTo = strtok(nullptr, " ");

	return { MakeId(firstOperand), 0, Operation::Copy, MakeId(assignedTo) };
}

static Gate ParseLogicGate(const char* firstOperand, const char* operation)
{
	const char* secondOperand = strtok(nullptr, " ");
	const char* assignment = strtok(nullptr, " ");
	const char* assignedTo = strtok(nullptr, " ");

	assert(strcmp(assignment, "->") == 0);
	(void)assignment;

	Operation op;
	switch (operation[0])
	{
	default:
		assert(false);
	case 'A':
		op = Operation::And;
		break;
	case 'O':
		op = Operation::Or;
		break;
	case 'L':
		op = Operation::LeftShift;
		break;
	case 'R':
		op = Operation::RightShift;
		break;
	}

	return { MakeId(firstOperand), MakeId(secondOperand), op, MakeId(assignedTo) };
}

static Gate ParseAssignment(const char* firstToken)
{
	const char* secondToken = strtok(nullptr, " ");
	return (strcmp(secondToken, "->") == 0) ? ParseCopy(firstToken) : ParseLogicGate(firstToken, secondToken);
}

static Gate ParseLine(char* line)
{
	const char* token = strtok(line, " ");
	return (strcmp(token, "NOT") == 0) ? ParseNot() : ParseAssignment(token);
}

static void ParseCircuit(Circuit* c)
{
	c->Gates.reserve(512);
	while (PuzzleInput::NextLine())
	{
		char line[32];
		int32_t read = Parse::ReadLine(line, sizeof(line));
		assert(read > 0);
		(void)read;

		c->Gates.push_back(ParseLine(line));
	}
	ranges::sort(c->Gates, less{});

	c->Values.reserve(c->Gates.size());
}

static uint16_t Evaluate(const char* wire, Circuit* circuit)
{
	vector<uint32_t> evalStack;
	evalStack.reserve(circuit->Gates.size());
	evalStack.push_back(MakeId(wire));

	while (evalStack.empty() == false)
	{
		uint32_t gateId = evalStack.back();
		if (circuit->FindValue(gateId))
		{
			evalStack.pop_back();
			continue;
		}

		if (IsIdNumeric(gateId))
		{
			circuit->SetValue(gateId, ExtractNumber(gateId));
		}
		else
		{
			Gate* gate = circuit->FindGate(gateId);
			Value* a = nullptr;
			Value* b = nullptr;

			switch (gate->Op)
			{
			case Operation::Copy:
			case Operation::Not:
				a = circuit->FindValue(gate->A);
				b = a;
				break;

			case Operation::And:
			case Operation::Or:
			case Operation::LeftShift:
			case Operation::RightShift:
				a = circuit->FindValue(gate->A);
				b = circuit->FindValue(gate->B);
				break;
			}

			if (a == nullptr)
			{
				evalStack.push_back(gate->A);
				continue;
			}

			if (b == nullptr)
			{
				evalStack.push_back(gate->B);
				continue;
			}

			uint16_t value = 0;
			switch (gate->Op)
			{
			case Operation::Copy:
				value = a->Value;
				break;

			case Operation::And:
				value = a->Value & b->Value;
				break;

			case Operation::Or:
				value = a->Value | b->Value;
				break;

			case Operation::Not:
				value = ~a->Value;
				break;

			case Operation::LeftShift:
				value = a->Value << b->Value;
				break;

			case Operation::RightShift:
				value = a->Value >> b->Value;
				break;
			}

			circuit->SetValue(gateId, value);
		}

		evalStack.pop_back();
	}

	return circuit->FindValue(MakeId(wire))->Value;
}

void Puzzle07_A_2015()
{
	Circuit c;
	ParseCircuit(&c);

	uint16_t answer = Evaluate("a", &c);

	return PuzzleOutput::Submit(2015, 7, 1, int32_t(answer));
}

void Puzzle07_B_2015()
{
	Circuit c;
	ParseCircuit(&c);

	uint16_t a = Evaluate("a", &c);
	c.Values.clear();
	c.SetValue(MakeId("b"), a);
	uint16_t answer = Evaluate("a", &c);

	return PuzzleOutput::Submit(2015, 7, 2, int32_t(answer));
}

