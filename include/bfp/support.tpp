/**
 * @file support.tpp
 * @author Miroslav Cibulka
 * @brief This file contains implementation of support template functions
 */

#ifndef __BFP_SUPPORT_TPP
# define __BFP_SUPPORT_TPP

#include <vector>
#include <bfp/support_helpers.tpp>


#ifndef BINARY_FILE_PARSER_BFP_HPP
# error "Don't include this file directly, include bfp.hpp instead"
#endif

namespace bfp
  {
      template<
          typename __T>
        __T &dereference(__T &_obj)
          {
            return _obj;
          }

      template<
          typename __T>
        const __T &dereference(const __T &_obj)
          {
            return _obj;
          }

      template<
          typename __ptr>
        typename remove_references<__ptr>::type &dereference(__ptr *_obj)
          {
            return dereference(*_obj);
          }

      template<
          typename __ite,
          typename __finder_type,
          typename __value = typename ::std::iterator_traits<__ite>::value_type>
        const ::std::vector<__value> search(
            __ite _begin,
            __ite _end,
            __finder_type _val)
          {
            ::std::vector<__value> _ret;
            for (__ite _ite = _begin;
                 _ite != _end;
                 ++_ite)
              if (dereference(*_ite) == _val)
                _ret.push_back(*_ite);
            return _ret;
          }

      template<
          typename __ite,
          typename __finder_type,
          typename __value = typename ::std::iterator_traits<__ite>::value_type>
        __ite find(
            __ite _begin,
            __ite _end,
            __finder_type _val)
          {
            __ite _ite;
            for (_ite = _begin;
                 _ite != _end;
                 ++_ite)
              if (dereference(*_ite) == _val) break;
            return _ite;
          }

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
            __args... _args)
          {
            return __apply::call<__ret>::template apply<
                __ite,
                __func,
                __value,
                __args...>(begin, end, func, _args...);
          }


      template<
          typename __ite,
          typename __predicate,
          typename __return = ::std::vector<
              typename ::std::iterator_traits<__ite>::value_type>>
        __return filter(
            __ite begin,
            __ite end,
            __predicate _func)
          {
            __return ret;
            for (auto _ite = begin;
                 _ite != end;
                 ++_ite)
              if (_func(*_ite))
                ret.push_back(*_ite);
            return ret;
          }

  }
#endif // __BFP_SUPPORT_TPP