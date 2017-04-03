#ifndef SVG_CONVERTER_PARSING_DASHES_H_
#define SVG_CONVERTER_PARSING_DASHES_H_

#include <iterator>
#include <vector>

#include <boost/iterator/iterator_adaptor.hpp>
#include <boost/range.hpp>

#include "../math_defs.h"

namespace detail {

/**
 * Iterator that cycles over a vector indefinitely.
 */
template <class T>
class CyclicIterator
    : public boost::iterator_adaptor<
          CyclicIterator<T>, typename std::vector<T>::const_iterator,
          boost::use_default, std::forward_iterator_tag> {
 private:
    const std::vector<T>& vec_;

 public:
    explicit CyclicIterator(const std::vector<T>& vec);

    void increment();
};

template <class T>
CyclicIterator<T>::CyclicIterator(const std::vector<T>& vec)
    : CyclicIterator<T>::iterator_adaptor_{vec.begin()}, vec_{vec} {}

template <class T>
void CyclicIterator<T>::increment() {
    this->base_reference()++;
    if (this->base_reference() == vec_.end()) {
        this->base_reference() = vec_.begin();
    }
}

template <class T>
CyclicIterator<T> make_cyclic_iter(const std::vector<T>& vec) {
    return CyclicIterator<T>{vec};
}

}  // namespace detail

/**
 * Convert a polyline with an associated dasharray to single dashes.
 *
 * The given callback will be called like `callback(start_point, end_point)` for
 * each dash. The dashes will be exported consecutively along the polyline.
 */
template <class Callback>
void to_dashes(const std::vector<Point>& polyline,
               const std::vector<double>& dasharray, Callback callback) {
    Point current_point = polyline.front();

    if (dasharray.empty()) {
        for (auto point :
             boost::make_iterator_range(polyline).advance_begin(1)) {
            callback(current_point, point);
            current_point = point;
        }

        return;
    }

    auto current_dash_iter = detail::make_cyclic_iter(dasharray);
    double current_dash_remaining = *current_dash_iter;
    bool is_hole = false;

    for (auto point : boost::make_iterator_range(polyline).advance_begin(1)) {
        Point delta = point - current_point;
        double line_remaining = delta.norm();
        Point unit_vec = delta.normalized();

        while (current_dash_remaining < line_remaining) {
            Point target = current_point + current_dash_remaining * unit_vec;
            if (!is_hole) {
                callback(current_point, target);
            }

            line_remaining -= current_dash_remaining;
            current_dash_iter++;
            current_dash_remaining = *current_dash_iter;
            current_point = target;
            is_hole = !is_hole;
        }

        if (!is_hole) {
            callback(current_point, point);
        }

        current_dash_remaining -= line_remaining;
        current_point = point;
    }
}

#endif  // SVG_CONVERTER_PARSING_DASHES_H_
