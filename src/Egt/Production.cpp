/*
 * Production.cpp
 *
 *  Created on: 24.04.2015
 *      Author: Klemens
 */


#include "Reader/Record.h"
#include "Production.h"

using namespace std;

namespace Egt
{
Production Production::FromRecord(const Record &r)
{
	Production cs;

	cs.Index 		= r.Entries.at(1).get<Integer>();
	cs.HeadIndex 	= r.Entries.at(2).get<Integer>();

	for (auto itr = r.Entries.begin() + 4; itr != r.Entries.end(); itr++)
		cs.Symbols.push_back(itr->get<Integer>());

	return cs;
}

std::wostream& operator<<(std::wostream& s, const Production& f)
{
	s << "\tIndex: " << f.Index << "\tHeadIndex: " <<  f.HeadIndex << "\tSymbols: {";
	for (auto & sy : f.Symbols)
		s << sy << ", ";

	s << "}";

	return s;

}
}




