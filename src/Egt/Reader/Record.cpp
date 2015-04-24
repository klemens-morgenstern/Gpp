/*
 * record.cpp
 *
 *  Created on: 23.04.2015
 *      Author: Klemens
 */

#include "Record.h"

#include <iostream>
using namespace std;

namespace Egt
{


boost::optional<Record> read_record(std::istream& str)
{
	if (str.peek() == 'M')
	{
		char M;
		str >> M; //read the 'M'
		uint16_t cnt;

		str.read(reinterpret_cast<char*>(&cnt), 2);

		Record r;

		for (uint16_t i = 0; i<cnt; i++)
		{
			r.Entries.push_back(ReadEntry(str));
		}

		return r;
	}
	else return boost::none;
}

std::wostream &operator<<(std::wostream& str, const Record &e)
{
	str << "Record" << endl;
	for (auto &x : e.Entries)
	{
		str << "   " << e << endl;
	}

	str << endl;

	return str;
}


} /* namespace Gpp */
