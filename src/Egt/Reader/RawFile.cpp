/*
 * file.cpp
 *
 *  Created on: 23.04.2015
 *      Author: Klemens
 */

#include "RawFile.h"

#include <iostream>
#include <fstream>
#include <exception>
#include <sstream>

using namespace std;

namespace Egt
{

std::istream &operator>>(std::istream& str,  RawFile &e)
{
	if (!str.eof())
		e.Name = ReadString(str);


	while (!str.eof())
	{
		auto rec = read_record(str);
		if (rec)
			e.Records.push_back(*rec);
		else
			return str;
	}
	return str;

}

RawFile LoadFile(const std::string &filename)
{
	ifstream str(filename, std::ios::binary);
	stringstream ss;
	ss << str.rdbuf();

	RawFile rf;
	ss >> rf;

	if (!ss.eof())
		throw std::ios_base::failure("File " + filename + " could not be loaded completyl");

	return rf;
}


}


