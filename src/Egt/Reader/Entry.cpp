/*
 * entry.cpp
 *
 *  Created on: 23.04.2015
 *      Author: Klemens
 */

#include "entry.h"
#include <sstream>
using namespace std;

namespace Egt
{

struct visi : boost::static_visitor<wstring>
{
	wstring operator()(boost::blank) {return L"blank";}
	wstring operator()(char val)
	{
		wostringstream ws;
		ws << L"byte: " << val;
		return ws.str();
	}
	wstring operator()(bool val)
	{
		wostringstream ws;
		ws << L"bool: " << val;
		return ws.str();

	}
	wstring operator()(uint16_t val)
	{
		wostringstream ws;
		ws << L"int: " << val;
		return ws.str();

	}
	wstring operator()(wstring  val) {return L"string: " + val + L"\n"; }
};


wostream &operator<<(wostream& str, const Entry &e)
{
	visi v;
	str << L"\t" <<  e.data.apply_visitor(v) << endl;


	return str;
}




Entry ReadEntry(std::istream& str)
{
	char c = str.peek();


	switch(c)
	{
	case 'E': //empty
		str.read(&c, 1);
		return {};
	case 'b': //bool
	{
		str.read(&c, 1);
		str.read(&c, 1);
		return Entry(c);
	}
	case 'B': //bool
	{
		str.read(&c, 1);
		str.read(&c, 1);
		return Entry(c != 0);
	}
	case 'I': //Integer
	{
		str.read(&c, 1);
		std::uint16_t val;
		str.read(reinterpret_cast<char*>(&val), 2);
		return Entry(val);
	}
	case 'S':
	{
		str.read(&c, 1);
		return Entry(ReadString(str));
	}
	default:
		throw std::runtime_error("Load error in entry - ");
		return Entry();
	}
}



} /* namespace Gpp */
