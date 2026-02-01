/*
    @author Magizhnun
    @version 1.0.0
*/
#include <algorithm>
#include <sstream>
#include "compiler.h"

size_t charIndex = 0;
size_t tokenIndex = 0;

Token token(TokenType type, char value) {
    return Token{ type, str(1, value) };
}

Token token(TokenType type, const str& value) {
    return Token{ type, value };
}

char shift(vector<char>& src) {
    return src[charIndex++];
}

Token shift(vector<Token>& src) {
    return src[tokenIndex++];
}

bool contain(const vector<TokenType>& arr, TokenType value) {
    return std::find(arr.begin(), arr.end(), value) != arr.end();
}

bool isInt(char c) {
    return c >= '0' && c <= '9';
}

bool isSkippable(char c) {
    return c == ' ' || c == '\n' || c == '\r' || c == '\t';
}

/* tokenizer */
vector<Token> tokenize(str& sourceCode) {
    if (sourceCode.empty()) {
        cerr << "Empty source";
        abort();
    }

    vector<char> src(sourceCode.begin(), sourceCode.end());
    vector<Token> tokens;
    charIndex = tokenIndex = 0;

    while (charIndex < src.size()) {
        char c = src[charIndex];

        if (c == ':') {
            tokens.emplace_back(token(Colon, shift(src)));
        }
        else if (c == ';') {
            tokens.emplace_back(token(Semicolon, shift(src)));
        }
        else if (c == '{') {
            shift(src);
            str value;

            while (charIndex < src.size()) {
                if (src[charIndex] == '\\') {
                    if (charIndex + 1 < src.size() && src[charIndex + 1] == '{') {
                        value += '{'; charIndex += 2;
                    }
                    else if (src[charIndex + 1] == '}') {
                        value += '}'; charIndex += 2;
                    }
                    else if (src[charIndex + 1] == '\\') {
                        value += '\\'; charIndex += 2;
                    }
                    else if (src[charIndex + 1] == 'n') {
                        value += '\n'; charIndex += 2;
                    }
                    else if (src[charIndex + 1] == 't') {
                        value += '\t'; charIndex += 2;
                    }
                    else value += shift(src);
                }
                else if (src[charIndex] == '}') break;
                else value += shift(src);
            }

            if (src[charIndex] != '}') {
                cerr << "Unclosed block";
                abort();
            }

            shift(src);
            tokens.emplace_back(token(Block, value));
        }
        else if (src[charIndex] == '-') {
            if (src[charIndex + 1] == '-') {
                ++charIndex;
                while (charIndex < src.size() && src[charIndex] != '\n') {
                    charIndex++;
                }
            }
        }
        else if (
            (src[charIndex] >= 'A' && src[charIndex] <= 'Z') ||
            (src[charIndex] >= 'a' && src[charIndex] <= 'z') ||
            isInt(src[charIndex])
            ) {
            str expr;
            while (charIndex < src.size() &&
                ((src[charIndex] >= 'A' && src[charIndex] <= 'Z') ||
                    (src[charIndex] >= 'a' && src[charIndex] <= 'z') ||
                    isInt(src[charIndex]))) {
                expr += shift(src);
            }

            if (expr == "props") tokens.emplace_back(token(Props, expr));
            else if (expr == "content") tokens.emplace_back(token(Content, expr));
            else if (expr == "classes") tokens.emplace_back(token(Classes, expr));
            else if (expr == "ids") tokens.emplace_back(token(Ids, expr));
            else if (expr == "dataset") tokens.emplace_back(token(Dataset, expr));
            else if (expr == "styles") tokens.emplace_back(token(Style, expr));
            else tokens.emplace_back(token(Tag, expr));
        }
        else if (isSkippable(src[charIndex])) {
            ++charIndex;
        }
        else {
            cerr << "Unexpected char \"" << src[charIndex] << "\"";
            abort();
        }
    }

    tokens.emplace_back(token(Eof, "Eof"));
    return tokens;
}

/* parser */
Parser::Parser(str& code) {
    src = tokenize(code);
    tokens = src;
    tokenIndex = 0;
}

Token Parser::current() { return src[tokenIndex]; }

Token Parser::eat(TokenType type) {
    Token t = shift(src);
    if (type >= 0 && t.type != type) {
        cerr << "Unexpected token " << t.value << " of type " << t.type;
        abort();
    }
    return t;
}

Token Parser::eat() { return shift(src); }

Element Parser::parseElement() {
    Element elem;
    elem.tagName = eat(Tag).value;
    eat(Colon);

    while (true) {
        Token t = current();

        if (t.type == Semicolon) {
            eat();
            elem.closed = true;
            break;
        }

        if (t.type == Tag) {
            elem.children.emplace_back(parseElement());
            continue;
        }

        vector<TokenType> blocks = { Props, Content, Dataset, Classes, Ids, Style };
        if (contain(blocks, t.type)) parseSet(&elem);
        else {
            cerr << "Unexpected token";
            abort();
        }
    }

    return elem;
}

str Parser::trim(const str& s) {
    size_t start = 0, end = s.size();
    while (start < end && std::isspace(static_cast<unsigned char>(s[start]))) ++start;
    while (end > start && std::isspace(static_cast<unsigned char>(s[start]))) --end;
    return s.substr(start, end - start);
}

vector<KeyVal> Parser::parseKeyValue(str value) {
    vector<KeyVal> arr;
    str curr;
    bool quotes = false;

    for (size_t i = 0; i <= value.size(); ++i) {
        char c = i < value.size() ? value[i] : '\0';
        if (c == '\0' || (c == ',' && !quotes)) {
            str t = trim(curr);
            if (!t.empty()) {
                size_t pos = t.find(':');
                if (pos != str::npos)
                    arr.emplace_back(trim(t.substr(0, pos)), trim(t.substr(pos + 1)) );
            }
            curr.clear();
        }
        else if (c == '"') { quotes = !quotes; curr += c; }
        else curr += c;
    }

    return arr;
}

vector<str> Parser::parseList(str value) {
    vector<str> arr;
    str curr;
    bool quotes = false;

    for (size_t i = 0; i <= value.size(); ++i) {
        char c = i < value.size() ? value[i] : '\0';
        if (c == '\0' || (c == ',' && !quotes)) {
            if (!trim(curr).empty()) arr.emplace_back(unquote(trim(curr)));
            curr.clear();
        }
        else if (c == '"') { quotes = !quotes; curr += c; }
        else curr += c;
    }

    return arr;
}

str Parser::unquote(str s) {
    return s.size() >= 2 && s[0] == '"' && s.back() == '"'
        ? s.substr(1, s.size() - 2)
        : s;
}

str Parser::parseContent(str value) {
    str res;
    for (size_t i = 0; i < value.size(); ++i) {
        if (value[i] == '\\' && i + 1 < value.size() && value[i + 1] == ',') {
            res += ','; i++;
        }
        else res += value[i];
    }
    return res;
}

void Parser::parseSet(Element* elem) {
    Token t = current();
    switch (t.type) {
    case Props: eat(); elem->props = parseKeyValue(eat(Block).value); break;
    case Dataset: eat(); elem->datasets = parseKeyValue(eat(Block).value); break;
    case Classes: eat(); elem->classes = parseList(eat(Block).value); break;
    case Ids: eat(); elem->ids = parseList(eat(Block).value); break;
    case Content: eat(); elem->content = parseContent(eat(Block).value); break;
    case Style: eat(); elem->style = parseKeyValue(eat(Block).value); break;
    default: cerr << "Invalid set"; abort();
    }
}

vector<Element> Parser::parse() {
    while (current().type != Eof)
        tree.emplace_back(parseElement());
    return tree;
}

/* html */
str style(const vector<KeyVal>& t) {
    if (t.empty()) return "";
    std::ostringstream out;
    out << " style=\"";
    for (size_t i = 0; i < t.size(); i++) {
        out << t[i].key << ":" << t[i].value;
        if (i + 1 < t.size()) out << ";";
    }
    out << "\"";
    return out.str();
}

str dataset(const vector<KeyVal>& t) {
    if (t.empty()) return "";
    std::ostringstream out;
    for (const auto& x : t)
        out << " data-" << x.key << "=\"" << x.value << "\"";
    return out.str();
}

str classes(const vector<str>& t) {
    if (t.empty()) return "";
    std::ostringstream out;
    out << " class=\"";
    for (const auto& x : t) out << x << " ";
    out << "\"";
    return out.str();
}

str ids(const vector<str>& t) {
    if (t.empty()) return "";
    std::ostringstream out;
    out << " id=\"";
    for (const auto& x : t) out << x << " ";
    out << "\"";
    return out.str();
}

str props(const vector<KeyVal>& t) {
    if (t.empty()) return "";
    std::ostringstream out;
    for (const auto& x : t) {
        if (x.key == "defer") out << " " << x.key;
        else out << " " << x.key << "=\"" << x.value << "\"";
    }
    return out.str();
}

str attr(const Element& e) {
    return style(e.style)
        + dataset(e.datasets)
        + ids(e.ids)
        + classes(e.classes)
        + props(e.props);
}

str convertElementToHTML(const Element& e, size_t depth) {
    str indent = "";
    for (size_t i = 0; i < depth; i++) indent += "\t";

    std::ostringstream out;
    out << indent << "<" << e.tagName << attr(e) << ">\n";

    if (!e.content.empty())
        out << indent << "\t" << e.content << "\n";

    for (const auto& child : e.children)
        out << convertElementToHTML(child, depth + 1);

    if (!e.closed || !e.children.empty() || !e.content.empty())
        out << indent << "</" << e.tagName << ">\n";

    return out.str();
}

str convertToHTML(const str& src) {
    str copy = src;
    Parser p(copy);
    auto tree = p.parse();

    std::ostringstream out;
    for (size_t i = 0; i < tree.size(); ++i) {
        out << convertElementToHTML(tree[i]);
        if (i + 1 < tree.size()) out << "\n";
    }
    return out.str();
}

