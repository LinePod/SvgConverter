#include "base.h"

BaseContext::BaseContext() : transform_{Transform::Identity()} {
}

BaseContext::BaseContext(const BaseContext& parent)
        : transform_(parent.transform_) {
}

void BaseContext::on_exit_element() {
}

void BaseContext::transform_matrix(const boost::array<double, 6>& numbers) {
    transform_.matrix() << numbers[0], numbers[2], numbers[4],
                           numbers[1], numbers[3], numbers[5];
}
