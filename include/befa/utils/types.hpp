//
// Created by miro on 1/31/17.
//

#ifndef BEFA_TYPES_HPP
#define BEFA_TYPES_HPP

#include "byte_array_view.hpp"
namespace types {
namespace tags {

#define DEFINE_TAG(name) \
struct                name          {};

DEFINE_TAG           (basic)
DEFINE_TAG           (pointer)
DEFINE_TAG           (rx)
DEFINE_TAG           (vector)
DEFINE_TAG           (arr_view)
DEFINE_TAG           (list)
DEFINE_TAG           (map)

#undef DEFINE_TAG
}  // namespace tags

// ~~~~~ Traits forward declarations
/**
 *
 * @tparam T Is simple type you want to derive types from
 * @tparam TagT is a tag of certain types (like pointers, maps etc.)
 * @tparam ArgsT are argument required by certain type (ie. map need key)
 */
template<
    typename                         T,
    typename                                                TagT,
    typename...                                                       ArgsT
>
struct trait_container                                                         {};
template<
    typename                         T
>
struct trait_container              <T, const types::tags:: pointer             >;
template<
    typename                         T
>
struct trait_container              <T, const types::tags:: basic               >;
template<
    typename                         T
>
struct trait_container              <T, const types::tags:: vector              >;
template<
    typename                         T
>
struct trait_container              <T, const types::tags:: list                >;
template<
    typename                         T
>
struct trait_container              <T, const types::tags:: map                 >;
template<
    typename                         T
>
struct trait_container              <T,       types::tags:: rx                  >;
template<
    typename                         T
>
struct trait_container              <T,       types::tags:: pointer             >;
template<
    typename                         T
>
struct trait_container              <T,       types::tags:: basic               >;
template<
    typename                         T
>
struct trait_container              <T,       types::tags:: vector              >;
template<
    typename                         T
>
struct trait_container              <T,       types::tags:: arr_view            >;
template<
    typename                         T
>
struct trait_container              <T,       types::tags:: list                >;
template<
    typename                         T,
    typename                                                          KeyT
>
struct trait_container              <T,       types::tags:: map,      KeyT      >;
// ~~~~~ Traits forward declarations

/**
 * So you can access any trait from anywhere
 */
template<
    typename                         T
>
struct trait_refs {
  /** Constant pointers aliases */
  using c_ptr     = trait_container <T,   const types::tags::pointer            >;
  /** Standard constant type aliases (ie. type = const T; ref = const T&; ...) */
  using c_info    = trait_container <T,   const types::tags::basic              >;
  /** Constant vector aliases */
  using c_vector  = trait_container <T,   const types::tags::vector             >;
  /** Constant list aliases */
  using c_list    = trait_container <T,   const types::tags::list               >;
  /** Constant map aliases */
  template<
      typename                                                        KeyT
  >
  using c_map     = trait_container <T,   const types::tags::map,     KeyT      >;
  /** Reactive CPP types aliases */
  using rx        = trait_container <T,         types::tags::rx                 >;
  /** Pointers aliases */
  using ptr       = trait_container <T,         types::tags::pointer            >;
  /** Standard type aliases (ie. type = T; ref = T&; ...) */
  using info      = trait_container <T,         types::tags::basic              >;
  /** Vector aliases */
  using vector    = trait_container <T,         types::tags::vector             >;
  /** Raw_array aliases */
  using arr_view  = trait_container <T,         types::tags::arr_view           >;
  /** Lists aliases */
  using list      = trait_container <T,         types::tags::list               >;
  /** Map aliases */
  template<
      typename                                                        KeyT
  >
  using map       = trait_container <T,         types::tags::map,     KeyT      >;
};

// ~~~~~ Const
template<
    typename                         T
>
struct trait_container              <T,   const types::tags::pointer            >
    : trait_refs                    <T> {
  using shared    = std::shared_ptr <T    const>;
  using weak      = std::weak_ptr   <T    const>;
  using raw       =                  T    const *;
};

template<typename T>
struct trait_container              <T,   const types::tags::vector             >
    : trait_refs                    <T> {
  using shared    = std::vector<
      typename trait_container      <T,   const types::tags::pointer            >::shared
  >;
  using weak      = std::vector<
      typename trait_container      <T,   const types::tags::pointer            >::weak
  >;
  using raw       = std::vector<
      typename trait_container      <T,   const types::tags::pointer            >::raw
  >;
  using value     = std::vector<
      typename trait_container      <T,   const types::tags::basic              >::type
  >;
};

template<
    typename                         T
>
struct trait_container              <T,   const types::tags::list               >
    : trait_refs                    <T> {
  using shared    = std::list<
      typename trait_container      <T,   const types::tags::pointer            >::shared
  >;
  using weak      = std::list<
      typename trait_container      <T,   const types::tags::pointer            >::weak
  >;
  using raw       = std::list<
      typename trait_container      <T,   const types::tags::pointer            >::raw
  >;
  using value     = std::list<
      typename trait_container      <T,   const types::tags::basic              >::type
  >;
};

template<
    typename                         T,
    typename                                                          KeyT
>
struct trait_container              <T,   const types::tags::map,     KeyT      >
    : trait_refs                    <T> {
  using shared    = std::map<
                                                                      KeyT,
      typename trait_container      <T,   const types::tags::pointer            >::shared
  >;
  using weak      = std::map<
                                                                      KeyT,
      typename trait_container      <T,   const types::tags::pointer            >::weak
  >;
  using raw       = std::map<
                                                                      KeyT,
      typename trait_container      <T,   const types::tags::pointer            >::raw
  >;
  using value     = std::map<
                                                                      KeyT,
      typename trait_container      <T,   const types::tags::basic              >::type
  >;
};

template<
    typename                         T
>
struct trait_container              <T,   const types::tags::basic              >
    : trait_refs                    <T> {
  using type       =                 T    const                                 ;
  using ref        =                 T    const&                                ;
};
// ~~~~~ Const

// ~~~~~ Non-Const

template<
    typename                         T
>
struct trait_container              <T,         types::tags::arr_view           >
    : trait_refs                    <T> {
  /** type */
  using t       =       ::array_view<T>                                         ;

  /** const type */
  using c       = const ::array_view<T>                                         ;
};

template<
    typename                         T
>
struct trait_container              <T,         types::tags::vector             >
    : trait_refs<T> {
  using shared     = std::vector<
      typename trait_container      <T,         types::tags::pointer            >::shared
  >;
  using weak       = std::vector<
      typename trait_container      <T,         types::tags::pointer            >::weak
  >;
  using raw        = std::vector<
      typename trait_container      <T,         types::tags::pointer            >::raw
  >;
  using value      = std::vector<
      typename trait_container      <T,         types::tags::basic              >::type
  >;

  using c    = trait_container      <T,   const types::tags::vector             >;
};

template<
    typename                         T
>
struct trait_container              <T,         types::tags::list               >
    : trait_refs                    <T> {
  using shared     = std::list<
      typename trait_container      <T,         types::tags::pointer            >::shared
  >;
  using weak       = std::list<
      typename trait_container      <T,         types::tags::pointer            >::weak
  >;
  using value      = std::list<
      typename trait_container      <T,         types::tags::basic              >::type
  >;

  using c          = trait_container<T,   const types::tags::list               >;
};

template<
    typename                         T,
    typename                                                          KeyT
>
struct trait_container              <T,         types::tags::map,     KeyT      >
    : trait_refs                    <T> {
  using shared     = std::map<
                                                                      KeyT      ,
      typename trait_container      <T,         types::tags::pointer            >::shared
  >;
  using weak       = std::map<
                                                                      KeyT      ,
      typename trait_container      <T,         types::tags::pointer            >::weak
  >;
  using value      = std::map<
                                                                      KeyT      ,
      typename trait_container      <T,         types::tags::basic              >::type
  >;

  using c          = trait_container<T,   const types::tags::map                >;
};

namespace {
using namespace      rxcpp;
using namespace      rxcpp::subjects;

template<
    typename                         T
>
struct trait_container              <T,         types::tags::rx                 >
    : trait_refs<T> {

  using subsc      =    subscription                                            ;
  using subj       =         subject<T                                         >;
  using obs        =      observable<T                                         >;
  using subs       = typename   subj                                            ::subscriber_type;
  using shared_subj=         subject<
      typename trait_container      <T,         types::tags::pointer           >::shared
  >;
  using shared_obs =      observable<
      typename trait_container      <T,         types::tags::pointer           >::shared
  >;
  using shared_subs
    = typename           shared_subj                                            ::subscriber_type;
  using weak_subj  =         subject<
      typename trait_container      <T,         types::tags::pointer           >::weak
  >;
  using weak_obs   =      observable<
      typename trait_container      <T,         types::tags::pointer           >::weak
  >;
  using weak_subs
    = typename             weak_subj                                            ::subscriber_type;
};
}  // namespace


template<
    typename                         T
>
struct trait_container              <T,         types::tags::pointer           >
    : trait_refs<T> {
  using shared     = std::shared_ptr<T>;
  using weak       = std::weak_ptr  <T>;
  using raw        =                 T *;

  using c          = trait_container<T,   const types::tags::pointer           >;
};

template<
    typename                         T
>
struct trait_container              <T,         types::tags::basic              >
    : trait_refs                    <T> {
  using type       =                 T;
  using ref        =                 T&;

  using c          = trait_container<T,   const types::tags::basic              >;
};
// ~~~~~ Non-Const

// ~~~~~ Shortcuts
namespace traits {

template<
    typename                               T
>
using container    = typename trait_refs  <T>::               info;

template<
    typename                               T
>
using type         = typename container   <T>::               info;

template<
    typename                               T
>
using c_type       = typename container   <T>::               c_info;

template<
    typename                               T
>
using rx           = typename container   <T>::               rx;

template<
    typename                               T
>
using ptr          = typename container   <T>::               ptr;

template<
    typename                               T
>
using c_ptr        = typename container   <T>::               c_ptr;

template<
    typename                               T
>
using vector       = typename container   <T>::               vector;

template<
    typename                               T
>
using c_vector     = typename container   <T>::               c_vector;

template<
    typename                               T
>
using raw_arr      = typename container   <T>::               arr_view;

template<
    typename                               T
>
using c_raw_arr    = typename container   <T>::               c_raw_arr;

template<
    typename                               T
>
using list         = typename container   <T>::               list;

template<
    typename                               T
>
using c_list       = typename container   <T>::               c_list;

template<
    typename                               T,
    typename                                                  KeyT
>
using map          = typename container   <T>::template   map<KeyT>;

template<
    typename                               T,
    typename                                                  KeyT
>
using const_map    = typename container   <T>::template c_map<KeyT>;
}  // namespace traits
// ~~~~~ Shortcuts

}  // namespace types
#endif //BEFA_TYPES_HPP
