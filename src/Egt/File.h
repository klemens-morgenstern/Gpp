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
	Map<CharacterSetTable> CharacterSetTables;
	Map<Symbol> SymbolTable;
	Map<GroupRecord> GroupRecords;
	Map<Production> Productions;

	InitialStates InitialStates;

	Map<DFAState> DFAStates;
	Map<LALRState> LALRStates;

	void AddProperty(const Property &prop);
	void AddRecord(const Record &r);

};

File readRawFile(const std::string & rf);

File readRawFile(const RawFile & rf);


} /* namespace Egt */

#endif /* EGT_FILE_H_ */
