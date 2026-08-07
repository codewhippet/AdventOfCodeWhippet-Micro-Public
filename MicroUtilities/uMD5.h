#pragma once

#include <string>
#include <stdint.h>

namespace uMD5
{
	void Hash(const uint8_t* buffer, size_t bufferSize, uint8_t hash[16]);
	void Hash(const std::string_view& sv, uint8_t hash[16]);
	void Hash(const std::string& str, uint8_t hash[16]);

	union Chunk
	{
		char chars[64];
		uint8_t bytes[64];
		uint32_t dwords[16];
	};

	void HashChunkInPlace(Chunk* chunk, size_t chunkSize, uint8_t hash[16]);
	void HashMiniChunkInPlace(Chunk* chunk, size_t chunkSize, uint8_t hash[16]);
	uint32_t HashMiniChunkInPlaceMSBOnly(Chunk* chunk, size_t chunkSize);

	uint32_t HashMicroChunkInPlaceMSBOnly(Chunk* chunk, size_t chunkSize);

	void Fuzz();
}
