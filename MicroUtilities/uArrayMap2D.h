#pragma once

#include "Vec2.h"
#include <iterator>
#include <stdint.h>

class uArrayMap2D;

class uArrayMap2DAxisIterator
{
public:
	using value_type = int32_t;
	using difference_type = ptrdiff_t;

	uArrayMap2DAxisIterator();
	explicit uArrayMap2DAxisIterator(int32_t current);

	uArrayMap2DAxisIterator& operator--();
	uArrayMap2DAxisIterator operator--(int);

	uArrayMap2DAxisIterator& operator++();
	uArrayMap2DAxisIterator operator++(int);

	int32_t operator*() const;

private:
	int32_t m_current;
};

bool operator==(const uArrayMap2DAxisIterator& a, const uArrayMap2DAxisIterator& b);
bool operator!=(const uArrayMap2DAxisIterator& a, const uArrayMap2DAxisIterator& b);

static_assert(std::input_or_output_iterator<uArrayMap2DAxisIterator>);

class uArrayMap2DAxis
{
public:
	uArrayMap2DAxis(int32_t begin, int32_t end);

	uArrayMap2DAxisIterator begin() const;
	uArrayMap2DAxisIterator end() const;
private:
	int32_t m_begin;
	int32_t m_end;
};

static_assert(std::ranges::input_range<uArrayMap2DAxis>);

class uArrayMap2DGridIterator
{
public:
	using difference_type = std::ptrdiff_t;
	using value_type = std::pair<Vec2Int, char>;

	uArrayMap2DGridIterator();
	uArrayMap2DGridIterator(const uArrayMap2D* arrayMap);

	uArrayMap2DGridIterator& operator--();
	uArrayMap2DGridIterator operator--(int);

	uArrayMap2DGridIterator& operator++();
	uArrayMap2DGridIterator operator++(int);

	const value_type& operator*() const
	{
		return m_cachedValue;
	}

	bool operator==(const uArrayMap2DGridIterator& other) const;

private:
	void UpdateCache();
	bool IsEnd() const;

	const uArrayMap2D* m_arrayMap;
	int32_t m_current;
	value_type m_cachedValue;
};

bool operator!=(const uArrayMap2DGridIterator& a, const uArrayMap2DGridIterator& b);

static_assert(std::input_or_output_iterator<uArrayMap2DGridIterator>);

class uArrayMap2DGrid
{
public:
	uArrayMap2DGrid(const uArrayMap2D* arrayMap);

	uArrayMap2DGridIterator begin() const;
	uArrayMap2DGridIterator end() const;
private:
	const uArrayMap2D* m_map;
};

static_assert(std::ranges::input_range<uArrayMap2DGrid>);

enum class uArrayMap2DOptions
{
	CloneAsNull,
	CloneAsInvalid,
	CloneAsUninitialised
};

class uArrayMap2D
{
public:

	uArrayMap2D(Vec2Int origin, int32_t width, int32_t height, char invalid);
	uArrayMap2D(Vec2Int origin, int32_t width, int32_t height, char* data, char invalid);
	uArrayMap2D(const uArrayMap2D& other);
	uArrayMap2D(uArrayMap2DOptions options, const uArrayMap2D& other);
	uArrayMap2D(uArrayMap2D&& other) noexcept;
	~uArrayMap2D();

	uArrayMap2D& operator=(const uArrayMap2D& other);
	uArrayMap2D& operator=(uArrayMap2D&& other) noexcept;

	char& operator()(Vec2Int p);
	char& operator()(int32_t x, int32_t y);

	const char& operator()(Vec2Int p) const;
	const char& operator()(int32_t x, int32_t y) const;

	uArrayMap2DAxis AxisRangeX() const;
	uArrayMap2DAxis AxisRangeY() const;

	Vec2Int GetOrigin() const;
	int32_t GetWidth() const;
	int32_t GetHeight() const;
	Vec2Int GetDimensions() const;

	bool IsInside(Vec2Int p) const;

	friend class uArrayMap2DGridIterator;
	uArrayMap2DGrid Grid() const;

	char GetInvalidCharacter() const;
	void Replace(char from, char to);

	int32_t Count(char value) const;

	void Print() const;
	void Save(const char* filename) const;

	std::vector<char> GetData() const;

private:

	int32_t GetDataSize() const;

	Vec2Int m_origin;
	int32_t m_width;
	int32_t m_height;

	char* m_pStorage;
	char m_invalid;
};

uArrayMap2D ReaduArrayMap(char emptyChar = '.');
