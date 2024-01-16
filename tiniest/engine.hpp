#ifndef TINIEST_ENGINE_HPP
#define TINIEST_ENGINE_HPP

#include <cstdlib>
#include <tiniest/lex.hpp>

namespace es {

typedef union {
} MemoryBlock;

class Engine {
private:
  MemoryBlock *m_arena;
  std::size_t m_arena_size;

public:
  Engine(void *arena, std::size_t arena_size)
      : m_arena(static_cast<MemoryBlock *>(arena)),
        m_arena_size(arena_size / sizeof(MemoryBlock)) {}
  Engine(const Engine &) = delete;
  Engine(Engine &&) = default;
  ~Engine() = default;
  Engine &operator=(Engine &original);
};

} // namespace es

#endif // TINIEST_ENGINE_HPP