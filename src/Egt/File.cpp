/*
 * File.cpp
 *
 *  Created on: 23.04.2015
 *      Author: Klemens
 */

#include "File.h"
#include <type_traits>

#include "Reader/GetVis.h"

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
		this->CharacterSetTables.push_back(CharacterSetTable::FromRecord(r));
		break;
	case Symbol::Id:
		this->SymbolTable.push_back(Symbol::FromRecord(r));
		break;
	case GroupRecord::Id:
		GroupRecords.push_back(GroupRecord::FromRecord(r));
		break;
	case Production::Id:
		Productions.push_back(Production::FromRecord(r));
		break;
	case InitialStates::Id:
		InitialStates = InitialStates::FromRecord(r);
		break;
	case DFAState::Id:
		DFAStates.push_back(DFAState::FromRecord(r));
		break;
	}
}


File readRawFile(const RawFile & rf)
{
	File f;

	f.GoldVersion = rf.Name;

	for (auto &rec : rf.Records)
		f.AddRecord(rec);

	return f;
}

std::wostream& operator<<(std::wostream& s, const File& f)
{
	s << f.Name << endl;
	s << "==================== Properties ====================" << endl;
	s << "\tGoldVersion 	 : " << f.GoldVersion 	   	<< endl;
	s << "\tName			 : " << f.Name			   	<< endl;
	s << "\tVersion			 : " << f.Version		   	<< endl;
	s << "\tAuthor			 : " << f.Author			<< endl;
	s << "\tAbout			 : " << f.About			   	<< endl;
	s << "\tCharacterSet	 : " << f.CharacterSet	   	<< endl;
	s << "\tCharacterMapping : " << f.CharacterMapping  << endl;
	s << "\tGeneratedBy		 : " << f.GeneratedBy	   	<< endl;
	s << "\tGeneratedDate 	 : " << f.GeneratedDate 	<< endl;

	s << endl << f.TableCounts << endl;
	for (auto &cst :  f.CharacterSetTables)
		s << endl << cst << endl;

	s << "==================== Symbol Table ====================" << endl;
	for (auto &sy : f.SymbolTable)
		s << sy << endl;

	for (auto gc : f.GroupRecords)
		s << gc << endl;

	s << "==================== Production ====================" << endl;
	for (auto &sy : f.Productions)
		s << sy << endl;

	for (auto &d : f.DFAStates)
		s << d << endl;

	return s;
}


} /* namespace Egt */
