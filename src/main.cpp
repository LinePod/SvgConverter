#include <cstdlib>
#include <iostream>

#include "conversion.h"
#include "logging.h"
#include "svg.h"

SvgDocument load_svg(spdlog::logger& logger, const std::string& filename) {
    try {
        return SvgDocument{filename};
    } catch (const SvgLoadError& err) {
        logger.critical("Failed to load svg: {}", err.what());
        std::exit(1);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " filename.svg\n";
        return 1;
    }

    LIBXML_TEST_VERSION

    spdlog::logger& logger = setup_global_logger();
    auto doc = load_svg(logger, argv[1]);
    std::cout << convert(doc);

    return 0;
}
