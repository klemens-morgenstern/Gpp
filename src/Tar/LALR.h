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
#include <boost/variant.hpp>
#include <boost/none.hpp>
#include <type_traits>
#include <utility>
#include <boost/ptr_container/ptr_vector.hpp>
#include <memory>
#include <typeinfo>

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

struct token_tag {};

template<int Id>
struct SymbolType
{
	using Type = boost::none_t;
};


struct MyVariant;

template<int Id, 	 typename Token> 	auto get(MyVariant &var) -> typename SymbolType<Id>::type&;
template<typename T, typename Token>   	  T& get(MyVariant &var);

template<typename T, typename Token>
struct getter
{
	static T& get(MyVariant& var);
	using type = T;
};

template<typename Token>
struct getter<token_tag, Token>
{
	static Token& get(MyVariant& var);
	using type = Token;
};

struct NoRuleAction
{
	using Type = boost::none_t;
	template<typename ...Args>
	static boost::none_t Action(Args...){return boost::none;}

};

template<int Id>
struct RuleAction : NoRuleAction//the type of the rule, i.e. what it returns.
{
};

template<typename Token>
struct TokenWrapper
{
	using Type = Token; //used for passing as RuleObject
};


template<typename Action_, typename Token, typename Context, typename ...RuleObject>
struct ExecuteRuleAction
{
	template<typename Stack>
	static void Reduce(Context& ctx, Stack& stack)
	{
		Build(ctx, stack, std::integral_constant<size_t, sizeof...(RuleObject)>());
	}

	template<typename Counter, typename Stack, typename ...Args>
	static void Build(Context& ctx, Stack& stack, Counter,  Args&&...args)
	{
		auto obj = std::move(stack.top());
		stack.pop();
		Build(ctx, stack, std::integral_constant<size_t, Counter::value-1>(), std::move(obj), std::move(args)...);
	}

	template<typename Stack, typename ...Args>
	static void Build(Context& ctx, Stack& stack, std::integral_constant<size_t, 0>, Args&&... args)
	{
		Action(ctx, stack, std::move(args)...);
	}
	template<typename Stack, typename ...Args>
	static void Action(Context& ctx, Stack & stack, Args && ...args)
	{
		auto res = std::move(
					Action_::Action(
							ctx,
							std::move(getter<RuleObject, Token>::get(args))...//TODO check if that works.
					));
		stack.emplace(std::move(res));
	}

};

template<typename Token, typename Context, typename ...RuleObject>
struct ExecuteRuleAction<NoRuleAction, Token, Context, RuleObject...>
{
	template<typename Stack>
	static void Reduce(Context& ctx, Stack& stack)
	{
		Build(stack, std::integral_constant<size_t, sizeof...(RuleObject)>());
	}
	template<typename Stack, typename Counter>
	static void Build(Stack& stack, Counter)
	{
		stack.pop();
		Build(stack, std::integral_constant<size_t, Counter::value - 1>());
	}
	template<typename Stack>
	static void Build(Stack& stack, std::integral_constant<size_t, 0>)
	{
		stack.emplace(boost::none);
	}
};

template<typename Context, typename Stack>
struct Rule
{
	const int Id;
	virtual int Reduce(Context& ctx, Stack &stack) = 0;
	Rule(int Id) : Id(Id) {}
	Rule(const Rule&) = default;
	virtual ~Rule() = default;
};

template<int Id, int SymId, typename Token, typename Context, typename Stack, int...RuleId>
struct RuleImpl : Rule<Context, Stack>
{
	using Action = RuleAction<Id>;
	using Type = typename SymbolType<SymId>::Type;
	static_assert(std::is_convertible<typename Action::Type, Type>::value, "Actiontype must be convertible to symbol type");

	RuleImpl() : Rule<Context, Stack>(SymId) {};
	RuleImpl(const RuleImpl&) = default;
	virtual int Reduce(Context& ctx, Stack &stack) override
	{
		ExecuteRuleAction<Action, Token, Context,  typename SymbolType<RuleId>::Type...>::Reduce(ctx, stack);
		return sizeof...(RuleId);
	}
	virtual ~RuleImpl() = default;
};
/*
template<typename Context, typename Stack, typename...RuleObject>
using NoActionRuleImpl = RuleImpl<Context, Stack, RuleObject...>;
*/
template<typename Context, typename Stack, typename Iterator>
struct State
{
	using Rule_ = Rule<Context, Stack>;
	struct Shift	{ State &state; Shift(State& st) : state(st) {};};
	struct Reduce 	{ Rule_ &rule;  Reduce(Rule_& rl) : rule(rl) {};};
	struct Goto		{ State &state; Goto(State& st) : state(st) {};};
	struct Accept   {};

	struct StateAction;
	struct ActionVisitor;
	const int Id;
	std::vector<StateAction> Actions;
	State(int Id, std::initializer_list<StateAction> init) : Id(Id), Actions(init) {}
	State() = default;
	State(const State&) = default;
};




template<typename Context, typename Stack, typename Iterator>
struct State<Context, Stack, Iterator>::StateAction
{
	using Token 	= typename Iterator::value_type;
	using Symbol_t 	= typename Token::Symbol;
	Symbol_t Id;
	boost::variant<Shift, Reduce, Goto, Accept> Action;

	bool Execute(Iterator &current, Context &ctx, Stack &stack, State<Context, Stack, Iterator>*& state, std::stack<State*> &state_stack,	Symbol_t &current_symbol)
	{
		ActionVisitor vis(current, ctx, stack, state, state_stack, current_symbol);
		return Action.apply_visitor(vis);
	}
	StateAction(Symbol_t Id, const Shift&  arg) : Id(Id), Action(arg) {};
	StateAction(Symbol_t Id, const Reduce& arg) : Id(Id), Action(arg) {};
	StateAction(Symbol_t Id, const Goto&   arg) : Id(Id), Action(arg) {};
	StateAction(Symbol_t Id, const Accept& arg) : Id(Id), Action(arg) {};
	StateAction(const StateAction&) = default;
	StateAction(StateAction&&) noexcept = default;
};

template<typename Context, typename Stack, typename Iterator>
struct State<Context, Stack, Iterator>::ActionVisitor : boost::static_visitor<bool>
{
	using Token = typename Iterator::value_type;
	using Symbol_t = typename Token::Symbol;

	Iterator &current;
	Context &ctx;
	Stack &stack;

	State<Context, Stack, Iterator>*& state;

	std::stack<State*> &state_stack;
	Symbol_t &current_symbol;


	ActionVisitor(Iterator &current, Context &ctx, Stack &stack, State<Context, Stack, Iterator>*& state, std::stack<State*> &state_stack,	Symbol_t &current_symbol)
		: current(current), ctx(ctx), stack(stack), state(state), state_stack(state_stack), current_symbol(current_symbol) {};

	bool operator()(Shift& shift)
	{
		stack.emplace(Token(*current));
		current++;
		current_symbol = current->Id();
		state_stack.emplace(state);
		state = &shift.state;

		return false;
	}
	bool operator()(Reduce & red)
	{

		state_stack.emplace(state);
		auto cnt = red.rule.Reduce(ctx, stack);
		for (int i = 0; i < cnt; i++) //because this one also counts as state.
			state_stack.pop();

		current_symbol = static_cast<Symbol_t>(red.rule.Id);
		state = state_stack.top();
		return false;
	}
	bool operator()(Goto &_goto)
	{
		//state_stack.emplace(state);
		state = &_goto.state;
		current_symbol = current->Id();
		return false;
	}
	bool operator()(Accept & acc) {return true;}
};

template<typename State, typename Context, typename Stack, typename Iterator>
bool execute_sm(State* state, Context& ctx, Stack& stack, Iterator &itr, const Iterator &end)
{
	bool eof = false;
	bool found = false;
	using namespace std;

	using Token = typename Iterator::value_type;
	using Symbol_t = typename Token::Symbol;

	std::stack<State*> state_stack;
	state_stack.emplace(state);
	Symbol_t current_symbol = itr->Id();
	do
	{
		found = false;

		for (auto &act : state->Actions)
		{
			found = false;
			if (act.Id == current_symbol)
			{
				found = true;
				eof = act.Execute(itr, ctx, stack, state, state_stack, current_symbol);
				break;
			}
		}

	}
	while(found && !eof && (itr != end));
	return eof;
}

template<typename Context_, typename Types, typename Iterator_>
struct StateMachine
{
	using Iterator = Iterator_;
	using Context = Context_;
	using Stack = std::stack<Types, std::vector<Types>>;
	using Token = typename Iterator::value_type;

	State<Context, Stack, Iterator> & start;
	bool parse(Context &context, Iterator &itr, const Iterator &end)
	{
		Stack  stack;
		return execute_sm(&start, context, stack, itr, end);
	}
	StateMachine(State<Context, Stack, Iterator> & start) : start(start) {}
};

struct VariantFather
{
	virtual ~VariantFather() = default;
	virtual std::unique_ptr<VariantFather> clone() const = 0;
	virtual std::unique_ptr<VariantFather> move () = 0;
};

template<typename T>
struct Variant : VariantFather
{
	T member;

	Variant(T&& obj) : member(std::move(obj)) {}
	Variant(const T& obj) : member(obj) {};
	Variant(const Variant&) = default;
	Variant(Variant&&) noexcept = default;
	virtual ~Variant() = default;

	virtual std::unique_ptr<VariantFather> clone() const { return std::make_unique<Variant>(member);}
	virtual std::unique_ptr<VariantFather> move () 		 { return std::make_unique<Variant>(std::move(member)); }

	T& operator*() 	{return  member;}
	T* operator->() {return &member;}
};




struct MyVariant
{
	std::unique_ptr<VariantFather> member;


	template<int Id, typename ...Args>
	void emplace(Args&&... args)
	{
		using type = Variant<typename SymbolType<Id>::type>;
		member = std::make_unique<type>(std::forward<Args>(args)...);
	}
	template<int Id>
	auto get() -> typename SymbolType<Id>::type&
	{
		auto p = dynamic_cast<typename SymbolType<Id>::type*>(member.get());
		if (p == nullptr)
		{
			std::cout << "Requested " << util::demangle(typeid(typename SymbolType<Id>::type).name()) << std::endl;
		}
		assert(p != nullptr);
		return *p;
	}
	template<typename T>
	T& get()
	{
		auto p = dynamic_cast<Variant<T>*>(member.get());

		if (p == nullptr)
		{
			std::cout << "Requested " << util::demangle(typeid(T).name()) << std::endl;
		}
		assert(p != nullptr);

		return **p;
	}
	template<typename Id, typename ...Args>
	MyVariant(Args&&... args) {emplace<Id>(std::forward<Args>(args)...);};
	MyVariant() {member = std::make_unique<Variant<int>>(42);}
	MyVariant(MyVariant&& rhs) noexcept = default;
	MyVariant(const MyVariant& rhs) : member(rhs.member->clone())
	{

	}
	~MyVariant() = default;

	template<typename T> 	MyVariant(T && t)
	{
		member = std::make_unique<Variant<typename std::remove_reference<T>::type>>(std::forward<T>(t));
	}

	template<typename T>
	MyVariant& operator=(T && t)
	{
		member = std::make_unique<Variant<typename std::remove_reference<T>::type>>(std::forward<T>(t));
		return *this;
	}

};


template<typename T, typename Token>
T& getter<T, Token>::get(MyVariant& var)
{
		return var.get<T>();
};


template<typename Token>
Token& getter<token_tag, Token>::get(MyVariant& var)
{
	return var.get<Token>();
}

/*
template<int Id, typename Token>
auto get(MyVariant &var) -> typename getter<typename SymbolType<Id>::type, Token>::type&
{
	return var.get<Id, Token>();
}

template<typename T, typename Token>
auto get(MyVariant &var) -> typename getter<T, Token>::type&
{
	return getter<T, Token>::get(var);
			//var.get<T, Token>();
}
*/

#define LALR_MAKE_PARSER(RuleStart, RuleEnd) \
template<typename Context_, typename Iterator>\
struct Parser : StateMachine<Context_, MyVariant, Iterator> \
{\
	using Sm_t = StateMachine<Context_, MyVariant, Iterator>; \
	using Token = typename Sm_t::Token;\
	using Symbol_t = typename Token::Symbol; \
	using Context = typename Sm_t::Context; \
	using Stack = typename Sm_t::Stack;	\
	using Sa = typename State<Context, Stack, Iterator>::StateAction;



#define LALR_MAKE_SYMBOL_TOKEN(Id) \
template<>	\
struct SymbolType<static_cast<int>(Id)>\
{							\
	using Type = token_tag;		\
};

#define LALR_MAKE_SYMBOL_TYPE(Id, Type_) \
template<>	\
struct SymbolType<static_cast<int>(Id)> : Type_\
{							\
	using Type = Type_;		\
};


#define LALR_DEFINE_RULE_ACTION(Id, Type_)                \
template<>                                                      \
struct RuleAction<static_cast<int>(Id)> : Type_ 				\
{                                                               \
};

#define LALR_MAKE_TOKEN() TokenWrapper<Token>

#define LALR_MAKE_RULE(Id, Name, SymId, SubSymbols...) \
RuleImpl<static_cast<int>(Id), SymId, Token, Context, Stack, SubSymbols> Name;

#define LALR_MAKE_EMPTY_RULE(Id, Name, SymId) \
RuleImpl<static_cast<int>(Id), SymId, Token, Context, Stack> Name;

#define LALR_MAKE_STATE(Id, Actions...) \
State<Context, Stack, Iterator> State##Id {Id, { Actions }};

#define LALR_MAKE_SHIFT_ACTION(Id, State_Id) Sa(static_cast<Symbol_t>(Id), typename State<Context, Stack, Iterator>::Shift(State##State_Id))
#define LALR_MAKE_REDUCE_ACTION(Id, Rule)	 Sa(static_cast<Symbol_t>(Id), typename State<Context, Stack, Iterator>::Reduce(Rule))
#define LALR_MAKE_GOTO_ACTION(Id, State_Id)  Sa(static_cast<Symbol_t>(Id), typename State<Context, Stack, Iterator>::Goto(State##State_Id))
#define LALR_MAKE_ACCEPT_ACTION(Id) 		 Sa(static_cast<Symbol_t>(Id), typename State<Context, Stack, Iterator>::Accept())



} /* namespace Egt */
}
#endif /* TAR_LALR_H_ */
