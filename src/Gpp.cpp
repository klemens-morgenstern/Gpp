//============================================================================
// Name        : Gpp.cpp
// Author      : Lemmy
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <string>
#include <array>
#include <locale>
#include <sstream>
#include <boost/locale/encoding_utf.hpp>
#include <boost/locale.hpp>
#include <boost/iterator/function_input_iterator.hpp>
#include <boost/variant.hpp>

#include "Egt/Reader/RawFile.h"
#include "Egt/File.h"

#include "Cpp/MakeLexer.h"
#include "Cpp/MakeRules.h"

#include "Tar/DFA.h"

using namespace std;
using namespace Egt;


int main()
{

	File f = readRawFile("cte.egt");

	Gpp::makeLexer(f, "Lexer", "CteParser", "out/");
	Gpp::makeRules (f, "Rules", "CteParser", "out/");
//	Gpp::Dfa::test();

	return 0;
}
