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

using namespace std;
using namespace Egt;


int main()
{

	File f = readRawFile(LoadFile("cte.egt"));
	//auto rw = LoadFile("cte.egt");

	wcout << f << endl;
	//cout << rw.Records.size();

	return 0;
}
