/*
 * MakeRules.h
 *
 *  Created on: 30.04.2015
 *      Author: Klemens
 */

#ifndef CPP_MAKERULES_H_
#define CPP_MAKERULES_H_

#include <string>

namespace Egt {class File;}

namespace Gpp
{

void makeRules(const Egt::File &f, const std::string& name = "Lexer", const std::string& ns = "GoldParser", const std::string & path = "");

} /* namespace Egt */

#endif /* CPP_MAKERULES_H_ */
