#pragma once

namespace PuzzleInput
{
	void InitialiseStdIn();

	void FromFile(int year, int puzzle);
	void FromString(const char* dummy);
	void FromStdIn(int* year, int* puzzle, int* part, int* size);

	int GetChar();
	int PeekChar();
	void DropChar();

	bool NextLine();
	void DropLine();

	void DiscardRemaining();
}
