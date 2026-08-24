#pragma once

#include "HashMap.h"
#include <string>
#include <stdint.h>

class NameDictionary
{
public:

	NameDictionary(int32_t hashTableSize);

	int32_t IdFromName(const char* name);
	int32_t IdFromName(const char* name) const;
	const char* NameFromId(int32_t id) const;

private:

	HashMap<std::string, int32_t> NameToId;
	std::vector<std::string> IdToName;
	
};
