/*
 * InitialStates.cpp
 *
 *  Created on: 24.04.2015
 *      Author: Klemens
 */

#include "InitialStates.h"
#include "Reader/Record.h"
#include "Reader/GetVis.h"

using namespace std;

namespace Egt
{
InitialStates InitialStates::FromRecord(const Record &r)
{
	InitialStates p;

	p.DFA	= r.Entries.at(1).get<Integer>();
	p.LALR	= r.Entries.at(2).get<Integer>();

	return p;
}

std::wostream& operator<<(std::wostream& s, const InitialStates& p)
{
	s << "==================== Production ====================" << endl;
	s << "\tDFA : " << p.DFA << endl;
	s << "\tLALR: " << p.LALR << endl;
	return s;
}


}


