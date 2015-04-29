/*
 * CharacterSetTable.cpp
 *
 *  Created on: 24.04.2015
 *      Author: Klemens
 */

#include "Reader/Record.h"
#include "CharacterSetTable.h"

#include <iostream>

using namespace std;

namespace Egt
{

Indexed<CharacterSetTable> CharacterSetTable::FromRecord(const Record &r)
{
	CharacterSetTable cs;

	auto Idx		= r.Entries.at(1).get<Integer>();
	cs.UnicodePlane = r.Entries.at(2).get<Integer>();

	auto cnt = r.Entries.at(3).get<Integer>();

	for (auto i = 0; i< cnt*2; i+=2)
	{
		///the mother returns a range!
		auto beg = r.Entries.at(5+i).get<Integer>();
		auto end = r.Entries.at(6+i).get<Integer>();

		if (beg == end)
			cs.Characters.push_back(beg);
		else
			cs.CharRanges.push_back(CharRange(beg, end));
	}
	return {Idx, cs};
}

}
