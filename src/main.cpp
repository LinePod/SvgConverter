#include <cstdlib>
#include <iostream>

#include "conversion.h"
#include "xml.h"

XmlDocument load_svg(const std::string& filename) {
    try {
        return XmlDocument{filename};
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

    auto doc = load_svg(argv[1]);
    std::cout << convert(doc);

    return 0;
}
