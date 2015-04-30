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


template<typename Symbol_t, Symbol_t...Symbols>
struct Rule
{
	using symbols = value_tuple<Symbol_t, Symbols...>;
	template<typename ...Args>
	static void reduce(Args...) {};//todo: actions.
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

template<typename State, typename Iterator, typename Token = typename Iterator::value_type>
bool execute_sm(Stack<Token>& stack, Iterator &itr, const Iterator &end);

template<typename ... Actions_>
struct State
{
	using Actions = std::tuple<Actions_...>;
};

struct AcceptTrow {};

template<typename Action, typename Iterator, typename Token = typename Iterator::value_type> //reduce by a symbol
bool execute_state(std::integral_constant<Action_Enum, Reduce>, Stack<Token>& stack, Iterator &itr, const Iterator &end)
{
	stack.push(*itr);

	using Rule = typename Action::Rule;
	Rule::reduce(stack);
	return true;
}

template<typename Action, typename Iterator, typename Token = typename Iterator::value_type> //reduce by a symbol
bool execute_state(std::integral_constant<Action_Enum, Shift>, Stack<Token>& stack, Iterator &itr, const Iterator &end)
{
	stack.push(*itr);
	execute_sm<typename Action::State>(stack, itr, end);
	return false;

}

template<typename Action, typename Iterator, typename Token = typename Iterator::value_type> //reduce by a symbol
bool execute_state(std::integral_constant<Action_Enum, Goto>, Stack<Token>& stack, Iterator &itr, const Iterator &end)
{
	return false;
}

template<typename Action, typename Iterator, typename Token = typename Iterator::value_type> //reduce by a symbol
bool execute_state(std::integral_constant<Action_Enum, Accept>, Stack<Token>& stack, Iterator &itr, const Iterator &end)
{
	throw AcceptTrow();
}



template<typename State, typename Iterator, typename Token = typename Iterator::value_type>
bool execute_sm(Stack<Token>& stack, Iterator &itr, const Iterator &end)
{
	bool repeat = false;
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
					repeat = execute_state<Action>(ActionType(), stack, itr, end);
					return true;
				}
				else
					return false;
			}, typename State::Actions());

	}
	while(repeat && found);

	return found;
}

template<typename Token, typename StartingState>
class StateMachine
{
	Stack<Token> _stack{[]{std::vector<Token> v; v.reserve(1000); return v;}};
public:
	using Start = StartingState;
	template<typename Iterator>
	bool parse(Iterator &itr, const Iterator &end)
	{
		try
		{
			using Type = typename Start::ActionType;
			execute_sm<Start>(Type(), itr, end);
		}
		catch (AcceptTrow&)
		{
			return true;
		}
		return false;

	}
};


} /* namespace Egt */
}
#endif /* TAR_LALR_H_ */
