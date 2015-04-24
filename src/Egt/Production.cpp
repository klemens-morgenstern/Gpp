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
Indexed<Production> Production::FromRecord(const Record &r)
{
	Production cs;

	auto idx		= r.Entries.at(1).get<Integer>();
	cs.HeadIndex 	= r.Entries.at(2).get<Integer>();

	for (auto itr = r.Entries.begin() + 4; itr != r.Entries.end(); itr++)
		cs.Symbols.push_back(itr->get<Integer>());

	return {idx, cs};
}

}




