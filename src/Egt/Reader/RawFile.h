/*
 * file.h
 *
 *  Created on: 23.04.2015
 *      Author: Klemens
 */

#ifndef FILE_H_
#define FILE_H_

#include "Record.h"
#include "../Types.h"
#include <istream>
#include <vector>

namespace Egt {

struct RawFile
{
	String Name;
	Range<Record> Records;

};


std::istream &operator>>(std::istream& str,  RawFile &e);

RawFile LoadFile(const std::string &filename);



}
#endif /* FILE_H_ */
