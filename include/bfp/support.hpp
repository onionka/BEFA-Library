/**
 * @file support.hpp
 * @author Miroslav Cibulka
 * @brief Supporting function header
 *        search, find, apply, dereference
 */

#ifndef __BFP_SUPPORT_HPP
# define __BFP_SUPPORT_HPP

#ifndef BINARY_FILE_PARSER_BFP_HPP
# error "Don't include this file directly, use #include <bfp.hpp> instead"
#endif

#include <type_traits>            /// ::std::remove_reference
#include <bfp/support_helpers.tpp>/// support template functions


namespace bfp
  {
      /** Derefer pointer to object 'till only object remain
       * @tparam __ptr pointer type
       * @param _obj object/pointer to object
       * @return reference to object
       */
      template<
          typename __ptr>
        typename remove_references<__ptr>::type &dereference(
            __ptr *_obj);


      /** Finds elements via comparing not the same typed value,
       *      but has to have implemented operator == with __value type
       * @tparam __ite iterator type
       * @tparam __finder_type is finding value type
       * @tparam __value_type MUST be pointer or has implemented dereference operator
       * @param _begin iterator
       * @param _end iterator
       * @param _val to be search for
       * @return vector of found elements
       */
      template<
          typename __ite,
          typename __finder_type,
          typename __value = typename ::std::iterator_traits<__ite>::value_type>
        const ::std::vector<__value> search(
            __ite _begin,
            __ite _end,
            __finder_type _val);

      /**
       * @details Finds element via comparing not the same typed value,
       *          but has to have implemented operator == with __value type
       * @tparam __ite iterator type
       * @tparam __finder_type is finding value type
       * @tparam __value_type MUST be pointer or has implemented dereference operator
       * @param _begin iterator
       * @param _end iterator
       * @param _val to be search for
       * @return iterator on first element found
       */
      template<
          typename __ite,
          typename __finder_type,
          typename __value = typename ::std::iterator_traits<__ite>::value_type>
        __ite find(
            __ite _begin,
            __ite _end,
            __finder_type _val);

      /**
        * @brief Apply function to range from begin to end
        * @tparam __ite class type
        * @tparam __func class type
        * @tparam __args Variadic arguments
        * @tparam __value type of iterator
        * @param begin iterator
        * @param end iterator
        * @param func is function with parameters
        *        @code
        *          func(__value _val, __args... _args);
        *        @endcode
        * @param _args are variadic of func arguments
        */
      template<
          typename __ite,
          typename __func,
          typename __ret = typename __apply::function_traits<__func>::result_type,
          typename __value = typename ::std::iterator_traits<__ite>::value_type,
          typename... __args>
        __ret apply(
            __ite begin,
            __ite end,
            __func func,
            __args... _args);

      /**
       * @brief filter outs elements in range <begin, end)
       *        and puts it into vector
       * @tparam __ite is iterator
       * @tparam __predicate is callable predicate that via
       *                     its return value we decide if
       *                     element will be added to output vector
       * @tparam __return is output vector of iterator values
       * @param begin of iteration
       * @param end of iteration
       * @param _func is function/callable that will be called
       *              with iterating element as parameter
       *              synopsis:
       *              @code
       *                  bool (*_func)(elem_t);
       *              @endcode
       * @return vector of filtered elements
       */
      template<
          typename __ite,
          typename __predicate,
          typename __return = ::std::vector<
              typename ::std::iterator_traits<__ite>::value_type>>
        __return filter(
            __ite begin,
            __ite end,
            __predicate _func);
  }

#include <bfp/support.tpp>        /// support template functions


#endif //__BFP_SUPPORT_HPP
