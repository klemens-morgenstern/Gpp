/*
 * DFA.cpp
 *
 *  Created on: 24.04.2015
 *      Author: Klemens
 */

#include "DFA.h"
#include <iostream>

namespace Gpp
{
namespace Dfa
{

enum S
{
	s1 = 1,
	s2 = 2,
	s3 = 3,
	Eof = 4,
};

std::wstring to_wstring(S s)
{
	switch (s)
	{
	case s1:
		return L"s1";
	case s2:
		return L"s2";
	case s3:
		return L"s3";
	case Eof:
		return L"Eof";
	default:
		return L"default";
	}
}

#define DFA_GETFIRST(First, Zeugs...) First

#define DFA_MAKE_EDGE(StateId, Chars...) DFAEdge<StateId, decltype(DFA_GETFIRST(Chars...)), Chars...>

#define DFA_MAKE_ACCEPT_STATE(Id, Symbol, Edges...) \
template<> struct StateDef< Id > {using state = State<true, decltype(Symbol), Symbol, Edges... >;};

template<> struct StateDef<0> {	using state = State<false,  S, s1, DFAEdge<1, wchar_t, L'A', L'a', L'B', L'b'>, DFAEdge<2, wchar_t, L' '>>;};
template<> struct StateDef<1> {	using state = State<true,  S, s2, DFAEdge<1, wchar_t, L'A', L'a', L'B', L'b'>>;};
template<> struct StateDef<2> {	using state = State<true,  S, s3, DFAEdge<2, wchar_t, L' '>>;};

using Start = typename StateDef<0>::state;

void test()
{
	using namespace std;

	/*string str = "ABaababbaBabaab ABBBA";
	auto st = str.begin();
*/
	Lexer<wstring, typename StateDef<0>::state, true, Eof> l = L"ABaababbaBbaab  ABBBA";

	using Token = typename Lexer<wstring, typename StateDef<0>::state, true, Eof>::Token;
	l.Tokenize(
			[](Token t)
			{
				wcout << t.ToString() << " - " << to_wstring(t.Id()) << endl;
			}

				);




};
}
} /* namespace Egt */
