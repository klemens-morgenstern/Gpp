/*
 * InitialStates.h
 *
 *  Created on: 23.04.2015
 *      Author: Klemens
 */

#ifndef EGT_INITIALSTATES_H_
#define EGT_INITIALSTATES_H_


#include "Types.h"
#include <ostream>

namespace Egt
{


struct InitialStates
{
	static constexpr Byte Id = 'I';

	Integer DFA;
	Integer LALR;

	static InitialStates FromRecord(const Record &r);

};

std::wostream& operator<<(std::wostream& s, const InitialStates& f);

}

#endif /* EGT_INITIALSTATES_H_ */
