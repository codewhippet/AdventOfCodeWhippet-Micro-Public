#include "stdafx.h"
#include "PuzzleInput.h"
#include "Hardware.h"
#include <assert.h>

#if _WIN32
#include <Windows.h>
#include <fcntl.h>
#include <io.h>
#endif

#ifdef PICO_ON_DEVICE
#include <pico/stdlib.h>
#endif

const size_t ReadaheadBufferSize = 64;

struct PuzzleInputImpl
{
	std::array<char, ReadaheadBufferSize> ReadaheadBuffer = {};

	bool HasInput = false;
	size_t SourceReadIndex = 0;
	size_t PuzzleReadIndex = 0;

#if _WIN32
	FILE* SourceFile = nullptr;
#endif
	const char* SourceString = nullptr;

	void Reset()
	{
		memset(ReadaheadBuffer.data(), 0, ReadaheadBuffer.size());

		HasInput = false;
		SourceReadIndex = 0;
		PuzzleReadIndex = 0;

		SourceString = nullptr;
	}
};

static PuzzleInputImpl Input;

static int pgetc_none();
static int pgetc_file();
static int pgetc_string();
static int pgetc_stdin();
static int pgetc_pipe();

static int (*pgetc)() = pgetc_none;

static int pgetc_none()
{
	return EOF;
}

static int pgetc_file()
{
#if _WIN32
	int c = fgetc(Input.SourceFile);
	if (c == EOF)
	{
		fclose(Input.SourceFile);
		Input.SourceFile = nullptr;
		pgetc = pgetc_none;
	}
	return c;
#else
	return EOF;
#endif
}

static int pgetc_string()
{
	int c = *Input.SourceString++;
	if (c == '\0')
	{
		Input.SourceString = nullptr;
		pgetc = pgetc_none;
		c = EOF;
	}
	return c;
}

static int pgetc_stdin()
{
	int c = getchar();
	if ((c == '\x17') || (c == EOF))
	{
		pgetc = pgetc_none;
		c = EOF;
	}
	return c;
}

static void FillReadaheadBuffer()
{
	while (Input.HasInput && (Input.SourceReadIndex < (Input.PuzzleReadIndex + sizeof(Input.ReadaheadBuffer))))
	{
		int c = (*pgetc)();
		if (c == EOF)
		{
			Input.HasInput = false;
			break;
		}
		Input.ReadaheadBuffer[Input.SourceReadIndex++ % sizeof(Input.ReadaheadBuffer)] = (char)c;
	}
}

static void Reset()
{
#if _WIN32
	if (Input.SourceFile)
	{
		fclose(Input.SourceFile);
	}
#endif
	Input.Reset();
}

void PuzzleInput::InitialiseStdIn()
{
#ifdef PICO_ON_DEVICE
	stdio_init_all();

	const uint32_t timing[] = { 1, 1, 3, 3, 3, 1, 3, 1, 3, 3, 3, 1, 1, 1, 3, 1, 1, 7 };
	size_t timingIndex = 0;

	bool ledState = true;
	while (!stdio_usb_connected())
	{
		Hardware::SetLedOn(ledState);
		sleep_ms(timing[timingIndex++] * 125);
		if (timingIndex >= (sizeof(timing) / sizeof(timing[0])))
			timingIndex = 0;
		ledState = !ledState;
	}

	Hardware::SetLedOn(false);
#elif _WIN32
	_setmode(_fileno(stdin), _O_BINARY);

	fflush(stdout);
	_setmode(_fileno(stdout), _O_BINARY);
#else
#	error "Unsupported platform"
#endif
}

void PuzzleInput::FromFile(int year, int puzzle)
{
#if _WIN32
	Reset();

	char filename[MAX_PATH];
	sprintf_s(filename, R"(.\AoCInput\%04d\Puzzle%02d.txt)", year, puzzle);
	Input.SourceFile = fopen(filename, "r");
	Input.HasInput = true;
	pgetc = pgetc_file;
	FillReadaheadBuffer();
#else
	assert(false);
#endif
}

void PuzzleInput::FromString(const char* dummy)
{
	Reset();

	Input.SourceString = dummy;
	Input.HasInput = true;
	pgetc = pgetc_string;
	FillReadaheadBuffer();
}

void PuzzleInput::FromStdIn(int* year, int* puzzle, int* part, int* size)
{
	Reset();

	char buffer[64] = {};
	int newlines = 0;
	for (int i = 0; i < sizeof(buffer) - 1; i++)
	{
		int c = getchar();
		buffer[i] = (char)c;
		newlines += (c == '\n');
		if (newlines == 2)
		{
			break;
		}
	}

	int scanned = sscanf(buffer, "PUZZLE %d %d %d\n%d\n", year, puzzle, part, size);
	if (scanned == 4)
	{
		// Puzzles and parts are 1-indexed on the protocol and 0-indexed internally
		(*puzzle)--;
		(*part)--;

		Input.HasInput = true;
		pgetc = pgetc_stdin;
		FillReadaheadBuffer();
	}
	else
	{
		for (const char* c = &buffer[0]; *c; c++)
		{
			putchar(*c);
		}
		putchar('\x17');

		Hardware::FlashingStop(4);

		*year = -1;
		*puzzle = -1;
		*part = -1;
	}
}

int PuzzleInput::GetChar()
{
	if (Input.PuzzleReadIndex == Input.SourceReadIndex)
		return EOF;

	int c = Input.ReadaheadBuffer[Input.PuzzleReadIndex++ % sizeof(Input.ReadaheadBuffer)];
	FillReadaheadBuffer();
	return c;
}

int PuzzleInput::PeekChar()
{
	if (Input.PuzzleReadIndex == Input.SourceReadIndex)
		return EOF;

	int c = Input.ReadaheadBuffer[Input.PuzzleReadIndex % sizeof(Input.ReadaheadBuffer)];
	FillReadaheadBuffer();
	return c;
}

void PuzzleInput::DropChar()
{
	Input.PuzzleReadIndex = std::min(Input.PuzzleReadIndex + 1, Input.SourceReadIndex);
	FillReadaheadBuffer();
}

bool PuzzleInput::NextLine()
{
	while (PeekChar() == '\n')
		DropChar();

	return PeekChar() != EOF;
}

void PuzzleInput::DropLine()
{
	for (int c = PeekChar(); (c != '\n') && (c != EOF); c = PeekChar())
		DropChar();
}

void PuzzleInput::DiscardRemaining()
{
	while (GetChar() != EOF)
		;
}
