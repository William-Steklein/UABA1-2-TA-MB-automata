#include "CFG.h"

int CFG::nextID = 0;

CFG::CFG() {
    ID = nextID++;

    //epsilon
    symbols[""] = new Symbol;
    symbols[""]->name = "";

    //hardcoded for programming assignment 1
    addSymbol("BINDIGIT", true);
    addSymbol("S", true);
    addSymbol("0", false);
    addSymbol("1", false);
    addSymbol("a", false);
    addSymbol("b", false);

    addProduction("BINDIGIT", "0");
    addProduction("BINDIGIT", "1");
    addProduction("S", "");
    addProduction("S", {"a", "S", "b", "BINDIGIT"});

    setStartVariable("S");
}

CFG::CFG(const std::string& json_filename)
{
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
    //todo
    return false;
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

void CFG::print(std::ostream &output_stream) const {
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

void CFG::clear() {
    // symbols
    for (const auto& symbol : symbols)
        delete symbol.second;

    symbols.clear();
    start_symbol = nullptr;
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

std::ostream *CFG::getErrorOutputStream() const {
    return error_output_stream;
}

void CFG::setOutputStream(std::ostream &_output_stream) {
    error_output_stream = &_output_stream;
}

int CFG::getID() const {
    return ID;
}

