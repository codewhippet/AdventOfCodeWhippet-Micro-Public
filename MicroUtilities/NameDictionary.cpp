#include "stdafx.h"
#include "NameDictionary.h"

NameDictionary::NameDictionary(int32_t hashTableSize)
	: NameToId(hashTableSize, "")
{
	IdToName.reserve(hashTableSize);
}

int32_t NameDictionary::IdFromName(const char* name)
{
	int32_t id;
	if (NameToId.TryFind(name, &id) == false)
	{
		id = static_cast<int32_t>(IdToName.size());
		IdToName.push_back(name);
		NameToId.Insert(IdToName.back().c_str(), id);
		
	}
	return id;
}

int32_t NameDictionary::IdFromName(const char* name) const
{
	return NameToId.FindOrDefault(name, -1);
}

const char* NameDictionary::NameFromId(int32_t id) const
{
	return IdToName[id].c_str();
}
