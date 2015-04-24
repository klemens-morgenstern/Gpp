/*
 * Group.cpp
 *
 *  Created on: 24.04.2015
 *      Author: Klemens
 */

#include "Reader/Record.h"
#include "Group.h"

#include <iostream>

using namespace std;

namespace Egt
{
/*	Integer 		Index
	String 			Name
	Integer 		ContainerIndex
	Integer 		StartIndex
	Integer 		EndIndex
	AdvanceMode_t 	AdvanceMode;
	EndingMode_t 	EndingMode
	Empty
	Integer 		NestingCount 	//count of the group

	Range<Integer> GroupIndex;*/

Indexed<GroupRecord> GroupRecord::FromRecord(const Record &r)
{
	GroupRecord gc;

	auto idx            = r.Entries.at(1).get<Integer 		>();
	gc.Name             = r.Entries.at(2).get<String 		>();
	gc.ContainerIndex   = r.Entries.at(3).get<Integer 		>();
	gc.StartIndex       = r.Entries.at(4).get<Integer 		>();
	gc.EndIndex         = r.Entries.at(5).get<Integer 		>();
	gc.AdvanceMode      = static_cast<GroupRecord::AdvanceMode_t>(r.Entries.at(6).get<Integer>());
	gc.EndingMode       = static_cast<GroupRecord::EndingMode_t> (r.Entries.at(7).get<Integer>());

	auto cnt     = r.Entries.at(9).get<Integer>();

	for (auto i = 0; i<cnt; i++)
		gc.GroupIndex.push_back(r.Entries.at(10+i).get<Integer>());

	return {idx, gc};
}

}


