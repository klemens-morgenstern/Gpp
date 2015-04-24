/*
 * LALRState.cpp
 *
 *  Created on: 24.04.2015
 *      Author: Klemens
 */


#include "LALRState.h"
#include "Reader/Record.h"
#include "Reader/GetVis.h"

namespace Egt
{
Indexed<LALRState> LALRState::FromRecord(const Record &r)
{
	LALRState p;

	auto idx = r.Entries.at(1).get<Integer>();

	for (auto i = 3u; i<r.Entries.size(); i+=4)
	{
		Action_t a;
		a.SymbolIndex = r.Entries.at(i).  get<Integer>();
		a.Action 	  = static_cast<ActionType_t>(r.Entries.at(i+1).get<Integer>());
		a.TargetIndex = r.Entries.at(i+2).get<Integer>();

		p.Actions.push_back(a);

	}

	return {idx, p};
}


}



