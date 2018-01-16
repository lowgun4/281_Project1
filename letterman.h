#ifndef LETTERMAN_H
#define LETTERMAN_H

#include <string>
#include <deque>
#include <cassert>
#include "dictionary.h"
using namespace std;


class Letterman {
public:
	Letterman(bool stack, bool change, bool swap, bool length, 
		string begin, string end) {
		stack_routing = stack;
		change_mode = change;
		swap_mode = swap;
		length_mode = length;
		begin_word = begin;
		end_word = end;
		word_stack.push_back(begin_word);
	}
	void search_for_path(Dictionary dict, string output) {
		word_stack.at(0).insert(0, "[]");
		word_stack.at(0).insert(1, to_string(dict.find(begin_word)));
		dict.mark_as_begin(begin_word);
		//while loop that checks if stack is empty
		while (word_stack.size() != 0) {
			//remove top word from stack: s
			string current_word = stack_routing
				? word_stack.back() : word_stack.front();
			stack_routing ? word_stack.pop_back() : word_stack.pop_front();
			//removes beginning section and initializes size_t current_index
			assert(current_word.length() > current_word.find_first_of("]") - 1 + 1);
			size_t current_index = stoi(current_word.substr(1, 
				current_word.find_first_of("]") - 1));
			current_word.erase(0, current_word.find_first_of("]") + 1);
			//index keeps track of current index of dictionary entry
			size_t dict_index = 0;

			string buffer;
			//for loop that traverses the dictionary: it
			for (vector<string>::iterator it = dict.begin(); it != dict.end(); ++it) {
				//skips d if d is marked as reached, e.g. first char of d is [
				if (it->at(0) == '[') {
					++dict_index;
					continue;
				}
				if(change_mode) {
					//if change_mode, check if s can be changed to d
					int change_index = is_changable(current_word, *it);
					if (change_index != -1) {
						if (it->compare(end_word) == 0) {
							dict.mark_as_reached(it, current_index, "c");
							dict.print_path(dict_index, output);
							return;
						}
						//add d to stack
						buffer = "[]";
						buffer.insert(1, to_string(dict_index));
						buffer += *it;
						word_stack.push_back(buffer);
						//mark d as reached
						dict.mark_as_reached(it, current_index, "c");
					}

				}
				if (swap_mode) {
					//if swap_mode, check if s can be swapped to d
					int swap_index = is_swappable(current_word, *it);
					if (swap_index != -1) {
						if (it->compare(end_word) == 0) {
							dict.mark_as_reached(it, current_index, "s");
							dict.print_path(dict_index, output);
							return;
						}
						//add d to stack
						buffer = "[]";
						buffer.insert(1, to_string(dict_index));
						buffer += *it;
						word_stack.push_back(buffer);
						//mark d as reached
						dict.mark_as_reached(it, current_index, "s");
					}

				}
				if (length_mode) {
					//if length_mode, check if s can be inserted to d
					int insert_index = is_insertable(current_word, *it);
					if (insert_index != -1) {
						if (it->compare(end_word) == 0) {
							dict.mark_as_reached(it, current_index, "i");
							dict.print_path(dict_index, output);
							return;
						}
						//add d to stack
						buffer = "[]";
						buffer.insert(1, to_string(dict_index));
						buffer += *it;
						word_stack.push_back(buffer);
						//mark d as reached
						dict.mark_as_reached(it, current_index, "i");
					}
					//if length_mode, check if s can be deleted to d
					int delete_index = is_deletable(current_word, *it);
					if (delete_index != -1) {
						if (it->compare(end_word) == 0) {
							dict.mark_as_reached(it, current_index, "d");
							dict.print_path(dict_index, output);
							return;
						}
						//add d to stack
						buffer = "[]";
						buffer.insert(1, to_string(dict_index));
						buffer += *it;
						word_stack.push_back(buffer);
						//mark d as reached
						dict.mark_as_reached(it, current_index, "d");
					}

				}
				++dict_index;
			}//for(vector<string::iterator it ...
		}//while(word_stack.size() != 0
				
		//print out unpassable message
		dict.print_no_path();
	}

private:
	bool stack_routing;
	bool change_mode;
	bool swap_mode;
	bool length_mode;
	string begin_word;
	string end_word;
	//each entry in word stack is preceeded by "[<index>]", with index being
	//its index in dictionary
	deque<string> word_stack;
};


#endif
