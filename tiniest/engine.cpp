#include <tiniest/engine.hpp>

es::Engine &es::Engine::operator=(es::Engine &original) {
  if (&original != this) {
    m_arena = original.m_arena;
    m_arena_size = original.m_arena_size;
    original.m_arena = nullptr;
    original.m_arena_size = 0U;
  }
}