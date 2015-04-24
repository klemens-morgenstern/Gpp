/*
 * Group.h
 *
 *  Created on: 23.04.2015
 *      Author: Klemens
 */

#ifndef EGT_GROUP_H_
#define EGT_GROUP_H_

#include "Types.h"

namespace Egt
{


struct GroupRecord
{
	static constexpr char Id = 'g';

	enum AdvanceMode_t : Integer
	{
		Token 	  = 0, ///<	The group will advance a token at a time.
		Character = 1, ///<	The group will advance by just one character at a time.
	};


	enum EndingMode_t : Integer
	{
		Open 	= 0, ///<	The ending symbol will be left on the input queue.
		Closed 	= 1, ///<	The ending symbol will be consumed.
	};

	String Name;
	Integer ContainerIndex;
	Integer StartIndex;
	Integer EndIndex;
	AdvanceMode_t AdvanceMode;
	EndingMode_t EndingMode;

	Range<Integer> GroupIndex;

	static Indexed<GroupRecord> FromRecord(const Record &r);
};

}



#endif /* EGT_GROUP_H_ */
