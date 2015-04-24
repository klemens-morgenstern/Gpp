/*
 * record.h
 *
 *  Created on: 23.04.2015
 *      Author: Klemens
 */

#ifndef RECORD_H_
#define RECORD_H_

#include <vector>
#include "entry.h"
#include "GetVis.h"

namespace Egt {

struct Record
{
	std::vector<Entry> Entries{};


};

boost::optional<Record> read_record(std::istream& str);
std::wostream &operator<<(std::wostream& str, const Record &e);


} /* namespace Gpp */

#endif /* RECORD_H_ */
