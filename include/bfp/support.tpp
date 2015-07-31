/**
 * @file support.tpp
 * @author Miroslav Cibulka
 * @brief This file contains implementation of support template functions
 */

#ifndef __BFP_SUPPORT_TPP
#define __BFP_SUPPORT_TPP

#include <vector>
#include <bfp/support_helpers.tpp>

namespace BFP
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
          typename __value = typename __ite::value_type>
        const ::std::vector <__value> search(
            __ite _begin,
            __ite _end,
            __finder_type _val)
          {
            ::std::vector <__value> _ret;
            for (__ite _ite = _begin; _ite != _end; ++_ite)
              if (dereference(*_ite) == _val)
                _ret.push_back(*_ite);
            return _ret;
          }

      template<
          typename __ite,
          typename __finder_type,
          typename __value = typename __ite::value_type>
        __ite find(
            __ite _begin,
            __ite _end,
            __finder_type _val)
          {
            __ite _ite;
            for (_ite = _begin; _ite != _end; ++_ite)
              if (dereference(*_ite) == _val) break;
            return _ite;
          }

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
            __args... _args)
          {
            return __apply::call<__ret>::template apply<
                __ite,
                __func,
                __value,
                __args...>(begin, end, func, _args...);
          }

      namespace __filter
        {
            /** @overload */
            template<
                typename __arg,
                typename __func>
              bool helper(
                  __arg _arg,
                  __func _func)
                {
                  return _func(_arg);
                }

            /**
             * @brief Helper template function that calls all function
             *        and returns ...
             */
            template<
                typename __arg,
                typename __func,
                typename... __funcs>
              bool helper(
                  __arg _arg,
                  __func _func,
                  __funcs... _funcs)
                {
                  return _func(_arg) && helper(_arg, _funcs...);
                }
        }

      template<
          class __ite,
          typename... __funcs>
        ::std::vector <typename __ite::value_type> filter(
            __ite begin,
            __ite end,
            __funcs... _func)
          {
            ::std::vector <typename __ite::value_type> ret;
            for (auto _ite = begin; _ite != end; ++_ite)
              if (__filter::helper(*_ite, _func...))
                ret.push_back(*_ite);
            return ret;
          }

  }
#endif // __BFP_SUPPORT_TPP