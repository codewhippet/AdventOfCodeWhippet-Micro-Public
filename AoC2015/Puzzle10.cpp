#include "stdafx.h"

using namespace std;

// https://en.wikipedia.org/wiki/Look-and-say_sequence#Cosmological_decay
// http://www.se16.info/js/lands2.htm
#define H 1
#define He 2
#define Li 3
#define Be 4
#define B 5
#define C 6
#define N 7
#define O 8
#define F 9
#define Ne 10
#define Na 11
#define Mg 12
#define Al 13
#define Si 14
#define P 15
#define S 16
#define Cl 17
#define Ar 18
#define K 19
#define Ca 20
#define Sc 21
#define Ti 22
#define V 23
#define Cr 24
#define Mn 25
#define Fe 26
#define Co 27
#define Ni 28
#define Cu 29
#define Zn 30
#define Ga 31
#define Ge 32
#define As 33
#define Se 34
#define Br 35
#define Kr 36
#define Rb 37
#define Sr 38
#define Y 39
#define Zr 40
#define Nb 41
#define Mo 42
#define Tc 43
#define Ru 44
#define Rh 45
#define Pd 46
#define Ag 47
#define Cd 48
#define In 49
#define Sn 50
#define Sb 51
#define Te 52
#define I 53
#define Xe 54
#define Cs 55
#define Ba 56
#define La 57
#define Ce 58
#define Pr 59
#define Nd 60
#define Pm 61
#define Sm 62
#define Eu 63
#define Gd 64
#define Tb 65
#define Dy 66
#define Ho 67
#define Er 68
#define Tm 69
#define Yb 70
#define Lu 71
#define Hf 72
#define Ta 73
#define W 74
#define Re 75
#define Os 76
#define Ir 77
#define Pt 78
#define Au 79
#define Hg 80
#define Tl 81
#define Pb 82
#define Bi 83
#define Po 84
#define At 85
#define Rn 86
#define Fr 87
#define Ra 88
#define Ac 89
#define Th 90
#define Pa 91
#define U 92

#define NUM_ELEMENTS 92
#define DECAY_INTO_COUNT 6

namespace Puzzle10_2015_Types
{
	struct Element
	{
		uint8_t Name;
		const char* Structure;
	};

	struct Decay
	{
		uint8_t From;
		uint8_t To[DECAY_INTO_COUNT];
	};
}

using namespace Puzzle10_2015_Types;

static const Element Elements[NUM_ELEMENTS] =
{
	{ H, "22" },
	{ He, "13112221133211322112211213322112" },
	{ Li, "312211322212221121123222112" },
	{ Be, "111312211312113221133211322112211213322112" },
	{ B, "1321132122211322212221121123222112" },
	{ C, "3113112211322112211213322112" },
	{ N, "111312212221121123222112" },
	{ O, "132112211213322112" },
	{ F, "31121123222112" },
	{ Ne, "111213322112" },
	{ Na, "123222112" },
	{ Mg, "3113322112" },
	{ Al, "1113222112" },
	{ Si, "1322112" },
	{ P, "311311222112" },
	{ S, "1113122112" },
	{ Cl, "132112" },
	{ Ar, "3112" },
	{ K, "1112" },
	{ Ca, "12" },
	{ Sc, "3113112221133112" },
	{ Ti, "11131221131112" },
	{ V, "13211312" },
	{ Cr, "31132" },
	{ Mn, "111311222112" },
	{ Fe, "13122112" },
	{ Co, "32112" },
	{ Ni, "11133112" },
	{ Cu, "131112" },
	{ Zn, "312" },
	{ Ga, "13221133122211332" },
	{ Ge, "31131122211311122113222" },
	{ As, "11131221131211322113322112" },
	{ Se, "13211321222113222112" },
	{ Br, "3113112211322112" },
	{ Kr, "11131221222112" },
	{ Rb, "1321122112" },
	{ Sr, "3112112" },
	{ Y, "1112133" },
	{ Zr, "12322211331222113112211" },
	{ Nb, "1113122113322113111221131221" },
	{ Mo, "13211322211312113211" },
	{ Tc, "311322113212221" },
	{ Ru, "132211331222113112211" },
	{ Rh, "311311222113111221131221" },
	{ Pd, "111312211312113211" },
	{ Ag, "132113212221" },
	{ Cd, "3113112211" },
	{ In, "11131221" },
	{ Sn, "13211" },
	{ Sb, "3112221" },
	{ Te, "1322113312211" },
	{ I, "311311222113111221" },
	{ Xe, "11131221131211" },
	{ Cs, "13211321" },
	{ Ba, "311311" },
	{ La, "11131" },
	{ Ce, "1321133112" },
	{ Pr, "31131112" },
	{ Nd, "111312" },
	{ Pm, "132" },
	{ Sm, "311332" },
	{ Eu, "1113222" },
	{ Gd, "13221133112" },
	{ Tb, "3113112221131112" },
	{ Dy, "111312211312" },
	{ Ho, "1321132" },
	{ Er, "311311222" },
	{ Tm, "11131221133112" },
	{ Yb, "1321131112" },
	{ Lu, "311312" },
	{ Hf, "11132" },
	{ Ta, "13112221133211322112211213322113" },
	{ W, "312211322212221121123222113" },
	{ Re, "111312211312113221133211322112211213322113" },
	{ Os, "1321132122211322212221121123222113" },
	{ Ir, "3113112211322112211213322113" },
	{ Pt, "111312212221121123222113" },
	{ Au, "132112211213322113" },
	{ Hg, "31121123222113" },
	{ Tl, "111213322113" },
	{ Pb, "123222113" },
	{ Bi, "3113322113" },
	{ Po, "1113222113" },
	{ At, "1322113" },
	{ Rn, "311311222113" },
	{ Fr, "1113122113" },
	{ Ra, "132113" },
	{ Ac, "3113" },
	{ Th, "1113" },
	{ Pa, "13" },
	{ U, "3" },
};

static const Decay DecayRules[NUM_ELEMENTS] =
{
	{ H, { H } },
	{ He, { Hf, Pa, H, Ca, Li } },
	{ Li, { He } },
	{ Be, { Ge, Ca, Li } },
	{ B, { Be } },
	{ C, { B } },
	{ N, { C } },
	{ O, { N } },
	{ F, { O } },
	{ Ne, { F } },
	{ Na, { Ne } },
	{ Mg, { Pm, Na } },
	{ Al, { Mg } },
	{ Si, { Al } },
	{ P, { Ho, Si } },
	{ S, { P } },
	{ Cl, { S } },
	{ Ar, { Cl } },
	{ K, { Ar } },
	{ Ca, { K } },
	{ Sc, { Ho, Pa, H, Ca, Co } },
	{ Ti, { Sc } },
	{ V, { Ti } },
	{ Cr, { V } },
	{ Mn, { Cr, Si } },
	{ Fe, { Mn } },
	{ Co, { Fe } },
	{ Ni, { Zn, Co } },
	{ Cu, { Ni } },
	{ Zn, { Cu } },
	{ Ga, { Eu, Ca, Ac, H, Ca, Zn } },
	{ Ge, { Ho, Ga } },
	{ As, { Ge, Na } },
	{ Se, { As } },
	{ Br, { Se } },
	{ Kr, { Br } },
	{ Rb, { Kr } },
	{ Sr, { Rb } },
	{ Y, { Sr, U } },
	{ Zr, { Y, H, Ca, Tc } },
	{ Nb, { Er, Zr } },
	{ Mo, { Nb } },
	{ Tc, { Mo } },
	{ Ru, { Eu, Ca, Tc } },
	{ Rh, { Ho, Ru } },
	{ Pd, { Rh } },
	{ Ag, { Pd } },
	{ Cd, { Ag } },
	{ In, { Cd } },
	{ Sn, { In } },
	{ Sb, { Pm, Sn } },
	{ Te, { Eu, Ca, Sb } },
	{ I, { Ho, Te } },
	{ Xe, { I } },
	{ Cs, { Xe } },
	{ Ba, { Cs } },
	{ La, { Ba } },
	{ Ce, { La, H, Ca, Co } },
	{ Pr, { Ce } },
	{ Nd, { Pr } },
	{ Pm, { Nd } },
	{ Sm, { Pm, Ca, Zn } },
	{ Eu, { Sm } },
	{ Gd, { Eu, Ca, Co } },
	{ Tb, { Ho, Gd } },
	{ Dy, { Tb } },
	{ Ho, { Dy } },
	{ Er, { Ho, Pm } },
	{ Tm, { Er, Ca, Co } },
	{ Yb, { Tm } },
	{ Lu, { Yb } },
	{ Hf, { Lu } },
	{ Ta, { Hf, Pa, H, Ca, W } },
	{ W, { Ta } },
	{ Re, { Ge, Ca, W } },
	{ Os, { Re } },
	{ Ir, { Os } },
	{ Pt, { Ir } },
	{ Au, { Pt } },
	{ Hg, { Au } },
	{ Tl, { Hg } },
	{ Pb, { Tl } },
	{ Bi, { Pm, Pb } },
	{ Po, { Bi } },
	{ At, { Po } },
	{ Rn, { Ho, At } },
	{ Fr, { Rn } },
	{ Ra, { Fr } },
	{ Ac, { Ra } },
	{ Th, { Ac } },
	{ Pa, { Th } },
	{ U, { Pa } },
};

static int8_t FindElement(const char* structure)
{
	for (size_t i = 0; i < NUM_ELEMENTS; i++)
	{
		if (strcmp(Elements[i].Structure, structure) == 0)
		{
			return Elements[i].Name;
		}
	}
	return 0;
}

static void StepDecay(vector<int32_t>* from, vector<int32_t>* to)
{
	for (size_t decayRule = 0; decayRule < NUM_ELEMENTS; decayRule++)
	{
		const Decay& rule = DecayRules[decayRule];
		int32_t elementCount = (*from)[rule.From];
		if (elementCount > 0)
		{
			for (size_t decayInto = 0; decayInto < DECAY_INTO_COUNT; decayInto++)
			{
				(*to)[rule.To[decayInto]] += elementCount;
			}
		}
	}

	to->front() = 0; // Element 0 doesn't exist
}

static int32_t CountLength(const vector<int32_t>& counts)
{
	int32_t length = 0;
	for (size_t i = 0; i < NUM_ELEMENTS; i++)
	{
		int32_t elementLength = (int32_t)strlen(Elements[i].Structure);
		length += elementLength * counts[Elements[i].Name];
	}
	return length;
}

void Puzzle10_A_2015()
{
	char line[64];
	int32_t lineLength = Parse::ReadLine(line, sizeof(line));
	assert(lineLength > 0);
	(void)lineLength;

	const int8_t startingElement = FindElement(line);
	assert(startingElement > 0);

	vector<int32_t> counts[2];
	counts[0].resize(NUM_ELEMENTS + 1);
	counts[1].resize(NUM_ELEMENTS + 1);

	size_t activeCounts = 0;
	counts[activeCounts][startingElement] = 1;

	for (int i = 0; i < 40; i++)
	{
		size_t nextCounts = 1 - activeCounts;
		StepDecay(&counts[activeCounts], &counts[nextCounts]);
		ranges::fill(counts[activeCounts], 0);
		activeCounts = nextCounts;
	}

	int32_t answer = CountLength(counts[activeCounts]);

	return PuzzleOutput::Submit(2015, 10, 1, answer);
}

void Puzzle10_B_2015()
{
	char line[64];
	int32_t lineLength = Parse::ReadLine(line, sizeof(line));
	assert(lineLength > 0);
	(void)lineLength;

	const int8_t startingElement = FindElement(line);
	assert(startingElement > 0);

	vector<int32_t> counts[2];
	counts[0].resize(NUM_ELEMENTS + 1);
	counts[1].resize(NUM_ELEMENTS + 1);

	size_t activeCounts = 0;
	counts[activeCounts][startingElement] = 1;

	for (int i = 0; i < 50; i++)
	{
		size_t nextCounts = 1 - activeCounts;
		StepDecay(&counts[activeCounts], &counts[nextCounts]);
		ranges::fill(counts[activeCounts], 0);
		activeCounts = nextCounts;
	}

	int32_t answer = CountLength(counts[activeCounts]);

	return PuzzleOutput::Submit(2015, 10, 2, answer);
}
