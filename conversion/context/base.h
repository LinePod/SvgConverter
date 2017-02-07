#ifndef SVG_CONVERTER_BASE_H
#define SVG_CONVERTER_BASE_H

#include <boost/numeric/ublas/matrix.hpp>

typedef boost::numeric::ublas::c_matrix<double, 3, 3> matrix_t;

/**
 * Base svg++ context used for structural elements like <svg> or <g>.
 *
 * Also used as a base class for all other context types.
 */
class BaseContext {
protected:
    matrix_t transform_;

public:
    BaseContext();
    BaseContext(const BaseContext& parent);

    void on_exit_element();

    void transform_matrix(const boost::array<double, 6>& numbers);
};

#endif //SVG_CONVERTER_BASE_H
