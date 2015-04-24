/*
 * DFAState.h
 *
 *  Created on: 23.04.2015
 *      Author: Klemens
 */

#ifndef EGT_DFASTATE_H_
#define EGT_DFASTATE_H_

#include "Types.h"
#include <ostream>

namespace Egt
{

struct Record;

struct DFAState
{
	static constexpr Byte Id = 'D';

	struct Edge
	{
		Integer CharSetIndex;
		Integer TargetIndex;
	};

	Integer Index;
	Boolean AcceptState;
	Integer AcceptIndex;

	Range<Edge> Edges;

	static DFAState FromRecord(const Record &r);


};

std::wostream& operator<<(std::wostream& s, const DFAState& p);

}


#endif /* EGT_DFASTATE_H_ */
