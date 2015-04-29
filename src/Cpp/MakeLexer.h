/*
 * MakeLexer.h
 *
 *  Created on: 24.04.2015
 *      Author: Klemens
 */

#ifndef CPP_MAKELEXER_H_
#define CPP_MAKELEXER_H_

#include <string>

namespace Egt {class File;}

namespace Gpp
{

void makeLexer(const Egt::File &f, const std::string& name = "Lexer", const std::string& ns = "GoldParser", const std::string & path = "");

} /* namespace Egt */

#endif /* CPP_MAKELEXER_H_ */
