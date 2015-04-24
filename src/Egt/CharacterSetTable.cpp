/*
 * CharacterSetTable.cpp
 *
 *  Created on: 24.04.2015
 *      Author: Klemens
 */

#include "Reader/Record.h"
#include "CharacterSetTable.h"

using namespace std;

namespace Egt
{

Indexed<CharacterSetTable> CharacterSetTable::FromRecord(const Record &r)
{
	CharacterSetTable cs;

	auto Idx		= r.Entries.at(1).get<Integer>();
	cs.UnicodePlane = r.Entries.at(2).get<Integer>();

	auto cnt = r.Entries.at(3).get<Integer>();

	for (auto i = 0; i<cnt; i++)
		cs.Characters.push_back(r.Entries.at(5+i).get<Integer>());
	return {Idx, cs};
}

}
