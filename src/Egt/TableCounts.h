/*
 * format.h
 *
 *  Created on: 23.04.2015
 *      Author: Klemens
 */

#ifndef EGT_TABLECOUNTS_H_
#define EGT_TABLECOUNTS_H_

#include "Types.h"
#include <ostream>

namespace Egt
{
struct Record;

/** TableCounts
 * Byte: 't'/116
 * | Item				 |  Type   | Description												|
 * |---------------------|---------|------------------------------------------------------------|
 * | Symbol Table	     | Integer | The number of symbols in the language.                     |
 * | Character Set Table | Integer | The number of character sets used by the DFA state table.  |
 * | Rule Table	         | Integer | The number of rules/productions in the language.           |
 * | DFA Table	         | Integer | The number of Deterministic Finite Automata states.        |
 * | LALR Table	         | Integer | The number of LALR States.                                 |
 * | GroupTable	         | Integer | The number of lexical groups.                              |
 */
struct TableCounts
{
	static constexpr Byte Id = 't';

	Integer SymbolTable;
	Integer SetTable;
	Integer RuleTable;
	Integer DFATable;
	Integer LALRTable;
	Integer GroupTable;

	static TableCounts FromRecord(const Record &r);

};

std::wostream &operator<<(std::wostream& str, const TableCounts &e);


}
#endif /* EGT_TABLECOUNTS_H_ */
