#include <cstdlib>
#include <iostream>

#include <svgpp/svgpp.hpp>
#include <libxml/parser.h>

#include "xml.h"

XmlDocument loadSvg(const char* filename) {
    try {
        return XmlDocument(filename);
    } catch (const XmlLoadError& err) {
        std::cerr << "Failed to load svg: " << err.what() << '\n';
        std::exit(1);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " filename.svg\n";
        return 1;
    }

    LIBXML_TEST_VERSION

    auto doc = loadSvg(argv[1]);

    return 0;
}