/*
 * types.h
 *
 *  Created on: 23.04.2015
 *      Author: Klemens
 */

#ifndef EGT_TYPES_H_
#define EGT_TYPES_H_

#include <cstdint>
#include <vector>
#include <string>
#include <boost/blank.hpp>
#include <istream>

namespace Egt
{
using Integer 	= std::uint16_t;
template<typename T>
using Range		= std::vector<T>;
using String  	= std::wstring;
using Byte		= char;
using Boolean	= bool;
using Blank 	= boost::blank;




inline String ReadString(std::istream &ss)
{
	String name;

	while (!ss.eof())
	{
		wchar_t value = 0xFFFF;
		ss.read(reinterpret_cast<char*>(&value), 2);
		if (value == '\0')
			break;

		name.push_back(value);
	}

	return name;
}

}



#endif /* EGT_TYPES_H_ */
