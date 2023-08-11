#pragma once

#include <cassert>
#include <cstdint>
#include <utility>

// simple lookup table with fixed size(no dynamic allocation)
template <typename K, typename V, uint16_t N> class LookupTable {
public:
  LookupTable() : m_count(0) {}

  void insert(K key, V value) {
    assert(m_count < N);

    if (get(key) == nullptr) {
      m_table[m_count++] = std::make_pair(key, value);
    }
  }

  V get(K key) {
    for (uint16_t i = 0; i < m_count; i++) {
      if (m_table[i].first == key) {
        return m_table[i].second;
      }
    }
    return nullptr;
  }

private:
  std::pair<K, V> m_table[N];
  uint16_t m_count;
};
