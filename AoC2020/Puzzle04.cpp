#include "stdafx.h"

using namespace std;

namespace Puzzle04_2020_Types
{
}

using namespace Puzzle04_2020_Types;

static bool TryGetIdAndValue(const char** line, pair<string, string>* idAndValue)
{
	idAndValue->first.clear();
	idAndValue->second.clear();

	const char* c = *line;
	while (isspace(*c))
	{
		c++;
	}

	bool foundSomething = false;
	if (*c)
	{
		foundSomething = true;

		// Field ID
		while (*c != ':')
		{
			idAndValue->first += *c++;
		}

		assert(*c == ':');
		c++;

		// Field value
		while (*c && !isspace(*c))
		{
			idAndValue->second += *c++;
		}
	}

	*line = c;

	return foundSomething;
}

void Puzzle04_A_2020()
{
	int32_t answer = 0;

	set<string> requiredFields = { "byr", "iyr", "eyr", "hgt", "hcl", "ecl", "pid", "cid" };

	set<string> seenFields = { "cid" };

	char line[128];
	while (PuzzleInput::PeekChar() != EOF)
	{
		int32_t lineSize = Parse::ReadLine(line, sizeof(line));

		if (lineSize == 0)
		{
			if (seenFields == requiredFields)
			{
				answer++;
			}
			seenFields.clear();
			seenFields.insert("cid");

			continue;
		}

		pair<string, string> idAndValue;

		const char* c = &line[0];
		while (TryGetIdAndValue(&c, &idAndValue))
		{
			seenFields.insert(idAndValue.first);
		}
	}

	if (seenFields == requiredFields)
	{
		answer++;
	}

	PuzzleOutput::Submit(2020, 4, 1, answer);
}

void Puzzle04_B_2020()
{
	int32_t answer = 0;

	const map<string, bool> requiredFields =
	{
		{ "byr", true },
		{ "iyr", true },
		{ "eyr", true },
		{ "hgt", true },
		{ "hcl", true },
		{ "ecl", true },
		{ "pid", true },
		{ "cid", true },
	};

	map<string, function<bool(const string&)>> validation;

	validation["byr"] = [](const string& s) { int year = stoi(s); return (year >= 1920) && (year <= 2002); };
	validation["iyr"] = [](const string& s) { int year = stoi(s); return (year >= 2010) && (year <= 2020); };
	validation["eyr"] = [](const string& s) { int year = stoi(s); return (year >= 2020) && (year <= 2030); };
	validation["hgt"] = [](const string& s)
		{
			int height = stoi(s);
			if (s.find("cm") != string::npos)
			{
				return (height >= 150) && (height <= 193);
			}
			if (s.find("in") != string::npos)
			{
				return (height >= 59) && (height <= 76);
			}
			return false;
		};
	validation["hcl"] = [](const string& s)
		{
			if (s.length() != 7)
				return false;

			if (s[0] != '#')
				return false;

			return count_if(s.begin() + 1, s.end(), [](char c) { return isxdigit(c); }) == 6;
		};
	validation["ecl"] = [](const string& s)
		{
			return
				(s == "amb") ||
				(s == "blu") ||
				(s == "brn") ||
				(s == "gry") ||
				(s == "grn") ||
				(s == "hzl") ||
				(s == "oth");
		};
	validation["pid"] = [](const string& s)
		{
			return count_if(s.begin(), s.end(), [](char c) { return isdigit(c); }) == 9;
		};
	validation["cid"] = [](const string&) { return true; };

	map<string, bool> seenFields = { { "cid", true } };

	char line[128];
	while (PuzzleInput::PeekChar() != EOF)
	{
		int32_t lineSize = Parse::ReadLine(line, sizeof(line));

		if (lineSize == 0)
		{
			if (seenFields == requiredFields)
			{
				answer++;
			}
			seenFields.clear();
			seenFields["cid"] = true;

			continue;
		}

		pair<string, string> idAndValue;

		const char* c = &line[0];
		while (TryGetIdAndValue(&c, &idAndValue))
		{
			seenFields[idAndValue.first] = validation[idAndValue.first](idAndValue.second);
		}
	}

	if (seenFields == requiredFields)
	{
		answer++;
	}

	PuzzleOutput::Submit(2020, 4, 2, answer);
}
