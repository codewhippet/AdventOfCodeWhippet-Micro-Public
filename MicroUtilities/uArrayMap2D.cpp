#include "stdafx.h"
#include <assert.h>

uArrayMap2D::uArrayMap2D(Vec2Int origin, int32_t width, int32_t height, char invalid)
{
	m_origin = origin;
	m_width = width;
	m_height = height;

	int32_t dataSize = GetDataSize();
	m_pStorage = static_cast<char*>(MemArena_Alloc(dataSize));
	memset(m_pStorage, invalid, dataSize);

	m_invalid = invalid;
}

uArrayMap2D::uArrayMap2D(Vec2Int origin, int32_t width, int32_t height, char* data, char invalid)
{
	m_origin = origin;
	m_width = width;
	m_height = height;

	m_pStorage = data;

	m_invalid = invalid;
}

uArrayMap2D::uArrayMap2D(const uArrayMap2D& other)
{
	m_origin = other.m_origin;
	m_width = other.m_width;
	m_height = other.m_height;

	int32_t dataSize = GetDataSize();
	m_pStorage = static_cast<char*>(MemArena_Alloc(dataSize));
	memcpy(m_pStorage, other.m_pStorage, dataSize);

	m_invalid = other.m_invalid;
}

uArrayMap2D::uArrayMap2D(uArrayMap2D&& other) noexcept
{
	m_origin = other.m_origin;
	m_width = other.m_width;
	m_height = other.m_height;

	m_pStorage = other.m_pStorage;
	other.m_pStorage = nullptr;

	m_invalid = other.m_invalid;
}

uArrayMap2D::uArrayMap2D(uArrayMap2DOptions options, const uArrayMap2D& other)
{
	m_origin = other.m_origin;
	m_width = other.m_width;
	m_height = other.m_height;

	int32_t dataSize = GetDataSize();
	m_pStorage = static_cast<char*>(MemArena_Alloc(dataSize));
	m_invalid = other.m_invalid;

	switch (options)
	{
	case uArrayMap2DOptions::CloneAsNull:
		memset(m_pStorage, 0, dataSize);
		break;
	case uArrayMap2DOptions::CloneAsInvalid:
		memset(m_pStorage, m_invalid, dataSize);
		break;

	case uArrayMap2DOptions::CloneAsUninitialised:
		break;
	}
}

uArrayMap2D::~uArrayMap2D()
{
	MemArena_Free(m_pStorage);
}

uArrayMap2D& uArrayMap2D::operator=(uArrayMap2D&& other) noexcept
{
	MemArena_Free(m_pStorage);

	m_origin = other.m_origin;
	m_width = other.m_width;
	m_height = other.m_height;

	m_pStorage = other.m_pStorage;
	other.m_pStorage = nullptr;

	m_invalid = other.m_invalid;

	return *this;
}

uArrayMap2D& uArrayMap2D::operator=(const uArrayMap2D& other)
{
	MemArena_Free(m_pStorage);

	m_origin = other.m_origin;
	m_width = other.m_width;
	m_height = other.m_height;

	int32_t dataSize = GetDataSize();
	m_pStorage = static_cast<char*>(MemArena_Alloc(dataSize));
	memcpy(m_pStorage, other.m_pStorage, dataSize);

	m_invalid = other.m_invalid;

	return *this;
}

char& uArrayMap2D::operator()(Vec2Int p)
{
	return this->operator()(p.X, p.Y);
}

char& uArrayMap2D::operator()(int32_t x, int32_t y)
{
	if (x < m_origin.X)
		return m_invalid;
	if (x >= (m_origin.X + m_width))
		return m_invalid;

	if (y < m_origin.Y)
		return m_invalid;
	if (y >= (m_origin.Y + m_height))
		return m_invalid;

	x = x - m_origin.X;
	y = y - m_origin.Y;

	int32_t rowStart = y * m_width;
	int32_t index = rowStart + x;

	assert(index >= 0 && index < GetDataSize());
	return m_pStorage[index];
}

const char& uArrayMap2D::operator()(Vec2Int p) const
{
	return this->operator()(p.X, p.Y);
}

const char& uArrayMap2D::operator()(int32_t x, int32_t y) const
{
	if (x < m_origin.X)
		return m_invalid;
	if (x >= (m_origin.X + m_width))
		return m_invalid;

	if (y < m_origin.Y)
		return m_invalid;
	if (y >= (m_origin.Y + m_height))
		return m_invalid;

	x = x - m_origin.X;
	y = y - m_origin.Y;

	int32_t rowStart = y * m_width;
	int32_t index = rowStart + x;

	assert(index >= 0 && index < GetDataSize());
	return m_pStorage[index];
}

uArrayMap2DAxis uArrayMap2D::AxisRangeX() const
{
	return uArrayMap2DAxis{ m_origin.X, m_origin.X + m_width };
}

uArrayMap2DAxis uArrayMap2D::AxisRangeY() const
{
	return uArrayMap2DAxis{ m_origin.Y, m_origin.Y + m_height };
}

Vec2Int uArrayMap2D::GetOrigin() const
{
	return m_origin;
}

int32_t uArrayMap2D::GetWidth() const
{
	return m_width;
}

int32_t uArrayMap2D::GetHeight() const
{
	return m_height;
}

Vec2Int uArrayMap2D::GetDimensions() const
{
	return { m_width, m_height };
}

bool uArrayMap2D::IsInside(Vec2Int p) const
{
	return
		(p.X >= m_origin.X) &&
		(p.X < (m_origin.X + m_width)) &&
		(p.Y >= m_origin.Y) &&
		(p.Y < (m_origin.Y + m_height));
}

uArrayMap2DGrid uArrayMap2D::Grid() const
{
	return uArrayMap2DGrid(this);
}

char uArrayMap2D::GetInvalidCharacter() const
{
	return m_invalid;
}

void uArrayMap2D::Replace(char from, char to)
{
	char* current = m_pStorage;
	char* end = current + GetDataSize();
	while (current != end)
	{
		if (*current == from)
		{
			*current = to;
		}
		current++;
	}
}

int32_t uArrayMap2D::Count(char value) const
{
	return static_cast<int32_t>(std::count(m_pStorage, m_pStorage + GetDataSize(), value));
}

void uArrayMap2D::Print() const
{
	std::string s;

	int32_t size = GetDataSize();
	for (int32_t i = 0; i < size; i++)
	{
		if ((i > 0) && (i % m_width) == 0)
		{
			s += '\n';
		}
		s += m_pStorage[i];
	}

	printf("%s\n", s.c_str());
}

void uArrayMap2D::Save(const char* filename) const
{
	std::string s;

	int32_t size = GetDataSize();
	for (int32_t i = 0; i < size; i++)
	{
		if ((i > 0) && (i % m_width) == 0)
		{
			s += '\n';
		}
		s += m_pStorage[i];
	}

	FILE* f = fopen(filename, "w");
	fprintf(f, "%s\n", s.c_str());
	fclose(f);
}

std::vector<char> uArrayMap2D::GetData() const
{
	return std::vector<char>(&m_pStorage[0], &m_pStorage[GetDataSize()]);
}

int32_t uArrayMap2D::GetDataSize() const
{
	return m_width * m_height;
}

uArrayMap2DAxisIterator::uArrayMap2DAxisIterator()
	: m_current(-1)
{
}

uArrayMap2DAxisIterator::uArrayMap2DAxisIterator(int32_t current)
	: m_current(current)
{
}

uArrayMap2DAxisIterator& uArrayMap2DAxisIterator::operator--()
{
	--m_current;
	return *this;
}

uArrayMap2DAxisIterator uArrayMap2DAxisIterator::operator--(int)
{
	uArrayMap2DAxisIterator old{ m_current };
	m_current--;
	return old;
}

uArrayMap2DAxisIterator& uArrayMap2DAxisIterator::operator++()
{
	++m_current;
	return *this;
}

uArrayMap2DAxisIterator uArrayMap2DAxisIterator::operator++(int)
{
	uArrayMap2DAxisIterator old{ m_current };
	m_current++;
	return old;
}

int32_t uArrayMap2DAxisIterator::operator*() const
{
	return m_current;
}

bool operator==(const uArrayMap2DAxisIterator& a, const uArrayMap2DAxisIterator& b)
{
	return (*a == *b);
}

bool operator!=(const uArrayMap2DAxisIterator& a, const uArrayMap2DAxisIterator& b)
{
	return !(a == b);
}

uArrayMap2DAxis::uArrayMap2DAxis(int32_t begin, int32_t end)
	: m_begin(begin)
	, m_end(end)
{
}

uArrayMap2DAxisIterator uArrayMap2DAxis::begin() const
{
	return uArrayMap2DAxisIterator{ m_begin };
}

uArrayMap2DAxisIterator uArrayMap2DAxis::end() const
{
	return uArrayMap2DAxisIterator{ m_end };
}

Vec2Int ReadArrayMapDimensions()
{
	Vec2Int size;
	while (PuzzleInput::NextLine())
	{
		size.Y++;
		while (PuzzleInput::GetChar() != '\n')
		{
			size.X++;
		}
	}

	return size;
}

uArrayMap2D ReaduArrayMap(char emptyChar)
{
	size_t allocated = 0;
	size_t used = 0;
	char* mapData = static_cast<char*>(MemArena_AllocLargest(&allocated));

	Vec2Int dimensions;
	while (PuzzleInput::NextLine())
	{
		dimensions.Y++;
		dimensions.X = 0;
		for (int c = PuzzleInput::GetChar(); c != '\n'; c = PuzzleInput::GetChar())
		{
			assert(used <= allocated);
			dimensions.X++;

			mapData[used++] = (char)c;
		}
	}

	MemArena_ShrinkAlloc(mapData, used);

	return uArrayMap2D(Vec2Int{ 0, 0 }, dimensions.X, dimensions.Y, mapData, emptyChar);
}

uArrayMap2DGridIterator::uArrayMap2DGridIterator()
	: m_arrayMap(nullptr)
	, m_current(0)
	, m_cachedValue({})
{
}

uArrayMap2DGridIterator::uArrayMap2DGridIterator(const uArrayMap2D* arrayMap)
	: m_arrayMap(arrayMap)
	, m_current(0)
{
	UpdateCache();
}

uArrayMap2DGridIterator& uArrayMap2DGridIterator::operator--()
{
	assert(m_current > 0);
	--m_current;
	UpdateCache();
	return *this;
}

uArrayMap2DGridIterator uArrayMap2DGridIterator::operator--(int)
{
	assert(m_current > 0);
	uArrayMap2DGridIterator old(*this);
	m_current--;
	UpdateCache();
	return old;
}

uArrayMap2DGridIterator& uArrayMap2DGridIterator::operator++()
{
	assert(m_arrayMap);
	assert(m_current < m_arrayMap->GetDataSize());
	++m_current;
	UpdateCache();
	return *this;
}

uArrayMap2DGridIterator uArrayMap2DGridIterator::operator++(int)
{
	assert(m_arrayMap);
	assert(m_current < m_arrayMap->GetDataSize());
	uArrayMap2DGridIterator old(*this);
	m_current++;
	UpdateCache();
	return old;
}

void uArrayMap2DGridIterator::UpdateCache()
{
	assert(m_arrayMap);

	int32_t x = m_current % m_arrayMap->m_width;
	int32_t y = m_current / m_arrayMap->m_width;

	m_cachedValue = { m_arrayMap->m_origin + Vec2Int{ x, y }, m_arrayMap->m_pStorage[m_current] };
}

bool uArrayMap2DGridIterator::IsEnd() const
{
	return (m_arrayMap == nullptr) || (m_current == m_arrayMap->GetDataSize());
}

bool uArrayMap2DGridIterator::operator==(const uArrayMap2DGridIterator& other) const
{
	if (IsEnd() && other.IsEnd())
		return true;

	return m_arrayMap == other.m_arrayMap && m_current == other.m_current;
}

bool operator!=(const uArrayMap2DGridIterator& a, const uArrayMap2DGridIterator& b)
{
	return !(a == b);
}

uArrayMap2DGrid::uArrayMap2DGrid(const uArrayMap2D* arrayMap)
	: m_map(arrayMap)
{
}

uArrayMap2DGridIterator uArrayMap2DGrid::begin() const
{
	return uArrayMap2DGridIterator(m_map);
}

uArrayMap2DGridIterator uArrayMap2DGrid::end() const
{
	return uArrayMap2DGridIterator();
}
