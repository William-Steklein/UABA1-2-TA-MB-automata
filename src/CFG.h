#ifndef UABA1_TA_AUTOMATA_CFG_H
#define UABA1_TA_AUTOMATA_CFG_H

#include <iostream>
#include <string>
#include <set>
#include <algorithm>
#include "json.hpp"

using json = nlohmann::json;

class CFG
{
    struct Symbol {
        std::string name;
        std::set<std::vector<Symbol*>> productions;
        bool is_var = false;
    };

    std::map<std::string, Symbol*> symbols;
    Symbol* start_symbol = nullptr;

	bool load(const std::string& filename);
	json save() const;

    std::ostream* error_output_stream = &std::cerr;
    int ID;
    static int nextID;

public:
	CFG();
    CFG(const std::string& json_filename);
    ~CFG();
    CFG& operator=(const CFG& _cfg);

    /* prints the CFG in json format */
    void print(std::ostream& output_stream = std::cout) const;

    void addSymbol(const std::string& symbol_name, bool is_var);
    bool removeSymbol(const std::string& symbol_name);
    std::string getStartSymbol() const;
    void setStartVariable(const std::string& start_variable_name);
    bool isSymbolVariable(const std::string& symbol_name) const;
    bool symbolExists(const std::string& symbol_name) const;

    bool addProduction(const std::string& variable_name, const std::string& symbol_name);
    bool addProduction(const std::string& variable_name, const std::vector<std::string>& symbol_string);
    bool removeProduction(const std::string& variable_name, const std::string& symbol_name);
    bool removeProduction(const std::string& variable_name, const std::vector<std::string>& symbol_string);
    std::vector<std::string> derive(const std::vector<std::string>& symbol_string) const;

    void clear();
	bool isLegal() const;

    std::ostream* getErrorOutputStream() const;
    void setOutputStream(std::ostream& _output_stream);
    int getID() const;

private:
    Symbol* getSymbol(const std::string& symbol_name, bool error_output = false) const;
};


#endif
