#ifndef SVG_CONVERTER_UTILITY_H
#define SVG_CONVERTER_UTILITY_H

#include <boost/mpl/fold.hpp>
#include <boost/mpl/insert.hpp>
#include <boost/mpl/lambda.hpp>

namespace detail {

template <class... Types>
struct Concat;

template <class T>
struct Concat<T> {
    using type = T;
};

template <class T, class... Types>
struct Concat<T, Types...> {
    using type = typename boost::mpl::fold<
        T, typename Concat<Types...>::type,
        boost::mpl::insert<boost::mpl::_1, boost::mpl::_2>>::type;
};

}  // namespace detail

/**
 * Concatenate a number of boost mpl sequences.
 */
template <class... Types>
using Concat = typename detail::Concat<Types...>::type;

#endif  // SVG_CONVERTER_UTILITY_H
