#ifndef PUSHDOWN_H
#define PUSHDOWN_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <iomanip>

// Object to hold pushdown automata.
class pushdown
{   
    // Struct to hold the transfer functions of a PDA.
    struct PDA {
        char state;
        std::string read;
        std::string pop;
        std::string push;

        // Overloaded operator to find equal structs.
        bool operator==(PDA l)
        {
            if (l.state == state && l.read == read && l.pop == pop && l.push == push)
            {
                return true;
            }
            else {
                return false;
            }
        }
    };


    // Data members PDA;
    private:
        std::vector<PDA> automaton;
        std::vector<std::pair<std::string, std::string>> GNF;
        std::vector<std::pair<char, std::string>> grammar;
        std::vector<std::pair<char, std::string>> single_terminals;
        std::vector<std::pair<std::string, std::string>> GNF_singles;

    // Function decs.
    public:
        void read_input(std::string filename);
        void simplify();
        void lambda_removal();
        void unit_removal();
        void useless_removal();
        void sort_grammar();
        void convert_GNF();
        void convert_PDA();
        void display_table();
        void display_graph();
        void display_grammar(bool GNF_or_grammar);

};

#endif