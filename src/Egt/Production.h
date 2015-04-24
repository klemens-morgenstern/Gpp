/*
 * Format.h
 *
 *  Created on: 23.04.2015
 *      Author: Klemens
 */

#ifndef EGT_PRODUCTION_H_
#define EGT_PRODUCTION_H_

#include "Types.h"

namespace Egt
{


struct Production
{
	static constexpr Byte Id = 'R';

	Integer HeadIndex;

	Range<Integer> Symbols;

	static Indexed<Production> FromRecord(const Record &r);

};

}



#endif /* EGT_PRODUCTION_H_ */
