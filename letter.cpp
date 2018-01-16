#include <string>
#include <iostream>
#include "letterman.h"
#include "dictionary.h"
#include "getopt.h"
using namespace std;

string read_arguments(int argc, char *argv[]);

int main(int argc, char*argv[]) {
	string arguments = read_arguments(argc, argv);
	bool stack_routing = (arguments.at(0) == 's');
	bool change_mode = 
		(arguments.substr(2, 3).find_first_of("c") != string::npos);
	bool swap_mode =
		(arguments.substr(2, 3).find_first_of("p") != string::npos);
	bool length_mode =
		(arguments.substr(2, 3).find_first_of("l") != string::npos);
	string word_format = arguments.substr(6,1);
	string begin_word = arguments.substr(8, 32);
	begin_word = begin_word.substr(0, begin_word.find_first_of("*"));
	string end_word = arguments.substr(41, 32);
	end_word = end_word.substr(0, end_word.find_first_of("*"));
	Dictionary local_dict = Dictionary();
	if (local_dict.find(begin_word) == -1) {
		cerr << "ERROR: beginning word is not in dictionary >" << begin_word;
		cerr << endl;
		exit(1);
	}
	else if (local_dict.find(end_word) == -1) {
		cerr << "ERROR: ending word is not in dictionary >" << begin_word;
		cerr << endl;
		exit(1);
	}
	Letterman dayman(stack_routing, change_mode, swap_mode, length_mode, begin_word, end_word);
	dayman.search_for_path(local_dict, word_format);
}

string read_arguments(int argc, char * argv[]) {
	bool routing_specified = false;
	char routing_method;
	string morph_modes = "";
	string output = "";
	string begin_word = "";
	string end_word = "";
	opterr = true;
	int choice;
	int option_index = 0;
	option long_options[] = {
		{ "stack",	no_argument, nullptr, 's' },
		{ "queue",	no_argument, nullptr, 'q' },
		{ "change",	no_argument, nullptr, 'c' },
		{ "swap",	no_argument, nullptr, 'p' },
		{ "length",	no_argument, nullptr, 'l' },
		{ "output",	required_argument, nullptr, 'o' },
		{ "begin",	required_argument, nullptr, 'b' },
		{ "end",	required_argument, nullptr, 'e' },
		{ "help",	no_argument, nullptr, 'h' },
		{ nullptr,	0, nullptr, '\0' }
	};
	while ((choice = getopt_long(argc, argv, "sqcplo:b:e:h", long_options,
		&option_index)) != -1) {
		switch (choice) {
		case 's':
		case 'q':
			if (routing_specified) {
				cerr << "ERROR: Routing method specified twice\n";
				exit(1);
			}
			else {
				routing_specified = true;
				routing_method = static_cast<char>(choice);
			}
			break;
		case 'c':
		case 'p':
		case 'l':
			morph_modes += static_cast<char>(choice);
			break;
		case 'o':
			if (output.compare("") != 0) {
				cerr << "ERROR: multiple output flags in command line\n";
				exit(1);
			}
			output = optarg;
			if (output.compare("W") * output.compare("M") != 0) {
				cerr << "ERROR: invalid output flag >" << output << endl;
				exit(1);
			}

			break;
		case 'b':
			if (begin_word.compare("") != 0) {
				cerr << "ERROR: beginning word already specified >";
				cerr << begin_word << endl;
				exit(1);
			}
			begin_word = optarg;
			break;
		case 'e':
			if (end_word.compare("") != 0) {
				cerr << "ERROR: ending word already specified >";
				cerr << end_word << endl;
				exit(1);
			}
			end_word = optarg;
			break;
		case 'h':
			cout << "Hey! This is the help message!";
			cout << "Figure shit out on your own" << endl;
			exit(0);
			break;

		default:
			cerr << "ERROR: invalid option >" << choice << endl;
			exit(1);

		} // switch
	} // while

	if (output.compare("") == 0)
		output += 'W';
	if (!routing_specified) {
		cerr << "Error: no routing method specified" << endl;
		exit(1);
	} // if
	if (morph_modes.length() == 0) {
		cerr << "ERROR: You must specify at least one of c, l, or p\n";
		exit(1);
	}//if
	if (end_word.length() > 32 || begin_word.length() > 32) {
		cerr << "ERROR: end or begin word length exceeds 16 char limit";
		cerr << " >" << begin_word << " >" << end_word << endl;
		exit(1);
	}//if
	if (end_word.compare("") == 0 || begin_word.compare("") == 0) {
		cerr << "ERROR: end or begin word not specified";
		cerr << " >" << begin_word << " >" << end_word << endl;
		exit(1);
	}

	//returns string of the following indicators, seperated by a '|'
	//1. [1 char] routing method as single char (q or s)
	//2. [3 char] morph modes as chars in no order. if 3 morph modes are not
	//		specified, remaining spaces filled with *. (c, l, and/or p)
	//3. [1 char] output as single char (W or M)
	//4. [32 char] begin_word, remaining spaces filled by *
	//5. [32 char] end word, remaining spaces filled by *
	//EX: 
	//q|sl*|M|booty***************************|hero****************************
	
	string formatted_arguments = "";
	formatted_arguments += routing_method;
	formatted_arguments += '|';
	formatted_arguments.append(morph_modes);
	formatted_arguments.append(3 - morph_modes.length(), '*');
	formatted_arguments += '|';
	formatted_arguments.append(output);
	formatted_arguments += '|';
	formatted_arguments.append(begin_word);
	formatted_arguments.append(32 - begin_word.length(), '*');
	formatted_arguments += '|';
	formatted_arguments.append(end_word);
	formatted_arguments.append(32 - end_word.length(), '*');

	return formatted_arguments;

} // read_arguments()