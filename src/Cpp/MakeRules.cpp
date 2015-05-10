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
#include <map>

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
	static string make_symbol_name(const Egt::Symbol &sym)
	{
		if (sym.Type == Egt::Symbol::Nonterminal)
			return "NonTerminal_" + make_token_name(sym.Name);
		else if (sym.Type == Egt::Symbol::EndOfFile) //not clean
			return "Eof";
		else
			return  make_token_name(sym.Name);
	}
	std::map<int, string> RuleNames;
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
	stringstream rule_enum;

	rule_enum << "#ifndef PARSER_RULES" << endl;
	rule_enum << "#define PARSER_RULES\n" << endl;
	rule_enum << "namespace " << ns << "{" << endl;
	rule_enum << "enum class Rules \n{" << endl;

	stringstream ss;

	ss << "#ifndef PARSER_" << name << "_DEFS" << endl;
	ss << "#define PARSER_" << name << "_DEFS" << endl;

	ss << "#include \"Tree.hpp\"" << endl;
	ss << "#include \"LALR.h\"" << endl;
	ss << "#include \"Symbols.hpp\"" << endl;
	ss << "#include \"Rules.hpp\"" << endl;
	ss << "#include \"Actions.hpp\"" << endl << endl;



	ss << "namespace Gpp \n{\n" << endl;
	ss << "namespace Lalr \n{\n\n" << endl;

	int lower = 10000;
	int upper = 0;
	///Make Symbol
	for (auto &s : f.SymbolTable)
	{
		if (s.first < lower)
			lower = s.first;

		if (s.first > upper)
			upper = s.first;

		if (s.second.Type == Egt::Symbol::Terminal)
		{
			ss << "LALR_MAKE_SYMBOL_TOKEN(" << s.first << ")" << endl;
		}
	}
	ss << endl << endl;

	//START the mother
	ss << "LALR_MAKE_PARSER(" << lower << ", " << upper << ")\n//{" << endl;

	///Rules

	ss << "\tstruct Rules\n\t{" << endl;

	ss << "\t\tusing Token = typename Sm_t::Token;" << endl;
	ss << "\t\tusing Context = typename Sm_t::Context;" << endl;
	ss << "\t\tusing Stack = typename Sm_t::Stack;" << endl;

	std::set<string> double_names; //to prevent double naming.
	for (auto & r : f.Productions)
	{
		auto &nm = f.SymbolTable.at(r.second.HeadIndex).Name;
		auto nms = make_token_name(nm);
		string name;
		if (double_names.count(nms) != 0)
		{
			int i = 0;
			while (double_names.count(nms + std::to_string(i)) != 0) i++;

			name = nms + std::to_string(i);
			double_names.emplace(name);

		}
		else
		{
			name = nms;
			double_names.emplace(name);
		}

		RuleNames[r.first] = name;

		rule_enum << "\t" << name << " = " << r.first << ", \n";


		if (r.second.Symbols .size() == 0)
		{
			ss << "\t\tLALR_MAKE_EMPTY_RULE(" //<<r.first
					<< ns << "::Rules::" << name
					<< ", " << name << ", " << r.second.HeadIndex;
		}
		else
		{
			ss << "\t\tLALR_MAKE_RULE(" //<< r.first
					<< ns << "::Rules::" << name
					<< ", " << name<< ", " << r.second.HeadIndex;
			for (auto &s : r.second.Symbols)
				ss << ", " << s;
		}
		ss << ")//Arguments: ";

		for (auto s : r.second.Symbols)
		{
			auto sm = f.SymbolTable.at(s).Name;
			ss << string(sm.begin(), sm.end()) << ", ";
		}

		ss << endl;
	}

	ss << "\t};\n\n\tRules rules;\n" << endl;
	//States
	for (auto & s : f.LALRStates)
	{
		ss << "\tLALR_MAKE_STATE(" << s.first;

		for (auto &a : s.second.Actions)
		{
			ss << ", ";

			auto sym = make_symbol_name(f.SymbolTable.at(a.SymbolIndex));


			switch (a.Action)
			{
			case Egt::LALRState::Shift:
				ss << "LALR_MAKE_SHIFT_ACTION(";
				ss << ns << "::Symbols::" << sym;
				ss << ", " << a.TargetIndex << ")";
				break;
			case Egt::LALRState::Reduce:
				ss << "LALR_MAKE_REDUCE_ACTION(" ;
				ss << ns << "::Symbols::" << sym;
				ss << ", rules." << RuleNames[a.TargetIndex] << ")";
				break;
			case Egt::LALRState::Goto:
				ss << "LALR_MAKE_GOTO_ACTION(";
				ss << ns << "::Symbols::" << sym;
				ss << ", " << a.TargetIndex << ")";
				break;
			case Egt::LALRState::Accept:
				ss << "LALR_MAKE_ACCEPT_ACTION(" << a.TargetIndex << ")";
				break;
			}
		}
		ss << ")" << endl;
	}

	ss << "\tParser() : Sm_t(State" << f.InitialStates.LALR	<< ") {};\n" << endl;
	ss << "};\n" << endl;

	ss << "} //Lalr" << endl;
	ss << "} //Gpp" << endl;

	ss << "namespace " << ns << "\n{" << endl;
	ss << "using Gpp::Lalr::Parser;" << endl;

	ss << "} //" << ns << endl;

	ss << "#endif" << endl;

	ofstream fs(path + "/ParserDef.hpp");
	fs << ss.rdbuf();

	rule_enum << "};" << endl;
	rule_enum << "}//" << ns << "\n#endif" << endl;

	ofstream fs2(path + "/Rules.hpp");
	fs2 << rule_enum.rdbuf();
}


void makeRules(const Egt::File &f, const std::string& name, const std::string& ns, const std::string & path)
{
	RuleMake lx(f);
	lx.makeTree(name, ns, path);
	lx.makeParser(name, ns, path);

}

} /* namespace Egt */
