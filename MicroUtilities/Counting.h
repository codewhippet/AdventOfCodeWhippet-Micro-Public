#pragma once
#include <limits>
#include <utility>
#include <stdint.h>

template <typename VALUE_TYPE>
class MinValue
{
public:
	MinValue()
		: Value(std::numeric_limits<VALUE_TYPE>::max())
	{
	}

	void Update(VALUE_TYPE newValue)
	{
		if (newValue < Value)
		{
			Value = newValue;
		}
	}

	operator VALUE_TYPE() const
	{
		return Value;
	}

	VALUE_TYPE Get() const
	{
		return Value;
	}

private:
	VALUE_TYPE Value;
};

template <typename VALUE_TYPE>
class MaxValue
{
public:
	MaxValue()
		: Value(std::numeric_limits<VALUE_TYPE>::min())
	{
	}

	void Update(VALUE_TYPE newValue)
	{
		if (newValue > Value)
		{
			Value = newValue;
		}
	}

	operator VALUE_TYPE() const
	{
		return Value;
	}

	VALUE_TYPE Get() const
	{
		return Value;
	}

private:
	VALUE_TYPE Value;
};

template <typename VALUE_TYPE>
class MinMaxValues
{
public:
	MinMaxValues()
		: MinValue(std::numeric_limits<VALUE_TYPE>::max())
		, MaxValue(std::numeric_limits<VALUE_TYPE>::min())
	{
	}

	void Update(VALUE_TYPE newValue)
	{
		if (newValue < MinValue)
		{
			MinValue = newValue;
		}
		if (newValue > MaxValue)
		{
			MaxValue = newValue;
		}
	}

	operator std::pair<VALUE_TYPE, VALUE_TYPE>() const
	{
		return { MinValue, MaxValue };
	}

	VALUE_TYPE GetMin() const
	{
		return MinValue;
	}

	VALUE_TYPE GetMax() const
	{
		return MaxValue;
	}

private:
	VALUE_TYPE MinValue;
	VALUE_TYPE MaxValue;
};
