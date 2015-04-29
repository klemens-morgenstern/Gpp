/*
 * MakeLexer.cpp
 *
 *  Created on: 24.04.2015
 *      Author: Klemens
 */

#include "MakeLexer.h"
#include "../Egt/File.h"
#include <fstream>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string_regex.hpp>
#include <boost/optional.hpp>

#include <iostream>
using namespace std;

namespace Gpp
{

struct LexerMake
{
	const Egt::File &f;

	LexerMake(const Egt::File &f) : f(f) {};

	boost::optional<std::string> eof = boost::none;
	std::map<int, std::string> symbol_names;

	void make_symbols(const std::string& name, const std::string& ns, const std::string & path);
	string make_token_name(const wstring &in);
	void make_edge(const Egt::DFAState::Edge& e, std::stringstream &s);
	void make_lexerdef( const std::string& name, const std::string& ns, const std::string & path);
};

string LexerMake::make_token_name(const wstring &in)
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

void LexerMake::make_symbols(const std::string& name, const std::string& ns, const std::string & path)
{
	stringstream header;

	header << "/*                                                  " <<  endl;
	header << " *" << "LexerSymbols.hpp" <<  endl;
	header << " *                                                  " <<  endl;
	header << " *  Created on: "<< __DATE__ <<  endl;
	header << " *      Author: Gpp Generator					   " <<  endl;
	header << " */                                                 " <<  endl;
    header << "                                                    " <<  endl;

    auto include_guard = boost::to_upper_copy(ns) + "_LEXER_SYMBOLS_H_";
	header << "#ifndef " << include_guard <<  endl;
	header << "#define " << include_guard <<  endl << endl;

	header << "\n\n";

	header << "namespace " << ns << "\n{\n";

	header << "enum class LexerSymbols \n\t{" << endl;

	int i = 1;
	for (auto &s : f.SymbolTable)
	{
		using namespace Egt;
		auto type = s.second.Type;
		if (type == Symbol::Terminal)
		{
			auto nm = make_token_name(s.second.Name);
			symbol_names[s.first] = nm;
			header << "\t\t" << nm << " = " << i++  << ",\n";
		}
		else if (type == Symbol::EndOfFile)
		{
			auto ef = make_token_name(s.second.Name);
			if (ef == "EOF")
				ef = "Eof";
			symbol_names[s.first] = ef;
			header << "\t\t" << ef << " = " << i++  << ",\n";//" = ~0,\n";
			eof = ef;
		}
		else if (type == Symbol::Noise)
		{
			auto nm = make_token_name(s.second.Name);
			symbol_names[s.first] = nm;
			header << "\t\t" << nm << " = " << i++  << ",\n";//" =  ~1,\n";
		}
		else if (type == Symbol::GroupStart)
		{
			auto nm = make_token_name(s.second.Name) + "Start";
			symbol_names[s.first] = nm;
			header << "\t\t" << nm << " = " << i++  << ",\n";//" =  ~1,\n";
		}
		else if (type == Symbol::GroupEnd)
		{
			auto nm = make_token_name(s.second.Name) + "End";
			symbol_names[s.first] = nm;
			header << "\t\t" << nm << " = " << i++  << ",\n";//" =  ~1,\n";
		}

	}
	header << "\t};\n" << endl;

	header << "} // namespace " << ns << endl;
	header << endl;
	header << "#endif // " << include_guard << endl;

	ofstream file(path + "LexerSymbols.hpp");

	file << header.rdbuf() << endl;;
}

void LexerMake::make_edge(const Egt::DFAState::Edge& e, std::stringstream &s)
{
	s << "DFA_MAKE_EDGE(" << e.TargetIndex;
	auto & cs = f.CharacterSetTables.at(e.CharSetIndex);

	for (auto c : cs.Characters)
		s << ", DFA_CHAR(" << c << ")";

	for (auto p : cs.CharRanges)
		s << ", DFA_CHAR_RANGE(" << p.begin << ", " << p.end << ")";

	s << ")";
}

void LexerMake::make_lexerdef(const std::string& name, const std::string& ns, const std::string & path)
{
	stringstream lexerdef;

	lexerdef << "/*                                                  " <<  endl;
	lexerdef << " *" << name << ".cpp" <<  endl;
	lexerdef << " *                                                  " <<  endl;
	lexerdef << " *  Created on: "<< __DATE__ <<  endl;
	lexerdef << " *      Author: Gpp Generator					   " <<  endl;
	lexerdef << " */                                                 " <<  endl;
	lexerdef << endl;
	lexerdef << "#include \"LexerSymbols.hpp\"" << endl;
	lexerdef << "#include <string> " << endl;
	lexerdef << "#include \"Dfa.h\"\n" << endl;
	lexerdef << "namespace Gpp \n{" << endl;
	lexerdef << "namespace Dfa" << endl;
	lexerdef << "{" << endl;
    lexerdef << endl;

    for (auto &state : f.DFAStates)
    {
    	auto id = state.first;
    	if (state.second.AcceptState)
    	{
    		if (state.second.Edges.size() > 0)
    			lexerdef 	<< "DFA_MAKE_ACCEPT_STATE(";
    		else
    			lexerdef << "DFA_MAKE_END_STATE(";

    		lexerdef << id << ", " << ns << "::LexerSymbols::" << symbol_names.at(state.second.AcceptIndex);
    	}
    	else
    	{
    		if (state.second.Edges.size() > 0)
    			lexerdef << "DFA_MAKE_NOACCEPT_STATE(";
    		else
    			lexerdef << "DFA_MAKE_NOEND_STATE(";

    		lexerdef << id << ", " << ns << "::LexerSymbols";

    	}

		for (auto &e : state.second.Edges)
		{
			lexerdef << ", ";
			make_edge(e, lexerdef);
		}
		lexerdef << ")" << endl;

    }

    //    source <<

    lexerdef << endl;

	lexerdef << "} //Dfa" << endl;
	lexerdef << "} //Gpp" << endl;

	lexerdef << "namespace " << ns << "\n{" << endl;

	lexerdef << "template<typename Buffer = std::wstring>" << endl;
	lexerdef << "using Lexer = Gpp::Dfa::Lexer<Buffer, ";

	lexerdef << "typename Gpp::Dfa::StateDef<" <<f.InitialStates.DFA << ">::state";

	if (eof)
	{
		lexerdef << ", true, "<< ns << "::LexerSymbols::" << *eof;
	}
	lexerdef << ">;\n" << endl;

	lexerdef << "template<typename Buffer = std::wstring>" << endl;
	lexerdef << "using Token = Gpp::Dfa::Token<Buffer, CteParser::LexerSymbols>;" << endl;

	lexerdef << "} //" << ns <<endl;


	ofstream file(path + "LexerDef.hpp");

	file << lexerdef.rdbuf() << endl;;


}


void makeLexer(const Egt::File &f, const std::string& name, const std::string& ns, const std::string & path)
{

	LexerMake lx(f);
	lx.make_symbols (name, ns, path);
	lx.make_lexerdef(name, ns, path);

	//ofstream source(path + name + ".cpp");


}


} /* namespace Egt */
