/*
 * Symbol.h
 *
 *  Created on: 23.04.2015
 *      Author: Klemens
 */

#ifndef EGT_SYMBOL_H_
#define EGT_SYMBOL_H_

#include "Types.h"
#include <iostream>

namespace Egt
{

struct Record;


struct Symbol
{
	static constexpr Byte Id = 'S';

	enum Type_t : Integer
	{
		Nonterminal = 0,
		Terminal 	= 1,
		Noise 		= 2,
		EndOfFile 	= 3,
		GroupStart 	= 4,
		GroupEnd 	= 5,
		Decrement 	= 6, //deprecated
		Error		= 7,
	};

	String Name;
	Type_t Type;

	static Indexed<Symbol> FromRecord(const Record &r);

};

}



#endif /* EGT_SYMBOL_H_ */
