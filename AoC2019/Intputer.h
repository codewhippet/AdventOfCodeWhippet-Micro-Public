#pragma once

#include <istream>
#include <map>
#include <deque>
#include <vector>

struct IntputerIO
{
	std::deque<int64_t> Read;
	std::deque<int64_t> Write;
};

class Intputer
{
public:

	enum class ExecutionResult
	{
		Finished,
		PendingIo,
		Breakpoint,
		Exception,
	};

	Intputer() = default;
	Intputer(Intputer&&) = default;
	explicit Intputer(std::istream& input);

	virtual ~Intputer() = default;

	void ReadProgram(std::istream& input);
	void CopyProgram(const std::vector<int64_t>& program);
	void Reset(const std::vector<int64_t>& program);

	ExecutionResult Execute(int64_t breakAfter = std::numeric_limits<int64_t>::max());

	std::deque<int64_t>* GetReadQueue();
	std::deque<int64_t>* GetWriteQueue();

	void SetReadQueue(std::deque<int64_t>* readQueue);
	void SetWriteQueue(std::deque<int64_t>* writeQueue);
	void SetReadWriteQueues(std::deque<int64_t>* readQueue, std::deque<int64_t>* writeQueue);
	void SetReadWriteQueues(IntputerIO *io);

protected:

	Intputer(const Intputer& other);

	enum class ParameterMode
	{
		Position,
		Immediate,
		Relative,
	};

	int64_t ReadParam(int64_t parameter, ParameterMode mode);
	void WriteParam(int64_t parameter, ParameterMode mode, int64_t value);

	std::map<int64_t, int64_t> Program;
	int64_t PC = 0;
	int64_t RB = 0;

	std::deque<int64_t>* ReadQueue = nullptr;
	std::deque<int64_t>* WriteQueue = nullptr;
};

class IntputerWithIO : public Intputer
{
public:
	IntputerWithIO();
	IntputerWithIO(const IntputerWithIO& other);
	IntputerWithIO(IntputerWithIO&& other);
	explicit IntputerWithIO(std::istream& input);

private:
	IntputerIO Io;
};

// ----------------------------------------------------------------------------

template <typename REGISTER_TYPE>
struct uIntputerIO
{
	std::deque<REGISTER_TYPE> Read;
	std::deque<REGISTER_TYPE> Write;
};

extern template struct uIntputerIO<int32_t>;

template <typename REGISTER_TYPE>
class uIntputer
{
public:

	enum class ExecutionResult
	{
		Finished,
		PendingIo,
		Breakpoint,
		Exception,
	};

	uIntputer() = default;
	uIntputer(uIntputer&&) = default;
	explicit uIntputer(size_t memorySize);

	virtual ~uIntputer() = default;

	void ReadProgramFromInput(size_t memorySize);
	void CopyProgram(const std::vector<REGISTER_TYPE>& program);
	void Reset(const std::vector<REGISTER_TYPE>& program);

	ExecutionResult Execute(REGISTER_TYPE breakAfter = std::numeric_limits<REGISTER_TYPE>::max());

	std::deque<REGISTER_TYPE>* GetReadQueue();
	std::deque<REGISTER_TYPE>* GetWriteQueue();

	void SetReadQueue(std::deque<REGISTER_TYPE>* readQueue);
	void SetWriteQueue(std::deque<REGISTER_TYPE>* writeQueue);
	void SetReadWriteQueues(std::deque<REGISTER_TYPE>* readQueue, std::deque<REGISTER_TYPE>* writeQueue);
	void SetReadWriteQueues(uIntputerIO<REGISTER_TYPE>* io);

protected:

	uIntputer(const uIntputer& other);

	enum class ParameterMode
	{
		Position,
		Immediate,
		Relative,
	};

	REGISTER_TYPE ReadParam(REGISTER_TYPE parameter, ParameterMode mode);
	void WriteParam(REGISTER_TYPE parameter, ParameterMode mode, REGISTER_TYPE value);

	std::vector<REGISTER_TYPE> Program;
	REGISTER_TYPE PC = 0;
	REGISTER_TYPE RB = 0;

	std::deque<REGISTER_TYPE>* ReadQueue = nullptr;
	std::deque<REGISTER_TYPE>* WriteQueue = nullptr;
};

extern template class uIntputer<int32_t>;

template <typename REGISTER_TYPE>
class uIntputerWithIO : public uIntputer<REGISTER_TYPE>
{
public:
	uIntputerWithIO();
	uIntputerWithIO(const uIntputerWithIO& other);
	uIntputerWithIO(uIntputerWithIO&& other);
	explicit uIntputerWithIO(size_t memorySize);

private:
	uIntputerIO<REGISTER_TYPE> Io;
};

extern template class uIntputerWithIO<int32_t>;
