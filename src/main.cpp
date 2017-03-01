#include <libxml/parser.h>

#include <cstdlib>
#include <iostream>
#include <svgpp/svgpp.hpp>

#include "conversion.h"
#include "xml.h"

ManagedXmlDoc loadSvg(const char* filename);

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " filename.svg\n";
        return 1;
    }

    LIBXML_TEST_VERSION

    auto doc = loadSvg(argv[1]);
    std::cout << convert(doc);

    return 0;
}

ManagedXmlDoc loadSvg(const char* filename) {
    try {
        return loadDocument(filename);
    } catch (const XmlLoadError& err) {
        std::cerr << "Failed to load svg: " << err.what() << '\n';
        std::exit(1);
    }
}
