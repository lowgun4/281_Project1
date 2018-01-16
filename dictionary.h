#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <string>
#include <vector>
#include <iostream>
#include "letterman.h"
using namespace std;

int is_changable(string first, string last);
int is_swappable(string first, string last);
int is_insertable(string first, string last);
int is_deletable(string first, string last);

class Dictionary {
public:
	Dictionary() {
		string buffer;
		bool is_simple;
		int dictionary_size;
		getline(cin, buffer);
		is_simple = (buffer.compare("S") == 0);
		getline(cin, buffer);
		dictionary_size = stoi(buffer);
		for (int i = 0; i < dictionary_size; ++i) {
			getline(cin, buffer);
			if (buffer.compare("") == 0)
				break;
			if (buffer.at(0) == '/' && buffer.at(1) == '/') {
				--i;
			} //if
			else if (is_simple) {
				word_bank.push_back(buffer);
			}//else if(is_simple)
			else {
				size_t special_char_index = buffer.find_first_of("&[!?");
				if (special_char_index != string::npos) {
					switch (buffer.at(special_char_index)) {
					case '&':
						buffer.erase(buffer.length() - 1, 1);
						word_bank.push_back(buffer);
						for (int j = static_cast<int>(buffer.length()) - 2; j >= 0; --j) {
							buffer += buffer.at(j);
						}
						word_bank.push_back(
							buffer.substr(buffer.length() / 2,
								buffer.length() / 2 + 1));
						break;
					case '[':
					{
						string extra_chars = buffer.substr(special_char_index
							+ 1, buffer.find_first_of("]") -
							special_char_index - 1);
						buffer.erase(special_char_index, extra_chars.length() + 1);
						for (size_t j = 0; j < extra_chars.length(); ++j) {
							buffer.at(special_char_index) = extra_chars.at(j);
							word_bank.push_back(buffer);
						}//for
						break;
					}
					case '!':
					{
						char ph = buffer.at(special_char_index - 1);
						buffer.erase(special_char_index, 1);
						word_bank.push_back(buffer);
						buffer.at(special_char_index - 1)
							= buffer.at(special_char_index - 2);
						buffer.at(special_char_index - 2) = ph;
						word_bank.push_back(buffer);
						break;
					}
					case '?':
					{
						buffer.at(special_char_index)
							= buffer.at(special_char_index - 1);
						string placeholder = buffer;
						buffer.erase(special_char_index, 1);
						word_bank.push_back(buffer);
						word_bank.push_back(placeholder);
						break;
					}
					default:
						cerr << "ERROR: Program was passed invalid special ";
						cerr << "character >" << buffer.at(special_char_index);
						cerr << endl;
						exit(1);
						break;
					}//switch
				}//else if (special_char_index != string::npos)
				else {
					word_bank.push_back(buffer);
				}
			}//else
		}//for(int i = 0; ...
	}
	int find(string word) {
		for (size_t i = 0; i < word_bank.size(); ++i) {
			if (word.at(0) == word_bank.at(i).at(0)) {
				if (word.length() == word_bank.at(i).length()) {
					if (word.compare(word_bank.at(i)) == 0) {
						return static_cast<int>(i);
					}
				}
			}
		}
		return -1;
	}
	size_t mark_as_begin(string begin_word) {
		word_bank.at(find(begin_word)).insert(0, "[&]");
		return find(begin_word);
	}
	void mark_as_reached(vector<string>::iterator it, size_t morph_index, 
		string mode) {
		string ph = "[]";
		ph.insert(1, to_string(morph_index));
		ph.insert(1, mode);
		it->insert(0, ph);
		return;
	}
	void print_path(size_t end_index, string output) {
		string next_step = word_bank.at(end_index);
		string path_result = "";
		string buffer;
		bool is_word = (output.compare("W") == 0);
		int words_in_morph = 1;
		while (next_step.at(1) != '&') {
			size_t next_step_index = stoi(next_step.substr(2, 
				next_step.find_first_of("]") - 1));
			string step_word = next_step.substr(next_step.find_first_of("]") + 1);
			string next_next_step = word_bank.at(next_step_index);

			path_result.insert(0, "\n");
			if (is_word) {
				path_result.insert(0, step_word);
			}
			else {
				buffer = next_step.substr(1, 1);
				buffer += ",";
				int morph_index;
				switch (buffer.at(0)) {
				case 'c':
					morph_index = is_changable(next_next_step.substr
					(next_next_step.find_first_of("]") + 1), step_word);
					buffer += to_string(morph_index);
					buffer += ",";
					buffer += next_step.substr(next_step.find_first_of("]")
						+ morph_index + 1, 1);
					break;
				case 's':
					morph_index = is_swappable(next_next_step.substr
					(next_next_step.find_first_of("]") + 1), step_word);
					buffer += to_string(morph_index);
					break;
				case 'i':
					morph_index = is_insertable(next_next_step.substr
					(next_next_step.find_first_of("]") + 1), step_word);
					buffer += to_string(morph_index);
					buffer += ",";
					buffer += next_step.substr(next_step.find_first_of("]")
						+ morph_index + 1, 1);
					break;
				case 'd':
					morph_index = is_deletable(next_next_step.substr
					(next_next_step.find_first_of("]") + 1), step_word);
					buffer += to_string(morph_index);
					break;
				}
				path_result = buffer + path_result;
			}
			next_step = next_next_step;
			++words_in_morph;
		}

		path_result.insert(0, "\n");
		path_result.insert(0, next_step.substr(next_step.find_first_of("]")
				+ 1));
		path_result.insert(0, "\n");
		path_result.insert(0, to_string(words_in_morph));
		path_result.insert(0, "Words in morph: ");
		cout << path_result;
	}
	//prints message for when no solution is found
	//No solution, X words checked.
	void print_no_path() {
		int words_reached = 0;
		vector<string>::iterator it = word_bank.begin();
		while(it != word_bank.end()) {
			if(it->at(0) == '[')
				++words_reached;
			++it;
		}
		cout << "No solution, " << words_reached << " words checked.\n";
	}

	//returns morph index given dictionary iterator
	//returns -1 if no such morph index exists
	int get_morph_index(vector<string>::iterator it) {
		if (it->at(0) != '&')
			return -1;
		else
			assert(it->length() > it->find_first_of("]") - 1);
			return stoi(it->substr(1, it->find_first_of("]") - 1));
	}

	int num_words_checked() {
		int count = 0;
		for (vector<string>::iterator it = begin(); it != end(); ++it) {
			if (it->at(0) == '[')
				++count;
		}
		return count;
	}

	vector<string>::iterator begin() {
		return word_bank.begin();
	}
	vector<string>::iterator end() {
		return word_bank.end();
	}

private:
	//each entry in dictionary is unreached, unless it begins 
	//with a '['. once it becomes reach it is stored with a  "[<mode><morph index>]"
	//preceeding it.
	//morph index is the dictionary index of the 
	vector<string> word_bank;
};

int is_changable(string first, string last) {
		if (first.length() != last.length()) {
			return -1;
		}
		int changing_letter = -1;
		for (size_t i = 0; i < first.length(); ++i) {
			if (first.at(i) != last.at(i)) {
				if (changing_letter == -1)
					changing_letter = static_cast<int>(i);
				else
					return -1;
			}
		}
		if (changing_letter == -1)
			cerr << "ERROR: testing on the same word in is_changeable\n";
		return changing_letter;
	}
	//returns the new index of the first swapable letter in first
	//if impossible, returns -1
	int is_swappable(string first, string last){
		if (first.length() != last.length()) {
			return -1;
		}
		int swapping_letter = -1;
		for (size_t i = 0; i < first.length(); ++i) {
			if (first.at(i) != last.at(i)) {
				if (i != first.length() - 1 && first.at(i) == last.at(i + 1)
					&& last.at(i) == first.at(i + 1) && swapping_letter == -1) {
					swapping_letter = static_cast<int>(i);
					++i;
				}
				else
					return -1;
			}
		}
		if (swapping_letter == -1)
			cerr << "ERROR: testing on the same word in is_swapable\n";
		return swapping_letter;
	}
	//returns the new index of the inserted letter in first
	//if impossible, returns -1
	int is_insertable(string first, string last) {
		if (first.length() != last.length() - 1) {
			return -1;
		}
		int inserting_letter = -1;
		size_t j = 0;
		for (size_t i = 0; i < first.length(); ++i, ++j) {
			if (first.at(i) != last.at(j)) {
				if(inserting_letter == -1){
					inserting_letter = static_cast<int>(j);
					--i;
				}
				else
					return -1;
			}
		}
		if (inserting_letter == -1)
			return static_cast<int>(j);
		return inserting_letter;
	}
	//returns the index of the deletable letter in first
	//if impossible, returns -1
	int is_deletable(string first, string last) {
		if (first.length() != last.length() + 1) {
			return -1;
		}
		int deleting_letter = -1;
		size_t j = 0;
		for (size_t i = 0; j < last.length(); ++i, ++j) {
			if (first.at(i) != last.at(j)) {
				if (deleting_letter == -1) {
					deleting_letter = static_cast<int>(i);
					--j;
				}
				else
					return -1;
			}
		}
		if (deleting_letter == -1)
			return static_cast<int>(j);
		return deleting_letter;
	}
#endif

