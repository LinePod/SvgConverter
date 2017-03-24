#ifndef SVG_CONVERTER_MPL_UTIL_H_
#define SVG_CONVERTER_MPL_UTIL_H_

#include <boost/mpl/fold.hpp>
#include <boost/mpl/insert.hpp>
#include <boost/mpl/lambda.hpp>
#include <boost/mpl/set.hpp>

namespace detail {

namespace mpl = boost::mpl;

template <class... Types>
struct ConcatImpl;

template <class T>
struct ConcatImpl<T> {
    using type = T;
};

template <class T, class... Types>
struct ConcatImpl<T, Types...> {
    using type = typename mpl::fold<T, typename ConcatImpl<Types...>::type,
                                    mpl::insert<mpl::_1, mpl::_2>>::type;
};

using SetInserter = mpl::inserter<mpl::set<>, mpl::insert<mpl::_1, mpl::_2>>;

template <class Firsts, class Second>
using PairAllImpl =
    mpl::transform<Firsts, mpl::pair<mpl::_1, Second>, SetInserter>;

template <class Map, class Key, class Default>
struct AtOrDefaultImpl {
    using ValueOrVoid = typename mpl::at<Map, Key>::type;

    using type =
        mpl::if_<boost::is_same<ValueOrVoid, mpl::void_>, Default, ValueOrVoid>;
};

}  // namespace detail

/**
 * Concatenate a number of boost mpl sequences.
 */
template <class... Types>
using Concat = typename detail::ConcatImpl<Types...>::type;

/**
 * Create a set of pairs with varying first and a static second item.
 */
template <class Firsts, class Second>
using PairAll = typename detail::PairAllImpl<Firsts, Second>::type;

/**
 * Lookup value in map, and return a default if the map doesn't contain the key.
 */
template <class Map, class Key, class Default>
using AtOrDefault = typename detail::AtOrDefaultImpl<Map, Key, Default>::type;

#endif  // SVG_CONVERTER_MPL_UTIL_H_
