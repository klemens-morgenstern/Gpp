/*
 * File.cpp
 *
 *  Created on: 23.04.2015
 *      Author: Klemens
 */

#include "File.h"
#include <type_traits>

#include "Reader/GetVis.h"
#include <cassert>

using namespace std;
namespace Egt
{

void File::AddProperty(const Property &prop)
{
	switch (prop.Index)
	{
	case Property::NameIdx:				Name 			 = prop.Value; break;
	case Property::Version:				Version			 = prop.Value; break;
	case Property::Author:				Author			 = prop.Value; break;
	case Property::About:				About			 = prop.Value; break;
	case Property::Character_Set:		CharacterSet 	 = prop.Value; break;
	case Property::Character_Mapping:	CharacterMapping = prop.Value; break;
	case Property::Generated_Date:		GeneratedDate 	 = prop.Value; break;
	case Property::Generated_By:		GeneratedBy 	 = prop.Value; break;
	}
}


void File::AddRecord(const Record &r )
{
	switch(r.Entries.at(0).get<Byte>())
	{
	case Property::Id:
		AddProperty(Property::FromRecord(r));
		break;
	case TableCounts::Id:
		this->TableCounts = TableCounts::FromRecord(r);
		break;
	case CharacterSetTable::Id:
		this->CharacterSetTables.emplace(CharacterSetTable::FromRecord(r));
		break;
	case Symbol::Id:
		this->SymbolTable.emplace(Symbol::FromRecord(r));
		break;
	case GroupRecord::Id:
		GroupRecords.emplace(GroupRecord::FromRecord(r));
		break;
	case Production::Id:
		Productions.emplace(Production::FromRecord(r));
		break;
	case InitialStates::Id:
		InitialStates = InitialStates::FromRecord(r);
		break;
	case DFAState::Id:
		DFAStates.emplace(DFAState::FromRecord(r));
		break;
	case LALRState::Id:
		LALRStates.emplace(LALRState::FromRecord(r));
		break;
	}
}


File readRawFile(const RawFile & rf)
{
	using namespace std;

	File f;

	f.GoldVersion = rf.Name;

	for (auto &rec : rf.Records)
		f.AddRecord(rec);

	assert(f.SymbolTable.size()			== f.TableCounts.SymbolTable );
	assert(f.CharacterSetTables.size()	== f.TableCounts.SetTable    );
	assert(f.Productions.size()			== f.TableCounts.RuleTable   );
	//assert(f.DFAStates.size()			== f.TableCounts.DFATable    ); //??
	//assert(f.LALRStates.size()			== f.TableCounts.LALRTable   ); //??
	assert(f.GroupRecords.size()		== f.TableCounts.GroupTable  );

	return f;
}




} /* namespace Egt */
