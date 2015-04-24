#include "Property.h"
#include "Reader/Record.h"
#include "Reader/GetVis.h"

namespace Egt
{
Property Property::FromRecord(const Record &r)
{
	Property p;

	p.Index = static_cast<Index_t>(r.Entries.at(1).get<Integer>());
	p.Name	= r.Entries.at(2).get<String>();
	p.Value	= r.Entries.at(3).get<String>();

	return p;
}

std::wostream& operator<<(std::wostream& s, const Property& p)
{
	s << "Idx: " << p.Index << " " << p.Name << ":" << p.Value;
	return s;
}


}
