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

template<int id>
struct RulePP //rule parameter pack
{
	using Pack = std::tuple<>;
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


template<typename ... Actions_>
struct State
{
	using Actions = std::tuple<Actions_...>;
};

struct AcceptTrow {};


template<typename ...Rem>
struct Cutter
{
	template<typename Obj, typename State, typename Iterator,  typename ...Args>
	static bool cut(Iterator &itr, const Iterator &end, const Rem&... rems, Args&&... args)
	{
		return execute_sm<State>(itr, end, Obj(rems...), std::forward<Args>(args)...);
	}
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

template<typename Token, typename StartingState, typename Result>
class StateMachine
{
public:
	using Start = StartingState;
	template<typename Iterator>
	boost::optional<Result> parse(Iterator &itr, const Iterator &end)
	{
		return execute_sm<Result, Start>(itr, end);;

	}
};


} /* namespace Egt */
}
#endif /* TAR_LALR_H_ */
