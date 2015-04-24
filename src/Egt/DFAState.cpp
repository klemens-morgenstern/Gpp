/*
 * DFSState.cpp
 *
 *  Created on: 24.04.2015
 *      Author: Klemens
 */

#include "DFAState.h"
#include "Reader/Record.h"
#include "Reader/GetVis.h"

namespace Egt
{
Indexed<DFAState> DFAState::FromRecord(const Record &r)
{
	DFAState p;

	auto idx 		= r.Entries.at(1).get<Integer>();
	p.AcceptState	= r.Entries.at(2).get<Boolean>();
	p.AcceptIndex	= r.Entries.at(3).get<Integer>();

	for (auto i = 5u; i<r.Entries.size(); i+=3)
	{
		Edge edg;
		edg.CharSetIndex = r.Entries.at(i).  get<Integer>();
		edg.TargetIndex  = r.Entries.at(i+1).get<Integer>();
		p.Edges.push_back(edg);

	}

	return {idx, p};
}



}



