/*
 * LALR.h
 *
 *  Created on: 27.04.2015
 *      Author: Klemens
 */

#ifndef TAR_LALR_H_
#define TAR_LALR_H_

#include <tuple>
#include <stack>
#include <utility>
#include "DFA.h"

#include <iostream>

namespace Gpp {

namespace Lalr
{

using Dfa::value_tuple;
using Dfa::for_all;
using Dfa::contains;

template<typename T>
using Stack = std::stack<T, std::vector<T>>;

enum Action_Enum
{
	Shift = 1, ///<This action indicates the symbol is to be shifted. The Target field will contain the index of the state in the LALR State table that the parsing engine will advance to.
	Reduce = 2,///<This action denotes that the parser can reduce a rule. The Target field will contain the index of the rule in the Rule Table.
	Goto = 3,///<This action is used when a rule is reduced and the parser jumps to the state that represents the shifted nonterminal. The Target field will contain the index of the state in the LALR State table that the parsing engine will jump to after a reduction if completed.
	Accept = 4,///<When the parser encounters the Accept action for a given symbol, the source text is accepted as correct and complete. In this case, the Target field is not needed and should be ignored.
	Error = 5, ///<Rule not recognized
};

template<typename Action, typename First, typename ... Args>
bool for_first(Action action, std::tuple<First, Args...> tpl)
{
	if (action(First())) return true;
	return for_all(action, std::tuple<Args...>());
};

template<typename Action, typename First>
bool for_first(Action  action, std::tuple<First> tpl)
{
	return action(First());
};

template<typename Action>
bool for_first(Action  action, std::tuple<> tpl)
{
	return false;
};

template<typename Symbol_t, Symbol_t, typename Iterator = void>
struct SymbolType
{
	using Type = typename Iterator::Token;
};

template<typename State, typename Iterator, typename Token = typename Iterator::value_type>
bool execute_sm(Stack<Token>& stack, Iterator &itr, const Iterator &end);


template<typename Symbol_t, Symbol_t my_symbol, Symbol_t...Symbols>
struct Rule
{
	using symbols = value_tuple<Symbol_t, Symbols...>;
	template<typename State, typename Iterator,  typename ...Args>
	static bool reduce(State, Iterator &itr, const Iterator &end,
					const typename SymbolType<Symbol_t, Symbols, Iterator>::Type&... rems,
					Args&&... args)
	{
		using Obj = SymbolType<Symbol_t, my_symbol, Iterator>;

		return execute_sm<State>(itr, end, Obj(rems...), std::forward<Args>(args)...);
	}
};

template<int Index> struct RuleDef {};
template<int Index> struct StateDef {};
template<int Index> struct SymbolDef {};


template<int StateIndex, int SymbolIndex>
struct ShiftAction
{
	using ActionType = std::integral_constant<Action_Enum, Shift>;
	using State = typename StateDef<StateIndex>::State;
	using Symbol = typename SymbolDef<SymbolIndex>::Symbol;

};

template<int StateIndex, int SymbolIndex>
struct ReduceAction
{
	using ActionType = std::integral_constant<Action_Enum, Reduce>;
	using Rule = typename RuleDef<StateIndex>::Rule;
	using Symbol = typename SymbolDef<SymbolIndex>::Symbol;
};

template<int StateIndex, int SymbolIndex>
struct GotoAction
{
	using ActionType = std::integral_constant<Action_Enum,  Goto>;
	using State = typename StateDef<StateIndex>::State;
	using Symbol = typename SymbolDef<SymbolIndex>::Symbol;
};

template<int StateIndex, int SymbolIndex>
struct AcceptAction
{
	using ActionType = std::integral_constant<Action_Enum,  Accept>;
	using Symbol = typename SymbolDef<SymbolIndex>::Symbol;
};


struct AcceptTrow {};

template<typename ... Actions_>
struct State
{
	using Actions = std::tuple<Actions_...>;
};

template<typename State, typename Action, typename Iterator, typename Token = typename Iterator::value_type, typename ...Stack> //reduce by a symbol
bool execute_state(std::integral_constant<Action_Enum, Reduce>, Iterator &itr, const Iterator &end, Stack&&... stack)
{
	using Rule = typename Action::Rule;
	Rule::reduce(State(), itr, end, std::forward<Stack>(stack)...);
	return true;
}

template<typename State, typename Action, typename Iterator, typename Token = typename Iterator::value_type, typename ...Stack> //reduce by a symbol
bool execute_state(std::integral_constant<Action_Enum, Shift>, Iterator &itr, const Iterator &end, Stack&&... stack)
{
	return execute_sm<typename Action::State>(itr, end, *itr , std::forward<Stack>(stack)...);
}

template<typename State, typename Action, typename Iterator, typename Token = typename Iterator::value_type, typename ...Stack> //reduce by a symbol
bool execute_state(std::integral_constant<Action_Enum, Goto>, Iterator &itr, const Iterator &end, Stack&&... stack)
{
	return false;
}

template<typename State, typename Action, typename Iterator, typename Token = typename Iterator::value_type, typename ...Stack> //reduce by a symbol
bool execute_state(std::integral_constant<Action_Enum, Accept>, Iterator &itr, const Iterator &end, Stack&&... stack)
{
	throw AcceptTrow();
}



template<typename State, typename Iterator, typename Token = typename Iterator::value_type, typename ...Args>
bool execute_sm(Iterator &itr, const Iterator &end, Args&&... args)
{
//	bool repeat = false;
	bool found = false;
	do
	{
		found = for_first(
			[&](auto action) -> bool
			{
				using Action 	 = decltype(action);
				using Symbol 	 = typename Action::Symbol;
				using ActionType = typename Action::ActionType;
				if (Symbol::Id == itr->Id())
				{
					execute_state<State, Action>(ActionType(), itr, end, std::forward<Args>(args)...);
					return true;
				}
				else
					return false;
			}, typename State::Actions());

	}
	while(found);

	return found;
}

template<typename Token, int StartIndex = 0>//typename StartingState>
class StateMachine
{
public:
	using Start = typename StateDef<StartIndex>::State; //StartingState;
	template<typename Iterator>
	bool parse(Iterator &itr, const Iterator &end)
	{
		try
		{
			//using Type = typename Start::ActionType;
			execute_sm<Start>(itr, end);
		}
		catch (AcceptTrow&)
		{
			return true;
		}
		return false;

	}
};


#define LALR_MAKE_SYMBOL_TYPE(Symbol, Class) \
template<typename Iterator> \
struct SymbolType<decltype(Symbol), Symbol, Iterator> \
{ using Type = Class; }

#define LALR_MAKE_SYMBOL(Index, Symbol) \
template<> struct SymbolDef<Index> {static constexpr decltype(Symbol) Id = Symbol;};

#define LALR_MAKE_RULE(Index, Symbol, SubSymbols...) \
template<> struct RuleDef<Index> \
{using Rule = Rule<decltype(Symbol), Symbol, SubSymbols>;};

#define LALR_MAKE_STATE(Index, Actions...) \
template<> struct StateDef<Index> {using State = State<Actions>;};


#define LALR_MAKE_SHIFT_ACTION(Index, SymIndex)  ShiftAction<Index, SymIndex>
#define LALR_MAKE_REDUCE_ACTION(Index, SymIndex) ReduceAction<Index, SymIndex>
#define LALR_MAKE_GOTO_ACTION(Index, SymIndex) 	 GotoAction<Index, SymIndex>
#define LALR_MAKE_ACCEPT_ACTION(Index, SymIndex) AcceptAction<Index, SymIndex>



} /* namespace Egt */
}
#endif /* TAR_LALR_H_ */
