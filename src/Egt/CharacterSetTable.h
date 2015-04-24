/*
 * structure.h
 *
 *  Created on: 23.04.2015
 *      Author: Klemens
 */

#ifndef EGT_CHARACTERSETTABLE_H_
#define EGT_CHARACTERSETTABLE_H_

#include "types.h"
#include <ostream>

namespace Egt
{

struct Record;
//byte 'c'
struct CharacterSetTable
{
	static constexpr Byte Id = 'c';

	Integer UnicodePlane;
	//Integer RangeCount;

	Range<Integer> Characters;

	static Indexed<CharacterSetTable> FromRecord(const Record &r);

};

std::wostream& operator<<(std::wostream& s, const CharacterSetTable& f);


}



#endif /* EGT_CHARACTERSETTABLE_H_ */
