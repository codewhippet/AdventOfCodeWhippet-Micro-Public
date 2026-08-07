#include "stdafx.h"

using namespace std;

namespace Puzzle12_2015_Types
{
	enum ElfJsonType : uint8_t
	{
		Integer,
		String,
		Object,
		Array,

		Unknown
	};

	enum class ParsingState : uint8_t
	{
		Value,

		Integer,

		String,

		Object_Start,
		Object_Next,
		Object_End,

		ObjectMember_Start,
		ObjectMember_End,

		Array_Start,
		Array_Next,
		Array_End,
	};

	struct ParsedValue
	{
		ElfJsonType Type = ElfJsonType::Unknown;
		bool ContainsRed = false;
		int32_t AccumulatedValue = 0;
	};

	struct Parser
	{
		vector<ParsingState> StateStack;
		vector<ParsedValue> Values;
	};
}

using namespace Puzzle12_2015_Types;

// Because Parsing JSON is a Minefield [https://seriot.ch/projects/parsing_json.html] we
// only parse the subset that's used in the puzzle input: ElfJson. There's no whitespace,
// no escaped characters in the strings, no constants other than positive and negative
// integers.

static void DropString(Parser*)
{
	assert(PuzzleInput::PeekChar() == '"');
	PuzzleInput::DropChar();

	while (PuzzleInput::GetChar() != '"')
		;
}

static void HandleValueStart(Parser* p)
{
	p->Values.push_back({ ElfJsonType::Unknown });

	switch (PuzzleInput::PeekChar())
	{
	case '[':
		p->StateStack.back() = ParsingState::Array_Start;
		break;

	case '{':
		p->StateStack.back() = ParsingState::Object_Start;
		break;

	case '"':
		p->StateStack.back() = ParsingState::String;
		break;

	case '-':
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		p->StateStack.back() = ParsingState::Integer;
		break;

	case EOF:
		p->StateStack.pop_back();
		break;
	}
}

static void HandleInteger(Parser* p)
{
	p->Values.back() = { ElfJsonType::Integer, false, Parse::GetInt32() };
	p->StateStack.pop_back();
}

static void HandleString(Parser* p)
{
	assert(PuzzleInput::PeekChar() == '"');
	PuzzleInput::DropChar();

	// Short strings only; could be turned into a state machine recogniser for "red" if needed
	char buffer[16];
	for (size_t i = 0; i < sizeof(buffer); i++)
	{
		int c = PuzzleInput::GetChar();
		assert(c != EOF);
		if (c == '"')
		{
			buffer[i] = '\0';
			break;
		}

		buffer[i] = (char)c;
	}

	p->Values.back() = { ElfJsonType::String, (strcmp(buffer, "red") == 0), 0 };
	p->StateStack.pop_back();
}

static void HandleObjectStart(Parser* p)
{
	assert(PuzzleInput::PeekChar() == '{');
	PuzzleInput::DropChar();

	p->Values.back().Type = ElfJsonType::Object;

	assert((PuzzleInput::PeekChar() == '"') || (PuzzleInput::PeekChar() == '}'));
	p->StateStack.back() = (PuzzleInput::PeekChar() == '"' ? ParsingState::ObjectMember_Start : ParsingState::Object_End);
}

static void HandleObjectMemberStart(Parser* p)
{
	assert(PuzzleInput::PeekChar() == '"');
	DropString(p); // Don't need the key

	assert(PuzzleInput::PeekChar() == ':');
	PuzzleInput::DropChar();

	p->StateStack.back() = ParsingState::ObjectMember_End;
	p->StateStack.push_back(ParsingState::Value);
}

static void HandleObjectMemberEnd(Parser* p)
{
	if ((p->Values.back().Type == ElfJsonType::String) && (p->Values.back().ContainsRed))
	{
		// Zero out this whole object's value if it has "red" as a member value
		p->Values.pop_back();
		p->Values.back().AccumulatedValue = 0;
		p->Values.back().ContainsRed = true;
	}
	else
	{
		// Otherwise it counts towards our value
		int32_t memberValue = p->Values.back().AccumulatedValue;
		p->Values.pop_back();

		if (p->Values.back().ContainsRed == false)
		{
			p->Values.back().AccumulatedValue += memberValue;
		}
	}

	p->StateStack.back() = ParsingState::Object_Next;
}

static void HandleObjectNext(Parser* p)
{
	assert((PuzzleInput::PeekChar() == ',') || (PuzzleInput::PeekChar() == '}'));
	if (PuzzleInput::PeekChar() == ',')
	{
		PuzzleInput::DropChar();
		p->StateStack.back() = ParsingState::ObjectMember_Start;
	}
	else
	{
		p->StateStack.back() = ParsingState::Object_End;
	}
}

static void HandleObjectEnd(Parser* p)
{
	assert(PuzzleInput::PeekChar() == '}');
	PuzzleInput::DropChar();

	// Zero out our contribution if we contained red
	if (p->Values.back().ContainsRed)
	{
		assert(p->Values.back().AccumulatedValue == 0);
		p->Values.back().AccumulatedValue = 0;
	}

	p->StateStack.pop_back();
}

static void HandleArrayStart(Parser* p)
{
	assert(PuzzleInput::PeekChar() == '[');
	PuzzleInput::DropChar();

	p->Values.back().Type = ElfJsonType::Array;

	if (PuzzleInput::PeekChar() == ']')
	{
		p->StateStack.back() = ParsingState::Array_End;
	}
	else
	{
		p->StateStack.back() = ParsingState::Array_Next;
		p->StateStack.push_back(ParsingState::Value);
	}
}

static void HandleArrayNext(Parser* p)
{
	// Add this to our tally
	int32_t memberValue = p->Values.back().AccumulatedValue;
	p->Values.pop_back();
	p->Values.back().AccumulatedValue += memberValue;

	assert((PuzzleInput::PeekChar() == ',') || (PuzzleInput::PeekChar() == ']'));
	if (PuzzleInput::PeekChar() == ']')
	{
		p->StateStack.back() = ParsingState::Array_End;
	}
	else
	{
		PuzzleInput::DropChar();
		p->StateStack.back() = ParsingState::Array_Next;
		p->StateStack.push_back(ParsingState::Value);
	}
}

static void HandleArrayEnd(Parser* p)
{
	assert(PuzzleInput::PeekChar() == ']');
	PuzzleInput::DropChar();
	p->StateStack.pop_back();
}

static int32_t EvaluateDocument()
{
	Parser p;
	p.StateStack.reserve(1024);
	p.Values.reserve(1024);

	p.StateStack.push_back(ParsingState::Value);
	while (p.StateStack.empty() == false)
	{
		switch (p.StateStack.back())
		{
		case ParsingState::Value: HandleValueStart(&p); break;

		case ParsingState::Integer: HandleInteger(&p); break;

		case ParsingState::String: HandleString(&p); break;

		case ParsingState::Object_Start: HandleObjectStart(&p); break;
		case ParsingState::Object_Next: HandleObjectNext(&p); break;
		case ParsingState::Object_End: HandleObjectEnd(&p); break;

		case ParsingState::ObjectMember_Start: HandleObjectMemberStart(&p); break;
		case ParsingState::ObjectMember_End: HandleObjectMemberEnd(&p); break;

		case ParsingState::Array_Start: HandleArrayStart(&p); break;
		case ParsingState::Array_Next: HandleArrayNext(&p); break;
		case ParsingState::Array_End: HandleArrayEnd(&p); break;
		}
	}

	assert((PuzzleInput::PeekChar() == '\n') || (PuzzleInput::PeekChar() == EOF));

	assert(p.Values.size() == 1);
	return p.Values.back().AccumulatedValue;
}

// ----------------------------------------------------------------------------------

void Puzzle12_A_2015()
{
	vector<char> line(48 * 1024);
	int32_t lineLength = Parse::ReadLine(line.data(), line.size());

	int32_t answer = 0;
	regex number(R"(-?\d+)");
	for (cregex_iterator it = cregex_iterator(&line[0], &line[lineLength], number); it != cregex_iterator{}; ++it)
	{
		answer += atoi(it->str().c_str());
	}

	return PuzzleOutput::Submit(2015, 12, 1, answer);
}

void Puzzle12_B_2015()
{
#if false && _WIN32 && _DEBUG
	PuzzleInput::FromString(R"()");
	assert(EvaluateDocument() == 0);

	PuzzleInput::FromString(R"(123)");
	assert(EvaluateDocument() == 123);
	PuzzleInput::FromString(R"(-123)");
	assert(EvaluateDocument() == -123);

	PuzzleInput::FromString(R"("123")");
	assert(EvaluateDocument() == 0);

	PuzzleInput::FromString(R"({})");
	assert(EvaluateDocument() == 0);
	PuzzleInput::FromString(R"({"a":123})");
	assert(EvaluateDocument() == 123);
	PuzzleInput::FromString(R"({"a":123,"b":-123})");
	assert(EvaluateDocument() == 0);

	PuzzleInput::FromString(R"({"a":123,"b":"123"})");
	assert(EvaluateDocument() == 123);
	PuzzleInput::FromString(R"({"a":123,"b":"red"})");
	assert(EvaluateDocument() == 0);
	PuzzleInput::FromString(R"({"a":"red","b":123})");
	assert(EvaluateDocument() == 0);

	PuzzleInput::FromString(R"([])");
	assert(EvaluateDocument() == 0);
	PuzzleInput::FromString(R"([123])");
	assert(EvaluateDocument() == 123);
	PuzzleInput::FromString(R"([123,123])");
	assert(EvaluateDocument() == 246);
	PuzzleInput::FromString(R"(["123"])");
	assert(EvaluateDocument() == 0);
	PuzzleInput::FromString(R"([{"a":123}])");
	assert(EvaluateDocument() == 123);

	PuzzleInput::FromString(R"([{"a":"red","b":123}])");
	assert(EvaluateDocument() == 0);
	PuzzleInput::FromString(R"(["red"])");
	assert(EvaluateDocument() == 0);
	PuzzleInput::FromString(R"([123,"red"])");
	assert(EvaluateDocument() == 123);
	PuzzleInput::FromString(R"(["red",123])");
	assert(EvaluateDocument() == 123);

	PuzzleInput::FromString(R"([1,2,3])");
	assert(EvaluateDocument() == 6);
	PuzzleInput::FromString(R"([1,{"c":"red","b":2},3])");
	assert(EvaluateDocument() == 4);
	PuzzleInput::FromString(R"({"d":"red","e":[1,2,3,4],"f":5})");
	assert(EvaluateDocument() == 0);
	PuzzleInput::FromString(R"([1,"red",5])");
	assert(EvaluateDocument() == 6);

	PuzzleInput::FromFile(2015, 12);
#endif

	int32_t answer = EvaluateDocument();

	return PuzzleOutput::Submit(2015, 12, 2, answer);
}

