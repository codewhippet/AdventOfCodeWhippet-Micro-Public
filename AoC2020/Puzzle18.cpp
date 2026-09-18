#include "stdafx.h"

using namespace std;

namespace Puzzle18_2020_Types
{
}

using namespace Puzzle18_2020_Types;

static string CovertToReversePolishNotation(const string& formula)
{
	string output;
	output.reserve(formula.size());

	stack<char> operatorStack;
	for (char c : formula)
	{
		if (isdigit(c))
		{
			output.append(1, c);
		}
		else if ((c == '+') || (c == '*'))
		{
			while ((operatorStack.empty() == false) && (operatorStack.top() != '('))
			{
				output.append(1, operatorStack.top());
				operatorStack.pop();
			}
			operatorStack.push(c);
		}
		else if (c == '(')
		{
			operatorStack.push(c);
		}
		else if (c == ')')
		{
			assert(operatorStack.empty() == false);
			while (operatorStack.top() != '(')
			{
				output.append(1, operatorStack.top());
				operatorStack.pop();
			}
			assert(operatorStack.top() == '(');
			operatorStack.pop();
		}
	}

	while (operatorStack.empty() == false)
	{
		output.append(1, operatorStack.top());
		operatorStack.pop();
	}

	return output;
}

static string CovertToReversePolishNotationSillyPrecedence(const string& formula)
{
	string output;
	output.reserve(formula.size());

	stack<char> operatorStack;
	for (char c : formula)
	{
		if (isdigit(c))
		{
			output.append(1, c);
		}
		else if (c == '+')
		{
			while ((operatorStack.empty() == false) && (operatorStack.top() != '(') && (operatorStack.top() != '*'))
			{
				output.append(1, operatorStack.top());
				operatorStack.pop();
			}
			operatorStack.push(c);
		}
		else if (c == '*')
		{
			while ((operatorStack.empty() == false) && (operatorStack.top() != '('))
			{
				output.append(1, operatorStack.top());
				operatorStack.pop();
			}
			operatorStack.push(c);
		}
		else if (c == '(')
		{
			operatorStack.push(c);
		}
		else if (c == ')')
		{
			assert(operatorStack.empty() == false);
			while (operatorStack.top() != '(')
			{
				output.append(1, operatorStack.top());
				operatorStack.pop();
			}
			assert(operatorStack.top() == '(');
			operatorStack.pop();
		}
	}

	while (operatorStack.empty() == false)
	{
		output.append(1, operatorStack.top());
		operatorStack.pop();
	}

	return output;
}

static int64_t EvaluateReversePolishNotationSum(const string& sum)
{
	stack<int64_t> evaluationStack;

	for (char c : sum)
	{
		if (isdigit(c))
		{
			evaluationStack.push(c - '0');
		}
		else if (c == '+')
		{
			int64_t a = evaluationStack.top();
			evaluationStack.pop();

			int64_t b = evaluationStack.top();
			evaluationStack.pop();

			evaluationStack.push(a + b);
		}
		else if (c == '*')
		{
			int64_t a = evaluationStack.top();
			evaluationStack.pop();

			int64_t b = evaluationStack.top();
			evaluationStack.pop();

			evaluationStack.push(a * b);
		}
	}

	assert(evaluationStack.size() == 1);
	return evaluationStack.top();
}

void Puzzle18_A_2020()
{
	vector<char> line(256);

	int64_t answer = 0;
	while (PuzzleInput::NextLine())
	{
		Parse::ReadNonEmptyLine(line.data(), line.size());

		string rpn = CovertToReversePolishNotation(line.data());
		answer += EvaluateReversePolishNotationSum(rpn);
	}

	PuzzleOutput::Submit(2020, 18, 1, answer);
}

void Puzzle18_B_2020()
{
	vector<char> line(256);

	int64_t answer = 0;
	while (PuzzleInput::NextLine())
	{
		Parse::ReadNonEmptyLine(line.data(), line.size());

		string rpn = CovertToReversePolishNotationSillyPrecedence(line.data());
		answer += EvaluateReversePolishNotationSum(rpn);
	}

	PuzzleOutput::Submit(2020, 18, 2, answer);
}
