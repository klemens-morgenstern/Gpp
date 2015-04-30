/*
 * MakeRules.cpp
 *
 *  Created on: 30.04.2015
 *      Author: Klemens
 */

#include "MakeRules.h"
#include "../Egt/File.h"
#include <fstream>
#include <sstream>
#include <string>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string_regex.hpp>
#include <boost/optional.hpp>

#include <iostream>
using namespace std;

namespace Gpp
{

struct RuleMake
{
	const Egt::File &f;
	RuleMake(const Egt::File &f) : f(f) {};
	static string make_token_name(const wstring &in);
	void makeTree(const std::string& name, const std::string& ns, const std::string & path);
	void makeParser(const std::string& name, const std::string& ns, const std::string & path);

};

string RuleMake::make_token_name(const wstring &in)
{
	string name;

	name.reserve(in.size());
	for (auto & wc : in)
	{
		if (((wc >= 'A') && (wc <= 'Z'))
		 || ((wc >= 'a') && (wc <= 'z'))
		 || ((wc >= '0') && (wc <= '9')))
			name.push_back(static_cast<char>(wc)); //dirty, but hell
		else
			name += "u" + to_string(wc);
	}
	return name;
}

void RuleMake::makeTree(const std::string& name, const std::string& ns, const std::string & path)
{
	stringstream ss;

	ss << "#ifndef PARSER_" << name << "_TREE" << endl;
	ss << "#define PARSER_" << name << "_TREE" << endl;

	ss << "\n#include \"Token.hpp\"\n" << endl;
	ss << "namespace Stubs\n{" << endl;
	map<string, vector<Egt::Production>> mp;
	for (auto & r : f.Productions)
	{
		auto nm = RuleMake::make_token_name(f.SymbolTable.at(r.second.HeadIndex).Name);

		if (mp[nm].size() == 0)
			ss << "struct " << nm << ";" << endl;
		mp[nm].push_back(r.second);
	}

	ss << "}\n" <<endl;


	ss << "namespace Tree \n{" << endl;
	ss << "using namespace Stubs;" << endl;
	ss << "} //Tree" << endl;

	ss << "namespace Stubs \n{" << endl;

	for (auto & r : mp)
	{


		auto nm = RuleMake::make_token_name(f.SymbolTable.at(r.second.front().HeadIndex).Name);
		ss << "struct " << nm << "\n{" << endl;

		ss << "\t" << nm << "() = default;" << endl;
		ss << "\t" << nm << "(const " << nm <<  "&)  = default;" << endl;
		ss << "\t" << nm << "(" <<		nm << "&&)  = default;\n" << endl;

		for (auto & v : r.second)
		{
			if (v.Symbols.size() == 0) continue; //no argument, i.e. default constructor.
			ss << "\t" << nm << "(const ";
			for (auto itr = v.Symbols.begin(); itr != v.Symbols.end(); itr++)
			{
				if (itr != v.Symbols.begin())
					ss << ", ";

				auto sy = f.SymbolTable.at(*itr);

				auto s = RuleMake::make_token_name(sy.Name);
				if (sy.Type == Egt::Symbol::Nonterminal)
					ss << "Tree::" << s << "& /* " << s << " */" ;
				if (sy.Type == Egt::Symbol::Terminal)
					ss << "Token " << "& /* " << s << " */";

			}
			ss << ") {}" << endl;
		}
		ss << "\n};" << endl;
	}


	ss << "} // Stubs\n" << endl;

	ss << "#endif" << endl;

	ofstream fs(path + "/Tree.hpp");

	fs << ss.rdbuf();

}

void RuleMake::makeParser(const std::string& name, const std::string& ns, const std::string & path)
{
	stringstream ss;

	ss << "#ifndef PARSER_" << name << "_DEFS" << endl;
	ss << "#define PARSER_" << name << "_DEFS" << endl;

	ss << "#include \"Tree.hpp\"" << endl;
	ss << "#include \"LALR.h\"" << endl;
	ss << "#include \"LexerSymbols.hpp\"" << endl << endl;



	ss << "namespace Gpp \n{\n" << endl;
	ss << "namespace Lalr \n{\n\n" << endl;


	///make Symbol Types
	for (auto &s : f.SymbolTable)
	{
		if (s.second.Type == Egt::Symbol::Nonterminal)
			ss << "LALR_MAKE_SYMBOL_TYPE(" << s.first << ", Tree::" << make_token_name(s.second.Name) << ")" << endl;
	}
	ss << endl;
	///Make Symbol
	for (auto &s : f.SymbolTable)
	{
		if (s.second.Type == Egt::Symbol::Nonterminal)
		{
			ss << "LALR_MAKE_SYMBOL(" << s.first << ", " << s.first << ")";
			ss << "//" << make_token_name(s.second.Name) << endl;
		}
		else if (s.second.Type == Egt::Symbol::Terminal)
		{
			ss << "LALR_MAKE_SYMBOL(" << s.first << ", " << ns << "::LexerSymbols::" << make_token_name(s.second.Name) << ")" << endl;
		}
	}
	ss << endl;
	///Rules
	for (auto & r : f.Productions)
	{
		if (r.second.Symbols .size() == 0)
		{
			ss << "LALR_MAKE_EMPTY_RULE(" << r.first << ", " << r.second.HeadIndex;
		}
		else
		{
			ss << "LALR_MAKE_RULE(" << r.first << ", " << r.second.HeadIndex;
			for (auto &s : r.second.Symbols)
				ss << ", " << s;
		}
		auto &nm = f.SymbolTable.at(r.second.HeadIndex).Name;
		ss << ")//" << string(nm.begin(), nm.end()) << endl;
	}

	ss << endl;
	//States
	for (auto & s : f.LALRStates)
	{
		ss << "LALR_MAKE_STATE(" << s.first;

		for (auto &a : s.second.Actions)
		{
			ss << ", ";
			switch (a.Action)
			{
			case Egt::LALRState::Shift:
				ss << "LALR_MAKE_SHIFT_ACTION(";
				break;
			case Egt::LALRState::Reduce:
				ss << "LALR_MAKE_REDUCE_ACTION(";
				break;
			case Egt::LALRState::Goto:
				ss << "LALR_MAKE_GOTO_ACTION(";
				break;
			case Egt::LALRState::Accept:
				ss << "LALR_MAKE_ACCEPT_ACTION(";
				break;
			}
			ss  << a.TargetIndex << ", " << a.SymbolIndex << ")";
		}
		ss << ")" << endl;
	}


	ss << "} //Lalr" << endl;
	ss << "} //Gpp" << endl;

	ss << "namespace " << ns << "\n{" << endl;

	ss << "using Parser = Gpp::Lalr::StateMachine<";
	ss << f.InitialStates.LALR;
	ss << ">;" << endl;

	ss << "} //" << ns << endl;

	ss << "#endif" << endl;

	ofstream fs(path + "/ParserDef.hpp");
	fs << ss.rdbuf();
}


void makeRules(const Egt::File &f, const std::string& name, const std::string& ns, const std::string & path)
{
	RuleMake lx(f);
	lx.makeTree(name, ns, path);
	lx.makeParser(name, ns, path);

}

} /* namespace Egt */
