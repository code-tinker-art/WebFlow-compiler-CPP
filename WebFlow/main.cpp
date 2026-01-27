#include <fstream>
#include <string>
#include "compiler.h"

int main() {
    str code;
    std::ifstream in("./Testing files/test.webf");

    if (!in.is_open()) {
        cout << "File does not exist..." << endl;
        return 1;
    }

    str line;
    while (std::getline(in, line)) {
        code += line + '\n';
    }
    in.close();

    str output = convertToHTML(code);

    std::ofstream out("./Testing files/test.html");
    if (!out.is_open()) {
        return 1;
    }

    out << output;
    cout << "HTML generated sucessfully -> ./Testing files/test.html" << endl;
    out.close();

    return 0;
}
