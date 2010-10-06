#include <iostream>
#include <fstream>
#include <list>
#include "scanner.h"
#include "parser.h"
#include <sstream>
#include <string.h>
#include <exception>
using namespace std;


class CompilerException: std::exception{
private:
    char *msg;
public:
    CompilerException();
    CompilerException(const char* const msg);
    virtual const char* what() const throw();
};

CompilerException::CompilerException():
    msg(NULL)
{
}

CompilerException::CompilerException(const char* const msg):
    msg(strcpy(new char[strlen(msg) + 1], msg))
{
}

const char* CompilerException::what() const throw()
{
    return msg;
}


struct ConsoleKeys{
	bool PrintLexems;
};

void PrintHelp()
{
	cout << "Pascal Compiler\n\
Usage: compiler [options] source.pas\n\
\n\
avaible options are:\n\
    -h  show this message\n\
    -l  show lexems stream\
	";
}

ConsoleKeys ParseParametrs(int argc, char* argv[], list<string>& returned_files)
{
	ConsoleKeys keys;
	memset(&keys, 0, sizeof(keys));
	bool was_file = false;
	for (int i = 1; i < argc; ++i)
		if (argv[i][0] == '-')
		{
			if (!argv[i][1]) throw CompilerException("Invalid argument");
			if (argv[i][2]) throw CompilerException("Invalid argument");
			if (was_file) throw CompilerException("Invalid argunet");
				for (int j = 1; argv[i][j]; ++j)
					switch (argv[i][j])
					{
					case 'l':
						keys.PrintLexems = true;
					}
		}
		else
			{
				returned_files.push_back(argv[i]);
				was_file = true;
			}
	return keys;
}

char *my_argv[] = {"compiler.exe", "-l", "test/16.in" };

int main(int argc, char* argv[])
{
//	argv = my_argv;
//	argc = 3;
//    for (int i=0; i < argc; ++ i)
//        cout << argv[i] << endl;

    stringstream ss;
    ss << " 5 + (4*5) + 4 div 3";
//    ss << " 5 + 4";
    Scanner scan(ss);
    Parser p(scan);
    p << (cout);
    return 0;

	ConsoleKeys keys;
	list<string> files;
	if (argc == 1) PrintHelp();
	try
	{
		keys = ParseParametrs(argc, argv, files);
		if (keys.PrintLexems)
		{
			if (files.empty()) throw CompilerException("no files specefied");
			ifstream in;
			in.open(files.front().c_str(), ios::in);
			if (!in.good()) throw CompilerException("can't open file");
			files.pop_front();
			Scanner scan(in);
		    for (Token t; t.GetType() != END_OF_FILE;)
			{
				cout << ( t = scan.NextToken() );
			}
		}
	}
    catch (Scanner::exception e)
	{
        cout << e.what() << endl;
	}
	catch (CompilerException e)
	{
        cout << "Fuck!!! We have error!!!" << ' ' << e.what() << endl;
	}
	catch (exception e)
	{
		cout << "Fuck!!! We have error!!!" << ' ' << e.what() << endl;
	}
	return 0;
}
