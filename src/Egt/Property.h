/*
 * property.h
 *
 *  Created on: 23.04.2015
 *      Author: Klemens
 */

#ifndef EGT_PROPERTY_H_
#define EGT_PROPERTY_H_

#include "Types.h"
#include <ostream>

namespace Egt {

struct Record;

/** Property record
 * | Byte 	  | Integer | String | String |
 * |----------|---------|--------|--------|
 * | 'P'/112  | Index   | Name   | Value  |
 */

struct Property
{
	static constexpr Byte Id = 'p';


	enum Index_t : Integer
	{
		NameIdx				= 0,
		Version				= 1,
		Author				= 2,
		About				= 3,
		Character_Set		= 4,
		Character_Mapping	= 5,
		Generated_Date		= 6,
		Generated_By		= 7,
	};

	Index_t Index;
	String Name;
	String Value;

	static Property FromRecord(const Record &r);

};

std::wostream& operator<<(std::wostream& s, const Property& f);

} /* namespace Gpp */

#endif /* EGT_PROPERTY_H_ */
