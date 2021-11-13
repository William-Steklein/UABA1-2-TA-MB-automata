#include "CFG.h"

int CFG::nextID = 0;

CFG::CFG() {
    ID = nextID++;

    //epsilon
    symbols[""] = new Symbol;
    symbols[""]->name = "";

    //hardcoded for programming assignment 1
//    addSymbol("BINDIGIT", true);
//    addSymbol("S", true);
//    addSymbol("0", false);
//    addSymbol("1", false);
//    addSymbol("a", false);
//    addSymbol("b", false);
//
//    addProduction("BINDIGIT", "0");
//    addProduction("BINDIGIT", "1");
//    addProduction("S", "");
//    addProduction("S", {"a", "S", "b", "BINDIGIT"});
//
//    setStartVariable("S");
}

CFG::CFG(const std::string& json_filename)
{
    ID = nextID++;

    //epsilon
    symbols[""] = new Symbol;
    symbols[""]->name = "";

    // load the rest
    load(json_filename);
}

CFG::~CFG() {
    for (const auto& symbol : symbols)
        delete symbol.second;
}

CFG &CFG::operator=(const CFG &_cfg) {
    //todo
    return *this;
}

bool CFG::load(const std::string& filename)
{
    // file input
    std::ifstream input(filename);

    json cfg_json;

    if (!input.good())
    {
        std::cerr << "Error: couldn't open \"" << filename << "\"" << std::endl;
        return cfg_json;
    }

    input >> cfg_json;
    input.close();

    if (!(cfg_json.contains("Variables") && cfg_json.contains("Terminals")
          && cfg_json.contains("Productions") && cfg_json.contains("Start")))
    {
        *getErrorOutputStream() << "Error: \"" << filename << "\" has an invalid format" << std::endl;
        return false;
    }

    for (const auto& symbol : cfg_json["Variables"])
    {
        addSymbol(symbol.get<std::string>(), true);
    }

    for (const auto& symbol : cfg_json["Terminals"])
    {
        addSymbol(symbol.get<std::string>(), false);
    }

    for (const auto& production : cfg_json["Productions"])
    {
        std::string head;
        std::vector<std::string> body;

        if (production.contains("head")) {
            head = production["head"];
        }
        if (production.contains("body")) {
            if (production["body"].empty()) body.emplace_back("");
            else {
                for (const auto& other_symbol : production["body"]) {
                    body.push_back(other_symbol.get<std::string>());
                }
            }
        }

        addProduction(head, body);
    }

    if (cfg_json.contains("Start")) {
        start_symbol = getSymbol(cfg_json["Start"].get<std::string>());
    }

    isLegal();

    return true;
}

json CFG::save() const
{
    //todo
    return json();
}

bool CFG::isLegal() const
{
    //todo
	return true;
}

bool CFG::Empty() const {
    if (symbols.empty() || !start_symbol) return false;
    return true;
}

void CFG::print(std::ostream &output_stream) const {
    if (!Empty()) {
        output_stream << "Empty CFG!" << std::endl;
        return;
    }

    // variables
    output_stream << "V = {";
    bool is_first = true;
    for (const auto& symbol : symbols) {
        if (symbol.second->is_var) {
            if (!is_first) output_stream << ", ";
            else is_first = false;
            output_stream << symbol.first;
        }
    }
    output_stream << "}" << std::endl;

    // terminals
    output_stream << "T = {";
    is_first = true;
    for (const auto& symbol : symbols) {
        if (symbol.first.empty()) continue;
        if (!symbol.second->is_var) {
            if (!is_first) output_stream << ", ";
            else is_first = false;
            output_stream << symbol.first;
        }
    }
    output_stream << "}" << std::endl;

    // productions
    output_stream << "P = {\n";
    for (const auto& symbol : symbols) {
        if (symbol.second->is_var) {
            for (const std::vector<Symbol*>& symbol_string : symbol.second->productions) {
                output_stream << "    " << symbol.first << " -> " << "`";
                is_first = true;
                for (Symbol* other_symbol : symbol_string) {
                    if (!is_first) output_stream << " ";
                    else is_first = false;
                    output_stream << other_symbol->name;
                }
                output_stream << "`" << std::endl;
            }
        }
    }
    output_stream << "}" << std::endl;

    // start variable
    output_stream << "S = " << start_symbol->name << std::endl;
}

void CFG::addSymbol(const std::string &symbol_name, bool is_var) {
    Symbol* new_symbol = new Symbol;
    new_symbol->name = symbol_name;
    new_symbol->is_var = is_var;
    symbols[symbol_name] = new_symbol;
}

bool CFG::removeSymbol(const std::string &symbol_name) {
    Symbol* symbol_to_delete = getSymbol(symbol_name);
    if (!symbol_to_delete) return false;

    if (symbol_to_delete == start_symbol) start_symbol = nullptr;

    // deletes the symbol pointer from the productions
    for (const auto& symbol : symbols) {
        std::set<std::vector<Symbol*>> new_productions;
        for (const std::vector<Symbol*>& symbol_string : symbol.second->productions) {
            std::vector<Symbol*> new_symbol_string = symbol_string;
            new_symbol_string.erase(std::remove(new_symbol_string.begin(), new_symbol_string.end(), symbol_to_delete), new_symbol_string.end());
            new_productions.insert(new_symbol_string);
        }
        symbol.second->productions = new_productions;
    }
    return false;
}

std::string CFG::getStartSymbol() const {
    return start_symbol->name;
}

void CFG::setStartVariable(const std::string &start_variable_name) {
    start_symbol = getSymbol(start_variable_name);
}

bool CFG::isSymbolVariable(const std::string &symbol_name) const {
    return getSymbol(symbol_name)->is_var;
}

bool CFG::symbolExists(const std::string &symbol_name) const {
    return getSymbol(symbol_name);
}

bool CFG::addProduction(const std::string &variable_name, const std::string &symbol_name) {
    Symbol* var = getSymbol(variable_name);
    Symbol* other_symbol = getSymbol(symbol_name);
    if (!var || !other_symbol) return false;

    var->productions.insert({other_symbol});

    return true;
}

bool CFG::addProduction(const std::string &variable_name, const std::vector<std::string> &symbol_string) {
    Symbol* var = getSymbol(variable_name);
    if (!var) return false;

    std::vector<Symbol*> other_symbols;
    for (const std::string& symbol_name : symbol_string) {
        Symbol* other_symbol = getSymbol(symbol_name);
        if (!other_symbol) return false;
        other_symbols.push_back(other_symbol);
    }

    var->productions.insert(other_symbols);

    return true;
}

bool CFG::removeProduction(const std::string &variable_name, const std::string &symbol_name) {
    //todo
    return false;
}

bool CFG::removeProduction(const std::string &variable_name, const std::vector<std::string> &symbol_string) {
    //todo
    return false;
}

std::vector<std::string> CFG::derive(const std::vector<std::string> &symbol_string) const {
    //todo
    return std::vector<std::string>();
}

bool CFG::accepts(const std::string &str) const {
    std::map<std::vector<Symbol*>, std::set<Symbol*>> reverse_productions = getReverseProductions();

//    std::vector<std::map<std::string, std::set<Symbol*>>> table;
    std::vector<std::vector<std::pair<std::string, std::set<Symbol*>>>> table;

    int substr_count = static_cast<int>(str.size());
    for (int substr_size = 1; substr_size <= str.size(); substr_size++) {
        std::vector<std::pair<std::string, std::set<Symbol*>>> current_iteration;

        //todo: put in helper function

        // get the substrings
        std::vector<std::string> substrings;
        std::string current_full_str = str;

        for (int i = 0; i < substr_count; i++) {
            int current_substr_size = 0;
            std::string current_string;
            for (char c : current_full_str) {
                current_substr_size++;

                current_string.push_back(c);

                if (current_substr_size == substr_size) {
                    substrings.push_back(current_string);
                    break;
                }
            }
            current_full_str.erase(0, 1);
        }

        // ****************************************************

        // get the reverse productions (set) and put it in the table

        for (const std::string& substr : substrings) {
            if (substr_size == 1) {
                current_iteration.emplace_back(substr, reverse_productions[{getSymbol(substr)}]);
//                current_iteration[substr] = reverse_productions[{getSymbol(substr)}];
            }
            else {
                std::set<Symbol*> answer;
                // get all possible pair substring combination of the substring

                for (int i = 1; i < static_cast<int>(substr.size()); i++) {
                    std::string comb1 = substr.substr(0, i);
                    std::string comb2 = substr.substr(i, substr.size() - i);
                    

                    std::vector<std::pair<std::string, std::set<Symbol*>>> comb1_prod1 = table[comb1.size() - 1];
                    std::set<Symbol*> comb1_prod;
                    for (const std::pair<std::string, std::set<Symbol*>>& comb : comb1_prod1) {
                        if (comb.first == comb1) {
                            comb1_prod = comb.second;
                        }
                    }

                    std::vector<std::pair<std::string, std::set<Symbol*>>> comb2_prod1 = table[comb2.size() - 1];
                    std::set<Symbol*> comb2_prod;
                    for (const std::pair<std::string, std::set<Symbol*>>& comb : comb2_prod1) {
                        if (comb.first == comb2) {
                            comb2_prod = comb.second;
                        }
                    }

                    for (auto symbol1 : comb1_prod) {
                        for (auto symbol2 : comb2_prod) {
                            auto reverse_production = reverse_productions.find({symbol1, symbol2});
                            if (reverse_production != reverse_productions.end()) {
                                answer.insert(reverse_production->second.begin(), reverse_production->second.end());
                            }
                        }
                    }
                }

//                current_iteration[substr] = answer;
                current_iteration.emplace_back(substr, answer);
            }
        }

        table.push_back(current_iteration);
    }

    printCYKTable(table);

    std::cout << std::boolalpha;

    for (auto symbol : table.back()[0].second) {
        if (symbol == start_symbol) {
            std::cout << true << std::endl;
            return true;
        }
    }

    std::cout << false << std::endl;
    return false;
}

void CFG::clear() {
    // symbols
    for (const auto& symbol : symbols)
        delete symbol.second;

    symbols.clear();
    start_symbol = nullptr;
}

std::ostream *CFG::getErrorOutputStream() const {
    return error_output_stream;
}

void CFG::setOutputStream(std::ostream &_output_stream) {
    error_output_stream = &_output_stream;
}

int CFG::getID() const {
    return ID;
}

CFG::Symbol *CFG::getSymbol(const std::string &symbol_name, bool error_output) const {
    auto symbol = symbols.find(symbol_name);
    if (symbol != symbols.end())
        return symbol->second;

    if (error_output) {
        *getErrorOutputStream() << "Error: CFG " << getID() << ", couldn't find symbol with name \"" << symbol_name
                                << "\"" << std::endl;
    }

    return nullptr;
}

std::map<std::vector<CFG::Symbol *>, std::set<CFG::Symbol *>> CFG::getReverseProductions() const {
    std::map<std::vector<Symbol*>, std::set<Symbol*>> reverse_productions;

    for (const auto& symbol : symbols) {
        for (const auto& production : symbol.second->productions) {
            if (reverse_productions[production].empty()) {
                reverse_productions[production] = {symbol.second};
            }
            else {
                reverse_productions[production].insert(symbol.second);
            }
        }
    }

    return reverse_productions;
}

void CFG::printCYKTable(const std::vector<std::vector<std::pair<std::string, std::set<Symbol *>>>>& table) {
    std::vector<int> column_width(table[0].size(), 0);
    int column_count = 0;
    for (int i = static_cast<int>(table.size()) - 1; i >= 0; i--) {
        column_count++;

        for (int j = 0; j < column_count; j++) {
            if (static_cast<int>(table[i][j].second.size()) > column_width[j]) {
                column_width[j] = static_cast<int>(table[i][j].second.size());
            }
        }
    }

    for (int i = static_cast<int>(table.size()) - 1; i >= 0; i--) {
        std::cout << "|";
        for(int j = 0; j < static_cast<int>(table[i].size()); j++) {
            std::cout << " {";
            for (auto symbol = (table[i][j].second).begin(); symbol != (table[i][j].second).end(); symbol++) {
                if ((symbol != table[i][j].second.end()) && (std::next(symbol) != table[i][j].second.end()))
                    std::cout << (*symbol)->name << ", ";
                else
                    std::cout << (*symbol)->name;
            }

            if (table[i][j].second.empty()) {
                std::cout << "}" << std::string((column_width[j] - 1) * 3, ' ');
                std::cout << "   |";
            }
            else {
                std::cout << "}" << std::string((column_width[j] - static_cast<int>(table[i][j].second.size())) * 3, ' ');
                std::cout << "  |";
            }
        }
        std::cout << std::endl;
    }
}
