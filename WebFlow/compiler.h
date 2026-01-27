#pragma once
#include <vector>
#include <string>
#include <iostream>

using std::cout;
using std::cerr;
using std::endl;
using std::abort;
using std::vector;

typedef std::string str;

enum TokenType {
    Tag,
    Colon,
    Semicolon,
    Props,
    Dataset,
    Classes,
    Ids,
    Content,
    Style,
    Block,
    Eof
};

struct Token {
    TokenType type;
    std::string value;
};

struct KeyVal {
    str key, value;

    KeyVal(str k, str v) {
        key = k;
        value = v;
    }
};

struct Element {
    str                 tagName;
    vector<KeyVal>      props, datasets, style;
    vector<str>         ids, classes;
    str                 content;
    bool                closed = false;
    vector<Element>     children;
};

Token token(TokenType type, char value);
Token token(TokenType type, const str& value);

char shift(vector<char>& src);
Token shift(vector<Token>& src);

bool contain(const vector<TokenType>& arr, TokenType value);
bool isInt(char c);
bool isSkippable(char c);

vector<Token> tokenize(str& sourceCode);

class Parser {
public:
    Parser(str& code);
    vector<Element> parse();

private:
    vector<Token> src, tokens;
    vector<Element> tree;

    Token current();
    Token eat(TokenType type);
    Token eat();

    Element parseElement();

    str trim(const str& s);
    vector<KeyVal> parseKeyValue(str value);
    vector<str> parseList(str value);
    str unquote(str s);
    str parseContent(str value);
    void parseSet(Element* elem);
};

str style(const vector<KeyVal>& t);
str dataset(const vector<KeyVal>& t);
str classes(const vector<str>& t);
str ids(const vector<str>& t);
str props(const vector<KeyVal>& t);
str attr(const Element& e);

str convertElementToHTML(const Element& e, size_t depth = 0);
str convertToHTML(const str& src);
