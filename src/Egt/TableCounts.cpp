/*
 * TableCounts.cpp
 *
 *  Created on: 24.04.2015
 *      Author: Klemens
 */

#include "TableCounts.h"
#include "Reader/Record.h"

using namespace std;

namespace Egt
{

TableCounts TableCounts::FromRecord(const Record &r)
{
	TableCounts tc;

	tc.SymbolTable = r.Entries.at(1).get<Integer>();
	tc.SetTable    = r.Entries.at(2).get<Integer>();
	tc.RuleTable   = r.Entries.at(3).get<Integer>();
	tc.DFATable    = r.Entries.at(4).get<Integer>();
	tc.LALRTable   = r.Entries.at(5).get<Integer>();
	tc.GroupTable  = r.Entries.at(6).get<Integer>();

	return tc;
}

wostream &operator<<(wostream& str, const TableCounts &e)
{
	str << "==================== Table Counts ====================" << endl;

	str << "\tSymbolTable: " << e.SymbolTable << endl;
	str << "\tSetTable   : " << e.SetTable    << endl;
	str << "\tRuleTable  : " << e.RuleTable   << endl;
	str << "\tDFATable   : " << e.DFATable    << endl;
	str << "\tLALRTable  : " << e.LALRTable   << endl;
	str << "\tGroupTable : " << e.GroupTable  << endl;

	return str;
}

}


