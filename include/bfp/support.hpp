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
       * @param __ptr pointer type
       * @param _obj object/pointer to object
       * @return reference to object
       */
      template<
          typename __ptr>
        typename remove_references<__ptr>::type &dereference(
            __ptr *_obj);


      /** Finds elements via comparing not the same typed value,
       *      but has to have implemented operator == with __value type
       * @param __ite iterator type
       * @param __finder_type is finding value type
       * @param __value_type MUST be pointer or has implemented dereference operator
       * @param _begin iterator
       * @param _end iterator
       * @param _val to be search for
       * @return vector of found elements
       */
      template<
          typename __ite,
          typename __finder_type,
          typename __value = typename __ite::value_type>
        const ::std::vector<__value> search(
            __ite _begin,
            __ite _end,
            __finder_type _val);

      /** Finds element via comparing not the same typed value,
       *      but has to have implemented operator == with __value type
       * @param __ite iterator type
       * @param __finder_type is finding value type
       * @param __value_type MUST be pointer or has implemented dereference operator
       * @param _begin iterator
       * @param _end iterator
       * @param _val to be search for
       * @return iterator on first element found
       */
      template<
          typename __ite,
          typename __finder_type,
          typename __value = typename __ite::value_type>
        __ite find(
            __ite _begin,
            __ite _end,
            __finder_type _val);

      /**
        * @brief Apply function to range from begin to end
        * @param begin iterator
        * @param end iterator
        * @param func is function with parameters
        *        @code
        *          func(__value _val, __args... _args);
        *        @endcode
        * @param _args are variadic of func arguments
        * @param __ite class type
        * @param __func class type
        * @param __args Variadic arguments
        * @param __value type of iterator
        */
      template<
          typename __ite,
          typename __func,
          typename __ret = typename __apply::function_traits<__func>::result_type,
          typename __value = typename __ite::value_type,
          typename... __args>
        __ret apply(
            __ite begin,
            __ite end,
            __func func,
            __args... _args);
  }

#include <bfp/support.tpp>        /// support template functions

#endif //__BFP_SUPPORT_HPP
