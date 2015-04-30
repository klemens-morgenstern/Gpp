/*
 * DFA.h
 *
 *  Created on: 24.04.2015
 *      Author: Klemens
 */

#ifndef TAR_DFA_H_
#define TAR_DFA_H_

#include <tuple>
#include <stdexcept>
#include <vector>
#include <iostream>

namespace Gpp
{

struct LexerError : std::runtime_error
{
	using runtime_error::runtime_error;
};


namespace Dfa
{


template<typename Action, typename First, typename ... Args>
void for_all(Action action, std::tuple<First, Args...> tpl)
{
	action(First());
	for_all(action, std::tuple<Args...>());
};

template<typename Action, typename First>
void for_all(Action  action, std::tuple<First> tpl)
{
	action(First());
};

template<typename Action>
void for_all(Action  action, std::tuple<> tpl)
{
};

template<typename T, T...Values>
struct value_tuple
{

};


template<typename T, typename Type, Type First, Type ... Args>
bool contains(T value, value_tuple<Type, First, Args...> tpl)
{
	if (value == First)
		return true;
	else
		return contains(value, value_tuple<Type, Args...>());
};

template<typename T, typename Type, Type First>
bool contains(T  value, value_tuple<Type, First> tpl)
{
	return First == value;
};

template<typename T, typename U>
bool contains(T  value, value_tuple<U> tpl) {return false;};


//new version.

template<typename T, typename First, typename ... Args>
bool contains(T value, std::tuple<First, Args...> tpl)
{
	//if (First::Matches(value))
	if (match(value, First()))
		return true;
	else
		return contains(value, std::tuple<Args...>());
};

template<typename T, typename First>
bool contains(T  value, std::tuple<First> tpl)
{
	return match(value, First());
};

template<typename T>
bool contains(T  value, std::tuple<> tpl) {return false;};



template<int I>
struct StateDef {}; //used to define a DFA State

template<typename CharType, CharType Char_>
struct Char
{
};

template<typename CharType, CharType Begin, CharType End>
struct CharRange
{

};

template<typename In, typename CharType, CharType Char_>
bool match(In in, Char<CharType, Char_>)
{
	return in == Char_;
}

template<typename In, typename CharType, CharType Begin, CharType End>
bool match(In in, CharRange<CharType, Begin, End>)
{
	return (in >= Begin) && (in <= End);
}

template<int State_Id, typename ... Chars_>
struct DFAEdge
{
	using Chars = std::tuple<Chars_...>;
	using State = typename StateDef<State_Id>::state;

};

template<bool Accept_, typename Symbol_, Symbol_ Value_, typename ... Edges>
struct State
{
	using Symbol = Symbol_;
	static constexpr bool Accept() 	 {return Accept_;};
	static constexpr Symbol Value()  {return Value_;};
	using Edges_t = std::tuple<Edges...>;
};

template<typename Symbol_, Symbol_ ContainerSymbol_, Symbol_ StartSymbol_, Symbol_ EndSymbol_, bool AdvanceByChar_, bool ConsumeEnd_, typename ... Nested_>
struct Group
{
	using Symbol = Symbol_;
	constexpr static Symbol ContainerSymbol = ContainerSymbol_;
	constexpr static Symbol StartSymbol = StartSymbol_;
	constexpr static Symbol EndSymbol	= EndSymbol_;
	constexpr static bool AdvanceByChar = AdvanceByChar_;
	constexpr static bool ConsumeEnd 	= ConsumeEnd_;
	using Nested = std::tuple<Nested_...>;
};


template<typename State, typename Iterator>
auto execute_sm(Iterator &begin, const Iterator &end) -> std::pair<bool, typename State::Symbol>;


template<typename Iterator, typename Symbol_, Symbol_ ContainerSymbol_, Symbol_ StartSymbol_, Symbol_ EndSymbol_, bool AdvanceByChar, bool ConsumeEnd, typename ... Nested_>
auto execute_group(Iterator &begin, const Iterator &end, Group<Symbol_, ContainerSymbol_, StartSymbol_, EndSymbol_, AdvanceByChar, ConsumeEnd, Nested_...>) -> std::pair<bool, Symbol_>
{
	std::cout << "ERROR" << std::endl;
}

template<typename Start, typename Iterator, typename Symbol_, Symbol_ ContainerSymbol_, Symbol_ StartSymbol_, Symbol_ EndSymbol_, bool ConsumeEnd, typename ... Nested_>
auto execute_group(Iterator &begin, const Iterator &end, Group<Symbol_, ContainerSymbol_, StartSymbol_, EndSymbol_, true /*AdvanceByChar*/, ConsumeEnd, Nested_...>) -> std::pair<bool, Symbol_>
{
	auto itr = begin;

	while (itr != end)
	{
		auto it_sm = itr;
		auto p = execute_sm<Start>(it_sm, end);


		if (p.first && (p.second == EndSymbol_))
		{
			if (ConsumeEnd)
				begin = it_sm;
			else
				begin = itr;

			return {true, ContainerSymbol_};
		}
		else
			itr++;
	}

	return {false, Symbol_()};
}

template<int idx>
struct GroupDef {/*using group = NoGroup<Symbol_>;*/};

template<bool = false>
struct GroupList
{
	using Groups = std::tuple<>;
};

template<int ... DefIds>
struct GroupListHelper
{
	using Tuple = std::tuple<typename GroupDef<DefIds>::Group...>;
};


//template<typename StartState, typename Iterator, typename Symbol_>
//auto execute_group(Iterator &begin, const Iterator &end, NoGroup<Symbol_>) -> std::pair<bool, Symbol_>
//{
//	return {false, Symbol_()};
//}

template<typename StartState, typename Iterator, typename Symbol_, bool dummy = false>
auto look_for_group(Iterator &begin, const Iterator &end, Symbol_ sym) -> std::pair<bool, Symbol_>
{
	auto longest = begin;

	bool found = false;
	Symbol_ found_sym = Symbol_();

	for_all([&](auto grp)
			{
				auto itr = begin;
				using Group = decltype(grp);

				if (sym == Group::StartSymbol)
				{
					auto gp = execute_group<StartState>(itr, end, grp);
					if (gp.first && (itr > longest))
					{
						found = true;
						found_sym = gp.second;
						longest = itr;
					}
				}
			}, typename GroupList<dummy>::Groups());

	if (found)
		begin = longest;

	return {found, found_sym};
}

template<typename State, typename Iterator>
auto execute_sm(Iterator &begin, const Iterator &end) -> std::pair<bool, typename State::Symbol>
{
	if (begin == end) //end of stream
	{
		if (State::Accept())
			return {true, State::Value()};
		else
			return {false, typename State::Symbol()};
	}

	Iterator longest = begin; //the longest found.

	bool accept = State::Accept();
	typename State::Symbol sym = State::Value();

	for_all([&](auto sn)
		{
			using Edge = decltype(sn);
			if (contains(*begin, typename Edge::Chars()))
			{
				Iterator itr = begin + 1;
				auto s = execute_sm<typename Edge::State>(itr, end);

				if (s.first && (itr >= longest))
				{
					sym = s.second;
					longest = itr;
					accept = s.first;
				}
			}
		}, typename State::Edges_t());

	begin = longest;
	return {accept, sym};
}

template<typename State_>
struct Sm
{
	using Symbol = typename State_::Symbol;
	using Start = State_;

	template<typename Iterator>
	static auto getToken(Iterator &begin, const Iterator& end) -> typename Start::Symbol
	{
		auto p = execute_sm<Start>(begin, end);
		if (!p.first)
			throw LexerError("Token not recognized ");
		else
			return p.second;
	}

};

template<typename Buffer, typename Symbol_>
struct Token
{
	using iterator = typename Buffer::const_iterator;
private:
	iterator _begin;
	iterator _end;
	Symbol_ _id;
public:
	using Symbol = Symbol_;
	using StringType = std::basic_string<typename Buffer::value_type>;
	Symbol Id() const {return _id;}
	StringType ToString() const {return StringType(_begin, _end);}

	iterator begin() {return _begin();}
	iterator end() 	{return _end();}

	Token() = default;
	Token(iterator begin, iterator end, Symbol id) : _begin(begin), _end(end), _id(id) 	{}
	Token(const Token&) = default;
};

template<typename Buffer, typename Start_, bool SendEof = false, typename Start_::Symbol Eof = typename Start_::Symbol()>
class Lexer
{
public:
	const Buffer _buffer;
public:
	using Symbol = typename Start_::Symbol;
	using Start = Start_;
	using Token = Token<Buffer, typename Start_::Symbol>;

private:
	template<typename Func>
	void _SendEof(Func func, std::true_type) {func(Token(_buffer.begin(), _buffer.begin(), Eof));}
	template<typename Func>
	void _SendEof(Func, std::false_type) {}
public:
	template<typename ...Args>
	Lexer(Args && ...args) : _buffer(std::forward<Args>(args)...) {}

	template<typename Func>
	void Tokenize(Func func)
	{
		auto itr = _buffer.begin();

		const auto end = _buffer.end();

		auto match = false;
		do
		{
			auto beg = itr;
			auto p = execute_sm<Start>(beg, end);

			match = p.first && (beg != itr);
			if (match)
			{
				//ok, got one. Now: is the mother maybe a groupbegin?
				auto it = itr;
				auto gp = look_for_group<Start>(it, end, p.second);
				if (gp.first) //found a group
				{
					beg = it;
					func(Token(itr, beg, gp.second));
				}
				else
					func(Token(itr, beg, p.second));
				itr = beg;
			}
		}
		while (match);

		if (itr == end)
			_SendEof<Func>(func, std::integral_constant<bool, SendEof>());
	}
};

template<typename Buffer, typename Start_, bool SendEof = false, typename Start_::Symbol Eof = typename Start_::Symbol()>
class BufferedLexer : Lexer<Buffer, Start_, SendEof, Eof>
{
	std::vector<typename Lexer<Buffer, Start_, SendEof, Eof>::Token> _buf;
public:
	using iterator = typename std::vector<typename Lexer<Buffer, Start_, SendEof, Eof>::Token>::const_iterator;
	using Lexer<Buffer, Start_, SendEof, Eof>::Lexer;

	iterator begin() const {return _buf.cbegin();}
	iterator end() const {return _buf.cend();}

	void Tokenize()
	{
		Lexer<Buffer, Start_, SendEof, Eof>::Tokenize(
				[this](const typename Lexer<Buffer, Start_, SendEof, Eof>::Token& tk)
				{
					_buf.push_back(tk);
				});
	}

};

#define DFA_GETFIRST(First, Zeugs...) First
#define DFA_MAKE_EDGE(StateId, Chars...) DFAEdge<StateId, Chars>

#define DFA_MAKE_NOACCEPT_STATE(Id, SymbolType, Edges...) \
template<> struct StateDef< Id > {using state = State<false, SymbolType, static_cast<SymbolType>(0), Edges >;};

#define DFA_MAKE_ACCEPT_STATE(Id, Symbol, Edges...) \
template<> struct StateDef< Id > {using state = State<true, decltype(Symbol), Symbol, Edges >;};

///state without edge
#define DFA_MAKE_END_STATE(Id, Symbol)\
template<> struct StateDef< Id > {using state = State<true, decltype(Symbol), Symbol>;};

#define DFA_MAKE_NOEND_STATE(Id, Symbol)\
template<> struct StateDef< Id > {using state = State<true, decltype(Symbol), Symbol>;};

#define DFA_CHAR(Value) Char<decltype(Value), Value >
#define DFA_CHAR_RANGE(Lower, Upper) CharRange<decltype(Lower), Lower, Upper >


#define DFA_MAKE_GROUP_NESTED(Idx, ContainerSymbol,StartSymbol, EndSymbol, AdvanceByChar, ConsumeEnd, Nested...) \
template<> \
struct GroupDef<Idx> {using Group = Group<decltype(ContainerSymbol), ContainerSymbol, StartSymbol, EndSymbol, AdvanceByChar, ConsumeEnd, Nested >;};

#define DFA_MAKE_GROUP(Idx, ContainerSymbol, StartSymbol, EndSymbol, AdvanceByChar, ConsumeEnd) \
template<> \
struct GroupDef<Idx> {using Group = Group<decltype(ContainerSymbol), ContainerSymbol, StartSymbol, EndSymbol, AdvanceByChar, ConsumeEnd>;};

#define DFA_MAKE_GROUPLIST(Ids...) \
template<> \
struct GroupList<false> {using Groups = typename GroupListHelper<Ids>::Tuple;};



}
} /* namespace Egt */

#endif /* TAR_DFA_H_ */
