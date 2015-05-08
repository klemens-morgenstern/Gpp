/*
 * entry.h
 *
 *  Created on: 23.04.2015
 *      Author: Klemens
 */

#ifndef ENTRY_H_
#define ENTRY_H_

#include "../Types.h"
#include "GetVis.h"
#include <boost/variant.hpp>
#include <boost/optional.hpp>
#include <string>
#include <ostream>
#include <istream>
#include <cstdint>

namespace Egt {

class Entry
{
public:
	typedef boost::variant<boost::none_t, Byte, Boolean, Integer, String> data_t;
	data_t data = boost::none;
	Entry() = default;
	Entry(Entry && )= default;
	Entry(const Entry & )= default;
	Entry(data_t dat) : data(dat) {};

	template<typename T>
	T get() const
	{
		GetVis<T> vis;
		return data.apply_visitor(vis);
	}

};

Entry ReadEntry(std::istream& str);


std::wostream &operator<<(std::wostream& str, const Entry &e);

} /* namespace Gpp */

#endif /* ENTRY_H_ */
