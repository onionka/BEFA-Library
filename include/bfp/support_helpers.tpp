/**
 * @file support_helpers.tpp
 * @author Miroslav Cibulka
 * @brief This file contains implementation of support template functions
 */

#ifndef __BFP_HELPERS_TPP
# define __BFP_HELPERS_TPP

#include <type_traits>
#include <iostream>
#include <tuple>


#ifndef BINARY_FILE_PARSER_BFP_HPP
# error "Don't include this file directly, include bfp.hpp instead"
#endif

namespace bfp
  {
      template<typename __T>
        struct remove_references
          {
            typedef typename ::std::remove_reference<__T>::type type;
          };

      /**
       * @brief retrieves type of object that is hidden behind nth reference
       * @param __ptr is object/pointer
       */
      template<typename __ptr>
        struct remove_references<__ptr *>
          {
            /** Here is hidden type behind pointer */
            typedef typename remove_references<__ptr>::type type;
          };

      namespace __apply
        {
            template<typename T>
              struct function_traits :
                  public function_traits<decltype(&T::operator())>
                {
                };

            /**
             * @brief generates info about function statically
             * @param ClassType is type of class in (lambda) function - deduced
             * @param ReturnType is type of return value in (lambda) function - deduced
             * @param Args are types of arguments in (lambda) function - deduced
             */
            template<
                typename ClassType,
                typename ReturnType,
                typename... Args>
              struct function_traits<ReturnType(ClassType::*)(Args...) const>
                {
                  enum
                    {
                      argc = sizeof...(Args)                   //!< @brief count of arguments
                    };

                  /** @return type of function */
                  typedef ReturnType result_type;

                  /**
             * @brief set of types in function arguments
             */
                  template<size_t i>
                    struct argv
                      {
                        /**
                   * @brief the i-th argument is equivalent to the i-th tuple element of a tuple
                   *        composed of those arguments.
                   */
                        typedef typename ::std::tuple_element<
                            i,
                            std::tuple<Args...>>::type type;
                      };
                };

            /**
             * @brief call function with certain, non-void, return value
             * @param __ret is type of return value
             */
            template<
                typename __ret>
              struct call
                {
                  /** @see ::BFP::apply */
                  template<
                      typename __ite,
                      typename __func,
                      typename __value,
                      typename... __args>
                    static __ret apply(
                        __ite begin,
                        __ite end,
                        __func func,
                        __args... _args)
                      {
                        auto ret = func(*begin, _args...);
                        for (auto _ite = begin + 1; _ite != end; _ite++)
                          ret = ret + func(*_ite, _args...);
                        return ret;
                      }
                };

            /**
             * @brief call function with void return value
             * @overload
             */
            template<>
              struct call<void>
                {
                  /** @see ::BFP::apply */
                  template<
                      typename __ite,
                      typename __func,
                      typename __value,
                      typename... __args>
                    static void apply(
                        __ite begin,
                        __ite end,
                        __func func,
                        __args... _args)
                      {
                        for (auto _ite = begin; _ite != end; _ite++)
                          func(*_ite, _args...);
                      }
                };
        }
  }
#endif //__BFP_HELPERS_TPP