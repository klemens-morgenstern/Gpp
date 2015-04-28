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
Indexed<Symbol> Symbol::FromRecord(const Record &r)
{
	Symbol cs;

	auto Idx 	= r.Entries.at(1).get<Integer>();
	cs.Name 	= r.Entries.at(2).get<String>();
	cs.Type 	= static_cast<Symbol::Type_t>(r.Entries.at(3).get<Integer>());

	return {Idx, cs};
}


}


