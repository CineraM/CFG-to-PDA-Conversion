#include "pushdown.hpp"

using namespace std;

// Function to read grammar from file.
void pushdown::read_input(string filename)
{
    ifstream input(filename);
    if(!input.is_open()) 
    {
        cout << "No file found!" << endl << endl;
    }
    
    char variable;
    string temp, production;
    
    // Read input line by line.
    while (getline(input, temp)) 
    {
            variable = temp[0];
            production = temp.substr(2, temp.size());
            // Separate container for productions with a single terminal.
            if ((production.length() == 1) && ((production >= "a") && (production <= "z")))
            {
                single_terminals.push_back(make_pair(variable, production));
            }
            grammar.push_back(make_pair(variable, production));
    }
    input.close();
}

// Function to simpilify grammar.
void pushdown::simplify()
{
    lambda_removal();
    unit_removal();
    useless_removal();
}

// Function to remove lambda productions.
void pushdown::lambda_removal()
{   
    char key;
    string temp;
    vector<pair<char, string> > temp_vec;

    for(unsigned int i = 0; i < grammar.size(); i++)
    {
        if(grammar[i].second[0] == '?'){     
            key = grammar[i].first;
            for(unsigned int j = 0; j < grammar.size(); j++){
                temp = grammar[j].second;
                for(string::size_type u = 0; u < grammar[j].second.size(); u++){ 
                    if(grammar[j].second[u] == key){ //u = production
                        temp.erase(u, 1);
                        temp_vec.push_back(make_pair(grammar[j].first, temp)); // Push new productions into temp container.
                    }
                }
            }
        }
    }

    // Push new productions into grammar.
    for(unsigned int i = 0; i < temp_vec.size(); i++)
        grammar.push_back(temp_vec[i]);

    // Final removal of lambda productions, empty productions, and duplicates.
    for(unsigned int i = 0; i < grammar.size(); i++){
        if(grammar[i].second == "?") // Lambda removal.
            grammar.erase(grammar.begin() + i);   
        if (grammar[i].second == "\0") // Empty production removal.
        {
            grammar.erase(grammar.begin() + i);
        }

        for(unsigned int j = i + 1; j < grammar.size(); ++j) // Duplicate removal.
        {
            if (grammar[i] == grammar[j])
            {
                grammar.erase(grammar.begin() + j);
            }
        }
        // First checks for productions > 1.
        if (grammar[i].second.size() > 1)
        {   
            // Then adds any productions made up of only terminals to another vector for later use.
            for(string::size_type u = 0; u < grammar[i].second.size(); u++)
            {   
                string l = grammar[i].second;
                if ((l[u] >= 'A') && (l[u] <= 'Z'))
                    break;
                else if (u == l.size() - 1)
                    single_terminals.push_back(make_pair(grammar[i].first, grammar[i].second));
            }
        }
        // Checks for productions == 1 and if they are terminal.
        if (grammar[i].second.size() == 1)
        {
            if (grammar[i].second >= "a" && grammar[i].second <= "z")
            {
                single_terminals.push_back(make_pair(grammar[i].first, grammar[i].second));
            }
        }
    }
}

// Function to remove unit productions.
void pushdown::unit_removal()
{
    string temp;
    vector<pair<char, string>> temp_vec, temp_singles;

    for (auto it = grammar.begin(); it != grammar.end(); ++it)
    {   
        // First find productions of length = 1 and non-terminal.
        if (it->second.length() == 1 && (isupper(it->second[0])))
        {
            // Now iterate through the single_terminals vector to find anything that matches.
            for (auto jt = single_terminals.begin(); jt != single_terminals.end(); ++jt)
            {
                if (jt->first == it->second[0])
                {
                    temp_vec.push_back(make_pair(it->first, jt->second));
                }
            }
        }
    }

    // Push new productions.
    for(unsigned int i = 0; i < temp_vec.size(); i++)
        grammar.push_back(temp_vec[i]);

    // Final removal of unit productions.
    for(unsigned int i = 0; i < grammar.size(); i++)
    {
        if (grammar[i].second.length() == 1)
        {
            if (isupper(grammar[i].second[0]))
            {
                grammar.erase(grammar.begin() + i);
            }
            else if (islower(grammar[i].second[0]))
            {
                temp_singles.push_back(make_pair(grammar[i].first, grammar[i].second));
            }
        }

        for(auto j = i + 1; j < grammar.size(); ++j) // Duplicate removal.
        {
            if (grammar[i] == grammar[j])
            {
                grammar.erase(grammar.begin() + j);
            }
        }
    }

    // Adds any new production rules with a single terminal to container.
    for (unsigned int i = 0; i < grammar.size(); ++i)
    {
        if (grammar[i].second.length() == 1 && islower(grammar[i].second[0]))
        {
            temp_singles.push_back(make_pair(grammar[i].first, grammar[i].second));
        }
    }

    // For some reason, certain cases lead to missed final unit production removal. Hence another loop.
    for(unsigned int i = 0; i < grammar.size(); i++)
    {
        if (grammar[i].second.length() == 1)
        {
            if (isupper(grammar[i].second[0]))
            {
                grammar.erase(grammar.begin() + i);
            }
        }
    }
    single_terminals = temp_singles;
}

// Function to remove useless or empty productions.
void pushdown::useless_removal()
{
    vector<char> keys;       // hold the reachable variables
    vector<pair<char, string>> ret, temp; // will override the grammar at the end
    vector<char>::iterator it;

    keys.push_back(grammar[0].first);
    for(unsigned int i = 0; i < grammar.size(); i++){
        for(string::size_type k = 0; k < grammar[i].second.size(); k++)
        { 
            if(isupper(grammar[i].second[k]))
            {
                for(unsigned int j = 0; j < grammar.size(); j++)
                {
                    if(grammar[i].second[k] == grammar[j].first)
                    {   
                        // Check if the key is already present in the container.
                        it = find(keys.begin(), keys.end(), grammar[i].second[k]);
                        if (it == keys.end())
                        {
                             keys.push_back(grammar[i].second[k]);   // push the reachable keys
                        }
                    }
                }
            }
        }
    }

    for(unsigned int i = 0; i < keys.size(); i++){
        for(unsigned int j = 0; j < grammar.size(); j++){
            if(keys[i] == grammar[j].first){
                ret.push_back(grammar[j]);  // make a vector with only the reachable keys
            }
        }
    }
    
    // Removes any unreachable productions in container with single terminals.
    for(unsigned int i = 0; i < keys.size(); i++)
    {
        for(unsigned int j = 0; j < single_terminals.size(); j++)
        {
            if (keys[i] == single_terminals[j].first)
            {
                temp.push_back(single_terminals[j]);
            }
        }
    }
    single_terminals = temp;
    grammar = ret;
}

// Function to sort grammar.
void pushdown::sort_grammar(){
    vector<pair<char,string>> temp;
    for(unsigned int i = 0; i < grammar.size(); i++){
        if(grammar[i].first == 'S'){
            temp.push_back(grammar[i]);
            grammar.erase(grammar.begin() + i);
            i--;
        }
    }

    sort(grammar.begin(), grammar.end());

    for(unsigned int i = 0; i < temp.size(); i++)
        grammar.insert(grammar.begin(), temp[i]);
    
}

// Function to convert a grammar to Greibach Normal Form.
void pushdown::convert_GNF()
{
    //new_var and new_prod are used to help generate Z(n) productions, temp is used for char->string conversion
    string new_var, new_prod, temp;
    //counter keeps track of n in the Z(n) productions
    int counter = 0;
    //separate vector temp_grammar is used to edits are separate from original grammar
    vector<pair<char,string>> temp_grammar = grammar;
    //production will be used a temp to help compare substitutions for GNF conversion
    pair<string,string> production;

    //vector to store productions that start with non terminals to then remove later
    vector<pair<char,string>> nonterminals;
    vector<pair<char,string>> nonterminal_subs;
    int static_tempsize = temp_grammar.size();

    //convert single_terminals into GNF_singles since GNF_singles will contain Z(n) productions
    for(auto pair: single_terminals) {
        temp = pair.first;
        GNF_singles.push_back(make_pair(temp, pair.second));
    }
        

    //begin loop to convert
    for (unsigned int i=0; i < temp_grammar.size(); i++) {
        
        //if production starts with a terminal and has more afterwards, search through it
        if( islower(temp_grammar[i].second[0]) && temp_grammar[i].second.size()>1 ) {
            for(unsigned int j=1; j<temp_grammar[i].second.size(); j++) {
                /*  any terminals after the first will be exchanged with non-terminals
                *   seen in the single_terminal productions, if not they will be created and pushed into GNF_singles
                */
                if( islower(temp_grammar[i].second[j]) && !isdigit(temp_grammar[i].second[j])) {
                    //searches through GNF_singles each time to find a matching single production, then sets a temporary pair
                    for(unsigned int k = 0; k<GNF_singles.size(); k++) {
                        if(temp_grammar[i].second[j] == GNF_singles[k].second[0]) {
                            production = GNF_singles[k];
                        }
                    }
                    
                    //temporary pair is then compared to the production each symbol at a time
                    if( production.second[0] == temp_grammar[i].second[j] ) {
                        temp_grammar[i].second.replace(j,1,production.first);
                    }
                    else if( isupper(temp_grammar[i].second[j]) || temp_grammar[i].second[j] == 'Z' || isdigit(temp_grammar[i].second[j]) ) {
                        continue;
                    }
                    else {
                        new_var = "Z" + to_string(counter);
                        new_prod = temp_grammar[i].second[j];
                        GNF_singles.push_back(make_pair(new_var,new_prod));
                        temp_grammar[i].second.replace(j,1,new_var);
                        counter++;
                    }
                }
            }
        }
        //in the case of productions that begin with a nonterminal they will be replaced
        else {
            for ( int j = 0; j < static_tempsize; j++)
            {   
                /*  each case identified will be pushed to nonterminals and the new productions without the beginning
                *   terminal will be replaced and pushed back into another vector, nonterminal_subs. to be handled afterwards
                */
                string s = temp_grammar[i].second;
                if(temp_grammar[i].second[0] == temp_grammar[j].first) {
                    if(islower(temp_grammar[j].second[0])) {
                        nonterminals.push_back(temp_grammar[i]);
                        s.replace(0,1,temp_grammar[j].second);
                    
                        nonterminal_subs.push_back(make_pair(temp_grammar[i].first,s));
                    }
                }

            }
        }
    }
    
    //push nonterminal subs into temp_grammar
    for(auto pair: nonterminal_subs) {
        temp_grammar.push_back(make_pair(pair.first, pair.second));
    }
    //remove leftover productions that begin with a nonterminal
    for(unsigned int k = 0; k<temp_grammar.size(); k++) {
        for(auto duplicate: nonterminals) {
            if(temp_grammar[k].first == duplicate.first && temp_grammar[k].second == duplicate.second)
                temp_grammar.erase(temp_grammar.begin()+k);
        }
    }

    //now that conversion is finished, temp_grammar is now converted to a <string,string> form to accept Z(n) productions in private var GNF
    for(auto pair: temp_grammar) {
        temp = pair.first;
        GNF.push_back(make_pair(temp, pair.second));
    }
    //after conversion the Z(n) productions GNF_singles are added to the main GNF grammar
    for(auto pair: GNF_singles) {
        if(pair.first[0] == 'Z')
            GNF.push_back(make_pair(pair.first,pair.second));
    }
}

// Function to convert a CFG to a Pushdown Automata.
void pushdown::convert_PDA()
{
    PDA temp;
    //push intial transition
    temp.state = '1';
    temp.read = "?";
    temp.pop = "Z";
    temp.push = "SZ";
    automaton.push_back(temp);
    
    for (auto it = GNF.begin(); it != GNF.end(); ++it)
    {
        temp.state = '2';
        temp.read = "?";
        temp.pop = it->first;
        temp.push = it->second;
        automaton.push_back(temp);
    }
    
    for (auto it = GNF_singles.begin(); it != GNF_singles.end(); ++it)
    {
        temp.state = '2';
        temp.read = it->second;
        temp.pop = it->second;
        temp.push = "?";
        automaton.push_back(temp);
    }

    //push final transition
    temp.state = '3';
    temp.read = "?";
    temp.pop = "Z";
    temp.push = "?";
    automaton.push_back(temp);

    // Checks for duplicate states, and removes them.
    for (unsigned int i = 0; i < automaton.size(); ++i)
    {
        for (unsigned int j = i + 1; j < automaton.size(); ++j)
        {
            if (automaton[i] == automaton[j])
            {
                automaton.erase(automaton.begin() + j);
            }
        }
    }
}
// Displays the transition table for a PDA.
void pushdown::display_table()
{ 
    // first state
    cout << "*    *    *    *    *    *    *    *" << endl;
    cout << endl;
    cout << "*                                  *" << endl;
    cout << setw(12);
    cout << "(q" << automaton[0].state << ", " <<  automaton[0].read << 
             "," << automaton[0].pop << ", " << automaton[0].push << ")" <<  endl;
    cout << "*                                  *" << endl;
    cout << endl;
    cout << "*    *    *    *    *    *    *    *" << endl;
    // transition line to middle state
    cout << "                 |                   " << endl;
    cout << "                 |                   " << endl;
    cout << "                 V                   " << endl;


    // middle state with majority of content
    cout << "*    *    *    *    *    *    *    *" << endl;
    cout << endl;
    cout << "*                                  *" << endl;
    
    for (unsigned int i = 1; i < automaton.size()-1; i++)
    {   
        string output;
        cout << setw(12);
        cout << "(q" <<  automaton[i].state << ", " <<  automaton[i].read << 
             ", " << automaton[i].pop << ", " << automaton[i].push << ")" << endl;
        cout << "*                                  *" << endl;

    }
    cout << endl;
    cout << "*    *    *    *    *    *    *    *" << endl; 
    cout << "                 |                   " << endl;
    cout << "                 |                   " << endl; 
    cout << "                 V                   " << endl;

    cout << "*    *    *    *    *    *    *    *" << endl; 
    cout << endl;
    cout << "*                                  *" ;
    cout << endl;
    cout << setw(12);
    cout << "(q" << automaton[automaton.size()-1].state << ", "
         << automaton[automaton.size()-1].read << ", " 
         << automaton[automaton.size()-1].pop << ", "
         << automaton[automaton.size()-1].push << ")" << endl;
    cout << "*                                  *" << endl;
    cout << endl;
    cout << "*    *    *    *    *    *    *    *" << endl; 
}


// Function to display a grammar's production rules.
void pushdown::display_grammar(bool GNF_or_grammar)
{   
    if (GNF_or_grammar)
    {
        cout << "Grammar: " << endl;
        for(auto it = grammar.begin(); it != grammar.end(); it++){
            if (it->second == "\0")
            {
                throw("Empty string!");
            }
         cout << it->first << " -> " << it->second << endl;
        }
    }
    else 
    {
        cout << "GNF: " << endl;
        for(auto it = GNF.begin(); it != GNF.end(); it++){
         cout << it->first << " -> " << it->second << endl;
        }
    }
    cout << endl;
}