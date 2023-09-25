#ifndef PARSER_H
#define PARSER_H

#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <fstream>
#include <map>
#include <chrono>
#include <cctype>
#include <vector>
#include <initializer_list>
#include "SequentialFile.h"
#include "AVL.h"
#include "ExtendibleHash.h"


using namespace std;

const int FB = 100000;
const int D = 10;

template <class STR>
void readCSVFile(const string& filename, STR& structure) {
    ifstream file("/Users/diegopachecoferrel/Desktop/" + filename);

    if (!file.is_open()) {
        cerr << "Error: No se pudo abrir el archivo " << filename << endl;
    }

    string line;
    getline(file, line); // Leer la primera línea (encabezados) y descartarla

    while (getline(file, line)) {
        int i=0;
        getline(file, line);
        istringstream ss(line);
        string id_str, district, year_str, month_str, day_str, vol_str, street;

        if (getline(ss, id_str, ',') && getline(ss, district, ',') &&
            getline(ss, year_str, ',') && getline(ss, month_str, ',') &&
            getline(ss, day_str, ',') && getline(ss, vol_str, ',') &&
            getline(ss, street, ',')) {
            Record record;
            record.id = stoi(id_str);
            strncpy(record.district, district.c_str(), sizeof(record.district));
            record.year = i;//stoi(year_str);
            record.month = i;//stoi(month_str);
            record.day = i;//stoi(day_str);
            record.vol = i;//stoi(vol_str);
            strncpy(record.street, street.c_str(), sizeof(record.street));

            structure.add(record);
            i++;

        } else {
            cerr << "Error al analizar la línea: " << line << endl;
        }
    }

    file.close();
}



class Token {
public:
    enum Type { ID, ERR, END, NUM, EOL, SELECT, FROM, WHERE, INSERT, DELETE, INTO, SET, VALUES, AND, OR, NOT, GT, LT, GE, LE, EQ, NEQ, LPAREN, RPAREN, COMILLAS, USING, INDEX, BETWEEN, CAMPO, CREATE, TABLE, FILE, COMA, BOOL };
    static const char* token_names[34];
    Type type;
    string lexema;
    Token(Type);
    Token(Type, char c);
    Token(Type, const string source);
};

const char* Token::token_names[34] = { "ID", "ERR", "END", "NUM", "EOL", "SELECT", "FROM", "WHERE", "INSERT", "DELETE", "INTO", "SET", "VALUES", "AND", "OR", "NOT", "GT", "LT", "GE", "LE", "EQ", "NEQ", "LPAREN", "RPAREN", "COMILLAS", "USING", "INDEX", "BETWEEN", "CAMPO", "CREATE", "TABLE", "FILE", "COMA", "BOOL" };

Token::Token(Type type):type(type){
    lexema = "";
}

Token::Token(Type type, const string source):type(type) {
    lexema = source;
}

std::ostream& operator << ( std::ostream& outs, const Token & tok )
{
    if (tok.lexema.empty())
        return outs << Token::token_names[tok.type];
    else
        return outs << Token::token_names[tok.type] << "(" << tok.lexema << ")";
}

std::ostream& operator << ( std::ostream& outs, const Token* tok ) {
    return outs << *tok;
}

class Scanner {
public:
    Scanner(const char* in_s);
    Token* nextToken();
    ~Scanner();
private:
    string input;
    int first, current;
    int state;
    char nextChar();
    void rollBack();
    void startLexema();
    void incrStartLexema();
    string getLexema();
    map<string, Token::Type> keywords;
};

Scanner::Scanner(const char* s):input(s),first(0),current(0) {
    keywords["SELECT"] = Token::SELECT;
    keywords["FROM"] = Token::FROM;
    keywords["WHERE"] = Token::WHERE;
    keywords["INSERT"] = Token::INSERT;
    keywords["DELETE"] = Token::DELETE;
    keywords["INTO"] = Token::INTO;
    keywords["SET"] = Token::SET;
    keywords["VALUES"] = Token::VALUES;
    keywords["AND"] = Token::AND;
    keywords["OR"] = Token::OR;
    keywords["NOT"] = Token::NOT;
    keywords["USING"] = Token::USING;
    keywords["INDEX"] = Token::INDEX;
    keywords["BETWEEN"] = Token::BETWEEN;
    keywords["CAMPO"] = Token::CAMPO;
    keywords["CREATE"] = Token::CREATE;
    keywords["TABLE"] = Token::TABLE;
    keywords["FILE"] = Token::FILE;
    keywords["select"] = Token::SELECT;
    keywords["from"] = Token::FROM;
    keywords["where"] = Token::WHERE;
    keywords["insert"] = Token::INSERT;
    keywords["delete"] = Token::DELETE;
    keywords["into"] = Token::INTO;
    keywords["set"] = Token::SET;
    keywords["values"] = Token::VALUES;
    keywords["and"] = Token::AND;
    keywords["or"] = Token::OR;
    keywords["not"] = Token::NOT;
    keywords["using"] = Token::USING;
    keywords["index"] = Token::INDEX;
    keywords["between"] = Token::BETWEEN;
    keywords["campo"] = Token::CAMPO;
    keywords["create"] = Token::CREATE;
    keywords["table"] = Token::TABLE;
    keywords["file"] = Token::FILE;
}

char Scanner::nextChar() {
    int c = input[current];
    if (c != '\0')
        current++;
    return c;
}

void Scanner::rollBack() { // retract
    if (input[current] != '\0')
        current--;
}

void Scanner::startLexema() {
    first = current-1;
    return;
}

void Scanner::incrStartLexema() {
    first++;
}

Scanner::~Scanner() { }

string Scanner::getLexema() {
    string res = input.substr(first,current-first);
    size_t n = res.size();

    if (res.front() == '"' and res.back() == '"') {
        res = res.substr(1, res.length() - 2);
    }

    if (res[n-1] == ')'){
        res = res.substr(0, n-1);
        current--;
    }

    if (res[n-1] == '\n'){
        res = res.substr(0, n-1);
        current--;
    }

    return res;

}

Token* Scanner::nextToken() {
    Token* token;
    char c;
    c = nextChar();

    while (c == ' ' || c == '\t' || c == '\n')
        c = nextChar();

    startLexema();

    if(c == '*')
        return new Token(Token::CAMPO, getLexema());
    if(c == ',')
        return new Token(Token::COMA);
    if(c == '"'){
        c = nextChar();
        while (c != '"') {
            c = nextChar();
        }
        return new Token(Token::ID, getLexema());
    }
    if(c == '(')
        return new Token(Token::LPAREN);
    if(c == ')')
        return new Token(Token::RPAREN);
    if(c == '<'){
        c = nextChar();
        if(c == '=')
            return new Token(Token::LE);
        return new Token(Token::LT);
    }
    if(c == '>'){
        c = nextChar();
        if(c == '=')
            return new Token(Token::GE);
        return new Token(Token::GT);
    }
    if(c == '='){
        c = nextChar();
        if(c == '>')
            return new Token(Token::GE);
        if(c == '<')
            return new Token(Token::LE);
        return new Token(Token::EQ);
    }
    if (isalpha(c)) {
        c = nextChar();
        while (isdigit(c) || isalpha(c)) {
            c = nextChar();
        }
        rollBack();
        if(getLexema() == "true" or getLexema() == "false")
            return new Token(Token::BOOL, getLexema());
        else if(keywords[getLexema()] == Token::ID)
            return new Token(Token::CAMPO, getLexema());
        else
            return new Token(keywords[getLexema()]);

    } else if (isdigit(c)) {
        c = nextChar();
        while (isdigit(c))
            c = nextChar();
        rollBack();

        return new Token(Token::NUM, getLexema());

    } else if (c ==  '\0') {
        return new Token(Token::END);

    }else {
        return new Token(Token::ERR, getLexema());
    }
}



///-------------------------PARSER----------------------------
class Parser {
    Scanner scanner;
    Token* currentToken;
    Token* previousToken;

public:
    Parser(const string& input) : scanner(input.c_str()), currentToken(nullptr), previousToken(nullptr) {}

    void parse(vector<Record>& records) {
        currentToken = scanner.nextToken();
        if (currentToken->type == Token::SELECT) {
            records = parseSelectStatement();
            return;
        }
        else if (currentToken->type == Token::CREATE) {
            parseCreateStatement();
            return;
        }
        else if (currentToken->type == Token::INSERT) {
            parseInsertStatement();
            return;
        }
        else if (currentToken->type == Token::DELETE) {
            parseDeleteStatement();
            return;
        }
    }

    ~Parser(){
        if (previousToken) {
            delete previousToken;
            previousToken = nullptr;
        }
        if (currentToken) {
            delete currentToken;
            currentToken = nullptr;
        }
    }

private:
    void advance() {
        previousToken = currentToken;
        currentToken = scanner.nextToken();
    }

    void expect(Token::Type expectedType) {
        if (currentToken && currentToken->type == expectedType) {
            advance();
        } else {
            cout << "Error: Unexpected token" << endl;
            exit(1);
        }
    }

    void expect(initializer_list<Token::Type> expectedTypes) {
        if (currentToken) {
            for (Token::Type type : expectedTypes) {
                if (currentToken->type == type) {
                    advance();
                    return;
                }
            }
        }

        cout << "Error: Unexpected token" << endl;
        exit(1);
    }

    vector<Record> parseSelectStatement(){
        vector<Record> found;

        vector<string> campos;
        advance();
        expect(Token::CAMPO);
        campos.push_back(previousToken->lexema);
        string tabla;

        while(currentToken->type == Token::COMA) {
            advance();  // Avanzar sobre la coma
            expect(Token::CAMPO);
            campos.push_back(previousToken->lexema);
        }

        expect(Token::FROM);
        expect(Token::CAMPO);

        tabla = previousToken->lexema;

        if(currentToken->type == Token::USING){
            advance();
            expect(Token::CAMPO);
            if(currentToken->type == Token::END){
                if(previousToken->lexema == "SEQUENTIAL" or previousToken->lexema == "sequential"){
                    SequentialFile f(tabla);
                    found = f.inorder();
                }else if(previousToken->lexema == "AVL" or previousToken->lexema == "avl"){
                    AVLFile f(tabla);
                    found = f.inorder();
                }else if(previousToken->lexema == "HASH" or previousToken->lexema == "hash"){
                    DynamicHash f(tabla + ".bin", tabla + "index.bin", D, FB);
                    found = f.inorder();
                }
            }
            return found;
        }

        // Consultas condicionadas
        if(currentToken->type == Token::WHERE)
            advance();

        string campo;

        expect(Token::CAMPO);
        campo = previousToken->lexema;

        expect({Token::GT, Token::GE, Token::LT, Token::LE, Token::EQ, Token::BETWEEN});
        if(previousToken->type == Token::EQ){
            string value;
            expect({Token::NUM, Token::ID, Token::BOOL});
            value = previousToken->lexema;
            expect(Token::USING);
            expect(Token::CAMPO);
            if(currentToken->type == Token::END){
                if(previousToken->lexema == "SEQUENTIAL" or previousToken->lexema == "sequential"){
                    SequentialFile f(tabla);
                    found = f.search(stoi(value));
                }else if(previousToken->lexema == "AVL" or previousToken->lexema == "avl"){
                    AVLFile f(tabla);
                    found = f.search(stoi(value));
                }else if(previousToken->lexema == "HASH" or previousToken->lexema == "hash"){
                    DynamicHash f(tabla + ".bin", tabla + "index.bin", D, FB);
                   found.push_back(f.search(stoi(value)));
                }
            }
            return found;
        }else if(previousToken->type == Token::BETWEEN){
            string begin, end;
            expect(Token::LPAREN);
            expect({Token::NUM, Token::ID});
            begin = previousToken->lexema;
            expect(Token::COMA);
            expect({Token::NUM, Token::ID});
            end = previousToken->lexema;
            expect(Token::RPAREN);
            expect(Token::USING);
            expect(Token::CAMPO);
            if(previousToken->lexema == "AVL" or previousToken->lexema == "avl"){
                AVLFile f(tabla);
                found = f.rangeSearch(stoi(begin), stoi(end));
            }else if(previousToken->lexema == "SEQUENTIAL" or previousToken->lexema == "sequential"){
                SequentialFile f(tabla);
                found = f.rangeSearch(stoi(begin), stoi(end));
            }

            return found;
        }

    };

    void parseCreateStatement(){
        string tabla, filename, index, key;
        advance();
        expect(Token::TABLE);
        expect(Token::CAMPO);
        tabla = previousToken->lexema;
        expect(Token::FROM);
        expect(Token::FILE);
        expect(Token::ID);
        filename = previousToken->lexema;
        expect(Token::USING);
        expect(Token::INDEX);
        expect(Token::CAMPO);
        index = previousToken->lexema;
        expect(Token::LPAREN);
        expect(Token::ID);
        key = previousToken->lexema;
        expect(Token::RPAREN);
        if(currentToken->type == Token::END){
            if(index == "AVL" or index == "avl"){
                AVLFile indice(tabla);
                readCSVFile<AVLFile>(filename, indice);
                cout << "Tabla creada con nombre " << tabla << endl;
            }else if(index == "SEQUENTIAL" or index == "sequential"){
                SequentialFile indice(tabla);
                readCSVFile<SequentialFile>(filename, indice);
                cout << "Tabla creada con nombre " << tabla << endl;
            }else if(index== "HASH" or index == "hash"){
                DynamicHash indice(tabla + ".bin", tabla + "index.bin", D, FB);
                readCSVFile<DynamicHash>(filename, indice);
            }else{
                cout << "Indice no existente";
            }
        }
    };

    void parseInsertStatement(){
        Record record;
        string tabla;

        advance();
        expect(Token::INTO);
        expect(Token::CAMPO);
        tabla = previousToken->lexema;
        expect(Token::VALUES);
        expect(Token::LPAREN);
        expect(Token::NUM);
        record.id = stoi(previousToken->lexema);
        expect(Token::COMA);
        expect(Token::ID);
        strncpy(record.district, previousToken->lexema.c_str(), sizeof(record.district));
        expect(Token::COMA);
        expect(Token::NUM);
        record.year = stoi(previousToken->lexema);
        expect(Token::COMA);
        expect(Token::NUM);
        record.month = stoi(previousToken->lexema);
        expect(Token::COMA);
        expect(Token::NUM);
        record.day = stoi(previousToken->lexema);
        expect(Token::COMA);
        expect(Token::NUM);
        record.vol = stoi(previousToken->lexema);
        expect(Token::COMA);
        expect(Token::ID);
        strncpy(record.street, previousToken->lexema.c_str(), sizeof(record.street));
        expect(Token::RPAREN);
        expect(Token::USING);
        expect(Token::CAMPO);
        if(currentToken->type == Token::END){
            if(previousToken->lexema == "AVL" or previousToken->lexema == "avl"){
                AVLFile f(tabla);
                f.add(record);
            }else if(previousToken->lexema == "SEQUENTIAL" or previousToken->lexema == "sequential"){
                SequentialFile f(tabla);
                f.add(record);
            }else if(previousToken->lexema == "HASH" or previousToken->lexema == "hash"){
                DynamicHash f(tabla + ".bin", tabla + "index.bin", D, FB);
                f.add(record);
            }
        }
    };

    void parseDeleteStatement(){
        string campo, tabla;
        advance();
        expect(Token::FROM);
        expect(Token::CAMPO);
        tabla = previousToken->lexema;
        expect(Token::WHERE);

        expect(Token::CAMPO);
        campo = previousToken->lexema;

        expect({Token::GT, Token::GE, Token::LT, Token::LE, Token::EQ, Token::BETWEEN});
        if(previousToken->type == Token::EQ){
            string value;
            expect({Token::NUM, Token::ID, Token::BOOL});
            value = previousToken->lexema;
            expect(Token::USING);
            expect(Token::CAMPO);
            if(currentToken->type == Token::END){
                if(previousToken->lexema == "SEQUENTIAL" or previousToken->lexema == "sequential"){
                    SequentialFile f(tabla);
                    f.remove(stoi(value));
                }else if(previousToken->lexema == "AVL" or previousToken->lexema == "avl"){
                    AVLFile f(tabla);
                    f.remove(stoi(value));
                }else if(previousToken->lexema == "HASH" or previousToken->lexema == "hash"){
                    DynamicHash f(tabla + ".bin", tabla + "index.bin", D, FB);
                    f.remove(stoi(value));
                }
            }
        }
    };

};

#endif // PARSER_H
