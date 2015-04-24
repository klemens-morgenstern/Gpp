/*
 * Symbol.cpp
 *
 *  Created on: 24.04.2015
 *      Author: Klemens
 */

#include "Reader/Record.h"
#include "Symbol.h"

using namespace std;

namespace Egt
{
Symbol Symbol::FromRecord(const Record &r)
{
	Symbol cs;

	cs.Index 	= r.Entries.at(1).get<Integer>();
	cs.Name 	= r.Entries.at(2).get<String>();
	cs.Type 	= static_cast<Symbol::Type_t>(r.Entries.at(3).get<Integer>());

	return cs;
}

std::wostream& operator<<(std::wostream& s, const Symbol& f)
{
	s << "\tIndex: " << f.Index << "\tName: " <<  f.Name << "\tType: " << static_cast<Integer>(f.Type);

	return s;

}
}


