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
 * A visitor for `Path::to_polylines` that dashifies the visited polyline.
 *
 * The dashified lines are reported to a wrapped visitor factory.
 */
template <class PolylineVisitorFactory>
class DashifyingPolylineVisitor {
 private:
    PolylineVisitorFactory wrapped_visitor_factory_;

    detail::CyclicIterator<double> current_dash_iter_;
    double current_dash_remaining_;
    bool is_current_dash_empty_ = false;
    Vector current_point_;

    /**
     * Report a dash to the wrapped visitor factory.
     */
    void report_dash(Vector start_point, Vector end_point);

 public:
    /**
     * Creates a new instance.
     *
     * @param polyline_visitor_factory Wrapped visitor factory to report the
     *                                 created dash lines to.
     * @param start_point Start point of the polyline to dashify.
     * @param dasharray Dasharray specifying the length and spacing of the
     *                  dashes. Reference must be valid for the entire lifetime
     *                  of this object.
     */
    DashifyingPolylineVisitor(PolylineVisitorFactory polyline_visitor_factory,
                              Vector start_point,
                              const std::vector<double>& dasharray);

    void operator()(Vector point);
};

template <class PolylineVisitorFactory>
DashifyingPolylineVisitor<PolylineVisitorFactory>::DashifyingPolylineVisitor(
    PolylineVisitorFactory polyline_visitor_factory, Vector start_point,
    const std::vector<double>& dasharray)
    : wrapped_visitor_factory_{polyline_visitor_factory},
      current_dash_iter_{detail::make_cyclic_iter(dasharray)},
      current_dash_remaining_{*current_dash_iter_},
      current_point_{std::move(start_point)} {}

template <class PolylineVisitorFactory>
void DashifyingPolylineVisitor<PolylineVisitorFactory>::report_dash(
    Vector start_point, Vector end_point) {
    auto&& visitor = wrapped_visitor_factory_(start_point);
    visitor(end_point);
}

template <class PolylineVisitorFactory>
void DashifyingPolylineVisitor<PolylineVisitorFactory>::operator()(
    Vector point) {
    Vector delta = point - current_point_;
    double line_remaining = delta.norm();
    Vector unit_vec = delta.normalized();

    while (current_dash_remaining_ < line_remaining) {
        Vector target = current_point_ + current_dash_remaining_ * unit_vec;
        if (!is_current_dash_empty_) {
            report_dash(current_point_, target);
        }

        line_remaining -= current_dash_remaining_;
        current_dash_iter_++;
        current_dash_remaining_ = *current_dash_iter_;
        current_point_ = target;
        is_current_dash_empty_ = !is_current_dash_empty_;
    }

    if (!is_current_dash_empty_) {
        report_dash(current_point_, point);
    }

    current_dash_remaining_ -= line_remaining;
    current_point_ = point;
}

#endif  // SVG_CONVERTER_PARSING_DASHES_H_
