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
 * Converts a polyline into a dashed line point by point.
 */
template <class Callback>
class PolylineDashifier {
 private:
    Callback dash_callback_;

    detail::CyclicIterator<double> current_dash_iter_;
    double current_dash_remaining_;
    bool is_current_dash_empty_ = false;
    Vector current_point_;

 public:
    /**
     * Creates a new instance.
     *
     * @param callback Will be called with a start and an end point for each
     *                 dash.
     * @param starting_point Start point of the polyline.
     * @param dasharray Dasharray as specified by the SVG spec. Must not be
     *                  empty. Reference must be valid for the lifetime of this
     *                  object.
     */
    PolylineDashifier(Callback callback, Vector starting_point,
                      const std::vector<double>& dasharray);

    /**
     * Processes another point on the polyline.
     */
    void process(Vector point);
};

template <class Callback>
PolylineDashifier<Callback>::PolylineDashifier(
    Callback callback, Vector starting_point,
    const std::vector<double>& dasharray)
    : dash_callback_{callback},
      current_dash_iter_{detail::make_cyclic_iter(dasharray)},
      current_dash_remaining_{*current_dash_iter_},
      current_point_{starting_point} {}

template <class Callback>
void PolylineDashifier<Callback>::process(Vector point) {
    Vector delta = point - current_point_;
    double line_remaining = delta.norm();
    Vector unit_vec = delta.normalized();

    while (current_dash_remaining_ < line_remaining) {
        Vector target = current_point_ + current_dash_remaining_ * unit_vec;
        if (!is_current_dash_empty_) {
            dash_callback_(current_point_, target);
        }

        line_remaining -= current_dash_remaining_;
        current_dash_iter_++;
        current_dash_remaining_ = *current_dash_iter_;
        current_point_ = target;
        is_current_dash_empty_ = !is_current_dash_empty_;
    }

    if (!is_current_dash_empty_) {
        dash_callback_(current_point_, point);
    }

    current_dash_remaining_ -= line_remaining;
    current_point_ = point;
}

#endif  // SVG_CONVERTER_PARSING_DASHES_H_
