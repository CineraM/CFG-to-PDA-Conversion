//
// Driver program to run the conversions of CFG to PDA.
//
#include "pushdown.hpp"

using namespace std;

int main()
{
    pushdown grammar;
    grammar.read_input("grammar.txt");
    cout << "Reading file..." << endl;
    grammar.display_grammar(true);
    grammar.simplify();
    cout << "Displaying simplified grammar: " << endl;
    grammar.display_grammar(true);
    grammar.convert_GNF();
    cout << "Displaying GNF conversion: " << endl;
    grammar.display_grammar(false);
    grammar.convert_PDA();
    grammar.display_table();

    return 0;
}