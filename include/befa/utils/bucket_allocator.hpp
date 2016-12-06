////
//// Created by miro on 12/4/16.
////
//
//#ifndef BEFA_BUCKET_ALLOCATOR_HPP
//#define BEFA_BUCKET_ALLOCATOR_HPP
//
//#include <cstddef>
//#include <cstdint>
//#include <type_traits>
//#include <array>
//#include <algorithm>
//#include <assert.h>
//#include <map>
//#include <stdlib.h>
//#include <malloc.h>
//#include <memory>
//#include <vector>
//
//namespace befa {
//using address_type = uint64_t;
//
//template<size_t Align = 32>
//struct Allocator {
//  using type = uint8_t;
//  using pointer = type *;
//  static constexpr size_t align = Align;
///*
//  virtual pointer allocate() = 0;
//
//  virtual bool owns(pointer) = 0;
//
//  virtual void destroy(pointer) = 0;
//
//  virtual size_t size() const = 0;
//  */
//};
//
//template<size_t Size, size_t Align = 32>
//struct StackAllocator
//    : public Allocator<Align> {
//  using pointer = typename befa::Allocator<Align>::pointer;
//  using type = typename befa::Allocator<Align>::type;
//
//  StackAllocator() { alloc_d.fill(false); }
//
//  pointer allocate(size_t) {
//    auto first_available = std::find(alloc_d.begin(), alloc_d.end(), false);
//    if (first_available == alloc_d.end())
//      return nullptr;
//    auto index = std::distance(alloc_d.begin(), first_available);
//    alloc_d[index] = true;
//    ++count;
//    return (
//        data + (index * size_t)
//    );
//  }
//
//  bool owns(pointer pointer) {
//    return (uint8_t *)pointer >= data &&
//        (uint8_t *)pointer < (data + (Size * sizeof(T)));
//  }
//
//  void destroy(pointer pointer) {
//    alloc_d[((uint8_t *) pointer - data) / sizeof(T)] = false;
//    --count;
//  }
//
//  size_t size() const {
//    return count;
//  }
//
// private:
//  uint8_t data[Size * sizeof(type)];
//  std::array<bool, Size> alloc_d;
//  size_t count = 0;
//};
//
//template<typename T, size_t BlockSize = 32, size_t Align = 32>
//struct Mallocator
//    : public Allocator<T, Align> {
//  using pointer = typename befa::Allocator<T, Align>::pointer;
//  using type = typename befa::Allocator<T, Align>::type;
//  using data_type = std::unique_ptr<uint8_t, std::function<void(uint8_t *)>>;
//
//  pointer allocate() {
//    auto first_empty = std::find(indexes.begin(), indexes.end(), false);
//    if (first_empty == indexes.end()) {
//      data.emplace_back(
//          (uint8_t *) malloc(BlockSize * sizeof(T)),
//          [](uint8_t *ptr) { free(ptr); }
//      );
//      fill_indexes(BlockSize, false);
//      return allocate();
//    }
//    auto index = std::distance(indexes.begin(), first_empty);
//    *first_empty = true;
//    return (T *)(
//        data[index / BlockSize].get() + (index % BlockSize) * sizeof(T)
//    );
//  }
//
//  bool owns(pointer pointer) {
//    return std::find_if(
//        data.cbegin(), data.cend(),
//        [&pointer](const data_type &ptr_u) {
//          auto ptr = ptr_u.get();
//          return (ptr <= (uint8_t *) pointer) &&
//              ((uint8_t *) pointer < (ptr + (BlockSize * sizeof(T))));
//        }) != data.cend();
//  }
//
//  void destroy(pointer pointer) {
//    auto data_ite = std::find_if(
//        data.begin(), data.end(),
//        [&pointer](const data_type &ptr_u) {
//          auto ptr = ptr_u.get();
//          return (ptr <= (uint8_t *) pointer) &&
//              ((uint8_t *) pointer < (ptr + (BlockSize * sizeof(T))));
//        });
//    indexes[(std::distance(data.begin(), data_ite) * BlockSize) +
//        ((uint8_t *) pointer - data_ite->get()) / sizeof(T)] = false;
//  }
//
//  size_t size() const {
//    size_t count = 0;
//    for (const auto &index : indexes)
//      if (index) ++count;
//    return count;
//  }
//
// private:
//  void fill_indexes(size_t size, bool value) {
//    indexes.resize(indexes.size() + size, value);
//  }
//
//  std::vector<data_type> data;
//  std::vector<bool> indexes;
//};
//
//template<typename MainA, typename AltA>
//struct AltAllocator
//    : public Allocator<
//        typename MainA::type,
//        MainA::align>,
//      public MainA, public AltA {
//  using type = typename MainA::type;
//  using pointer = typename MainA::pointer;
//  static constexpr size_t align = MainA::align;
//
//  using main_allocator_type = MainA;
//  using alt_allocator_type = typename std::enable_if<
//      std::is_same<
//          typename MainA::type,
//          typename AltA::type
//      >::value, AltA
//  >::type;
//
//  pointer allocate() {
//    if (auto *mem = MainA::allocate())
//      return mem;
//    else
//      return AltA::allocate();
//  }
//
//  template<typename ...Args>
//  void construct(type *ptr, Args &&...args) {
//    new(ptr) type(std::forward<Args>(args)...);
//  }
//
//  template<typename ...Args>
//  void destruct(type *ptr, Args &&...args) {
//    ptr->~type();
//  }
//
//  bool owns(pointer pointer) {
//    return MainA::owns(pointer) || AltA::owns(pointer);
//  }
//
//  void destroy(pointer pointer) {
//    if (MainA::owns(pointer))
//      MainA::destroy(pointer);
//    else
//      AltA::destroy(pointer);
//  }
//
//  size_t size() const {
//    return MainA::size() + AltA::size();
//  }
//};
//}
//
//namespace xtd {
//template<typename T>
//static befa::AltAllocator<
//    befa::StackAllocator<T, 256>,
//    befa::Mallocator<T, 256>
//> global_allocator;
//
//template<typename T, typename ...Args>
//std::shared_ptr<T> make_shared(Args &&...args) {
//  auto allocated = (T *) global_allocator<T>.allocate();
//  assert(allocated);
//  new (allocated) T(std::forward<Args>(args)...);
//  return std::shared_ptr<T>(
//      allocated,
//      [](T *ptr) {
//        ptr->~T();
//        global_allocator<T>.destroy(ptr);
//      }
//  );
//}
//}
//
//#endif //BEFA_BUCKET_ALLOCATOR_HPP
