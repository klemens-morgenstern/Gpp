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


template<int I>
struct StateDef {}; //used to define a DFA State


template<int State_Id, typename CharType, CharType  ... Char_>
struct DFAEdge
{
	using Chars = value_tuple<CharType, Char_...>;
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

template<typename Symbol = int>
using DFAEmpty = State<false, Symbol, 0>;

template<typename ...T>
using DFATree = std::tuple<T...>;

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

				if (s.first && (itr > longest))
				{
					sym = s.second;
					longest = itr;
					accept = true;
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
#define DFA_MAKE_EDGE(StateId, Chars...) DFAEdge<StateId, decltype(DFA_GETFIRST(Chars...)), Chars...>

#define DFA_MAKE_NOACCEPT_STATE(Id, SymbolType, Edges...) \
template<> struct StateDef< Id > {using state = State<true, Symbol, SymbolType(), Edges... >;};

#define DFA_MAKE_ACCEPT_STATE(Id, Symbol, Edges...) \
template<> struct StateDef< Id > {using state = State<true, decltype(Symbol), Symbol, Edges... >;};


}
} /* namespace Egt */

#endif /* TAR_DFA_H_ */
