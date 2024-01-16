#ifndef TINIEST_ENGINE_HPP
#define TINIEST_ENGINE_HPP

#include <cstdlib>
#include <optional>
#include <tiniest/lex.hpp>

namespace es {

class ObjectHeader {
private:
  std::uint32_t m_contents;

public:
  ObjectHeader() = default;
};

typedef union {
  ObjectHeader header;
} MemoryBlock;

class ObjectHandle {
private:
  ObjectHeader *m_header;
};

enum class error_t { OUT_OF_MEMORY };

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

  void execute(const char *code);
  std::optional<ObjectHandle> resolve_identifier(const char *identifier_name);
  error_t create_empty_object(ObjectHandle &object,
                              const char *identifier_name);
  error_t create_number(ObjectHandle &object, double value,
                        const char *identifier_name);
  error_t create_string(ObjectHandle &object, const char *value,
                        const char *identifier_name);
  error_t create_boolean(ObjectHandle &object, bool value,
                         const char *identifier_name);
  error_t create_null(ObjectHandle &object, const char *identifier_name);
  error_t create_undefined(ObjectHandle &object, const char *identifier_name);
  error_t create_function(ObjectHandle &object, const char *identifier_name);
};

} // namespace es

#endif // TINIEST_ENGINE_HPP