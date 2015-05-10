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
	void makeParser(const std::string& name, const std::string& ns, const std::string & path);
	void makeSymbolTypes(const std::string& name, const std::string& ns, const std::string & path);
	void makeRuleActions(const std::string& name, const std::string& ns, const std::string & path);

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

void RuleMake::makeParser(const std::string& name, const std::string& ns, const std::string & path)
{
	stringstream rule_enum;

	rule_enum << "#ifndef PARSER_RULES" << endl;
	rule_enum << "#define PARSER_RULES\n" << endl;
	rule_enum << "namespace " << ns << "{" << endl;
	rule_enum << "enum class Rules \n{" << endl;

	stringstream ss;

	ss << "#ifndef PARSER_" << name << "_DEFS" << endl;
	ss << "#define PARSER_" << name << "_DEFS\n" << endl;

	ss << "#include \"LALR.h\"" << endl;
	ss << "#include \"Symbols.hpp\"" << endl;
	ss << "#include \"Rules.hpp\"" << endl;
	ss << "#include \"Actions.hpp\"" << endl;
	ss << "#include \"SymbolTypes.hpp\"" << endl;
	ss << "#include \"RuleActions.hpp\"" << endl << endl;



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
		else if (s.second.Type == Egt::Symbol::Nonterminal)
		{
			ss << "LALR_MAKE_SYMBOL_TYPE(" << s.first << ", " << ns << "::SymbolTypes::" << make_token_name(s.second.Name) << ")" << endl;
		}
	}
	ss << endl << endl;
	///DEFINE the rule actions.
	std::set<string> double_names; //to prevent double naming.

	for (auto &r : f.Productions)
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

		ss << "LALR_DEFINE_RULE_ACTION_BEGIN(" << ns << "::Rules::" << name << ", " << ns << "::RuleActions::" << name << ");" << endl;
	}

	ss << "\n\n";

	//START the mother
	ss << "LALR_MAKE_PARSER(" << lower << ", " << upper << ")\n//{" << endl;

	///Rules

	ss << "\tstruct Rules\n\t{" << endl;

	ss << "\t\tusing Token = typename Sm_t::Token;" << endl;
	ss << "\t\tusing Context = typename Sm_t::Context;" << endl;
	ss << "\t\tusing Stack = typename Sm_t::Stack;" << endl;

	for (auto & r : f.Productions)
	{


		string name = RuleNames[r.first];
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

void RuleMake::makeSymbolTypes(const std::string& name, const std::string& ns, const std::string & path)
{
	stringstream ss;

	ss << "#ifndef SYMBOLTYPE_DEFS" << endl;
	ss << "#define SYMBOLTYPE_DEFS" << endl;

	ss << "#include <boost/none.hpp>\n" << endl;

	ss << "namespace " << ns << "\n{" << endl;
	ss << "namespace SymbolTypeDef\n{\n" << endl;

	for (auto& r : f.SymbolTable)
	{
		if (r.second.Type == Egt::Symbol::Nonterminal)
		{
			ss << "using " << make_token_name(r.second.Name) << " = boost::none_t;" << endl;
		}
	}


	ss << "}" << endl;
	ss << "namespace SymbolTypes\n{\n";
	ss << "using namespace SymbolTypeDef;" << endl;
	ss << "\n}\n}" << endl;
	ss << "#endif" << endl;
	ofstream fs(path + "/SymbolTypeDef.hpp");
	fs << ss.rdbuf();
}

void RuleMake::makeRuleActions(const std::string& name, const std::string& ns, const std::string & path)
{
	stringstream ss;  //used for predeclaration

	ss << "#ifndef RULEACTION_DEFS" << endl;
	ss << "#define RULEACTION_DEFS" << endl;

	ss << "#include <boost/none.hpp>" << endl;
	ss << "#include \"SymbolTypes.hpp\"" << endl;

	ss << "namespace " << ns << "\n{" << endl;
	ss << "namespace RuleActionDef\n{\n" << endl;

	ss << "using namespace " << ns << "::SymbolTypes;\n" << std::endl;
	std::set<string> double_names; //to prevent double naming.

//this loop collects the declaration and puts them into the stream
	for (auto& r : f.Productions)
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
		ss << "struct " << name << ";" << endl;
	}

	ss << endl;

	for (auto& r : f.Productions)
	{
		stringstream s_args; //argument list
		bool has_token = false;
		for (auto s : r.second.Symbols)
		{
			auto sym = f.SymbolTable.at(s);
			if (sym.Type == Egt::Symbol::Nonterminal)
			{
				s_args << ", " << ns << "::SymbolTypes::" << make_token_name(sym.Name) << "&& ";;
			}
			else
			{
				s_args << ", Token && /*" << make_token_name(sym.Name) << "*/";
				has_token = true;
			}
		}


		ss << "struct " << RuleNames[r.first] << "\n{\n";

		ss << "\tusing Type = " << ns << "::SymbolTypes::" << make_token_name(f.SymbolTable.at(r.second.HeadIndex).Name) << ";\n\n";

		if (has_token)
			ss << "\ttemplate<typename Context, typename Token>\n";
		else
			ss << "\ttemplate<typename Context>\n";

		ss << "\tstatic Type Action(Context& ctx" << s_args.str() << ")\n";
		ss << "\t{\n\t\treturn Type();\n\t}" << endl;
		ss << "};\n" << endl;
	}


	ss << "\n}" << endl;
	ss << "namespace RuleActions  \n{\n";
	ss << "using namespace RuleActionDef;" << endl;
	ss << "\n}\n}" << endl;
	ss << "#endif" << endl;
	ofstream fs(path + "/RuleActionDef.hpp");
	fs << ss.rdbuf();
}



void makeRules(const Egt::File &f, const std::string& name, const std::string& ns, const std::string & path)
{
	RuleMake lx(f);
	lx.makeParser(name, ns, path);
	lx.makeSymbolTypes(name, ns, path);
	lx.makeRuleActions(name, ns, path);
}

} /* namespace Egt */
