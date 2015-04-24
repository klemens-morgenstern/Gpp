/*
 * LALRState.h
 *
 *  Created on: 23.04.2015
 *      Author: Klemens
 */

#ifndef EGT_LALRSTATE_H_
#define EGT_LALRSTATE_H_

#include "Types.h"

namespace Egt
{


struct LALRState
{
	static constexpr char Byte = 'L';

	enum ActionType_t : Integer
	{
		Shift	= 1, ///<This action indicates the symbol is to be shifted. The Target field will contain the index of the state in the LALR State table that the parsing engine will advance to.
		Reduce	= 2, ///<This action denotes that the parser can reduce a rule. The Target field will contain the index of the rule in the Rule Table.
		Goto	= 3, ///<This action is used when a rule is reduced and the parser jumps to the state that represents the shifted nonterminal. The Target field will contain the index of the state in the LALR State table that the parsing engine will jump to after a reduction if completed.
		Accept	= 4, ///<When the parser encounters the Accept action for a given symbol, the source text is accepted as correct and complete. In this case, the Target field is not needed and should be ignored.
	};

	struct Action_t
	{
		Integer SymbolIndex;
		ActionType_t Action;
		Integer TargetIndex;
	};

	Integer Index;

	Range<Action_t> Actions;
};

}
#endif /* EGT_LALRSTATE_H_ */
