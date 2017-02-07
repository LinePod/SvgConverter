#include <boost/numeric/ublas/assignment.hpp>
#include "base.h"

BaseContext::BaseContext() {
    transform_ = boost::numeric::ublas::identity_matrix<double>(3);
}

BaseContext::BaseContext(const BaseContext& parent)
        : transform_(parent.transform_) {
}

void BaseContext::on_exit_element() {
}

void BaseContext::transform_matrix(const boost::array<double, 6>& numbers) {
    matrix_t matrix;
    matrix <<=
            numbers[0], numbers[2], numbers[4],
            numbers[1], numbers[3], numbers[5],
            0, 0, 1;
    transform_ = boost::numeric::ublas::prod(transform_, matrix);
}
