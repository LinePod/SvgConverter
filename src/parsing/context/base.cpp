#include "base.h"

BaseContext::BaseContext(const SvgDocument& document, spdlog::logger& logger)
    : document_{document}, logger_{logger} {}

void BaseContext::on_exit_element() {}
