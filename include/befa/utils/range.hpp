//
// Created by miro on 12/20/16.
//

#ifndef BEFA_RANGE_HPP
#define BEFA_RANGE_HPP

#include <vector>

namespace details {
template<typename IteratorT>
struct range {
  using iterator_type = IteratorT;
  using size_type = typename std::iterator_traits<iterator_type>::difference_type;

  range(iterator_type begin, iterator_type end)
      : _b(begin), _e(end) {}

  iterator_type begin() const { return _b; }
  iterator_type end() const { return _e; }
  size_type size() const { return std::distance(_b, _e); }

 private:
  iterator_type _b, _e;
};
}  // namespace details

template<typename IteT>
details::range<IteT> range(IteT begin, IteT end) {
  return details::range<IteT>(begin, end);
}

#endif //BEFA_RANGE_HPP
