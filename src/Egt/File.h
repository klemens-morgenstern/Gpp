/*
 * File.h
 *
 *  Created on: 23.04.2015
 *      Author: Klemens
 */

#ifndef EGT_FILE_H_
#define EGT_FILE_H_

#include "Reader/RawFile.h"
#include "Types.h"
#include "Property.h"
#include "TableCounts.h"
#include "CharacterSetTable.h"
#include "Symbol.h"
#include "Group.h"
#include "Production.h"
#include "InitialStates.h"
#include "DFAState.h"
#include "LALRState.h"
#include <ostream>

namespace Egt {

struct File
{
	String GoldVersion 		= L"";
	String Name				= L"";
	String Version			= L"";
	String Author			= L"";
	String About			= L"";
	String CharacterSet		= L"";
	String CharacterMapping	= L"";
	String GeneratedBy		= L"";
	String GeneratedDate 	= L"";

	TableCounts TableCounts;
	Range<CharacterSetTable> CharacterSetTables;
	Range<Symbol> SymbolTable;
	Range<GroupRecord> GroupRecords;
	Range<Production> Productions;

	InitialStates InitialStates;

	Range<DFAState> DFAStates;
	Range<LALRState> LALRStates;

	void AddProperty(const Property &prop);
	void AddRecord(const Record &r);

};

File readRawFile(const RawFile & rf);

std::wostream& operator<<(std::wostream& s, const File& f);

} /* namespace Egt */

#endif /* EGT_FILE_H_ */
