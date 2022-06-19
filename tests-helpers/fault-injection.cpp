#include "fault-injection.h"
#include <cassert>
#include <iostream>
#include <vector>

#include <sys/mman.h>

namespace {

template <typename T>
struct mmap_allocator {
  using value_type = T;

  mmap_allocator() = default;

  T* allocate(size_t n) {
    void* ptr = mmap(nullptr, n, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE, -1, 0);
    if (ptr == MAP_FAILED)
      throw std::bad_alloc();
    return reinterpret_cast<T*>(ptr);
  }

  void deallocate(void* p, std::size_t n) {
    int r = munmap(p, n);
    if (r != 0)
      std::abort();
  }
};

struct fault_injection_context {
  std::vector<size_t, mmap_allocator<size_t>> skip_ranges;
  size_t error_index = 0;
  size_t skip_index = 0;
  bool fault_registred = false;
};

thread_local bool disabled = false;
thread_local fault_injection_context* context = nullptr;

void dump_state() {
#if 0
        std::cout << "skip_ranges: {";
        if (!context->skip_ranges.empty())
        {
            std::cout << context->skip_ranges[0];
            for (size_t i = 1; i != context->skip_ranges.size(); ++i)
                std::cout << ", " << context->skip_ranges[i];
        }
        std::cout << "}\nerror_index: " << context->error_index << "\nskip_index: " << context->skip_index << '\n' << std::flush;
#endif
}

} // namespace

bool should_inject_fault() {
  if (!context)
    return false;

  if (disabled)
    return false;

  assert(context->error_index <= context->skip_ranges.size());
  if (context->error_index == context->skip_ranges.size()) {
    ++context->error_index;
    context->skip_ranges.push_back(0);
    context->fault_registred = true;
    return true;
  }

  assert(context->skip_index <= context->skip_ranges[context->error_index]);

  if (context->skip_index == context->skip_ranges[context->error_index]) {
    ++context->error_index;
    context->skip_index = 0;
    context->fault_registred = true;
    return true;
  }

  ++context->skip_index;
  return false;
}

void fault_injection_point() {
  if (should_inject_fault())
    throw injected_fault("injected fault");
}

void faulty_run(std::function<void()> const& f) {
  assert(!context);
  fault_injection_context ctx;
  context = &ctx;
  for (;;) {
    try {
      f();
    } catch (...) {
      fault_injection_disable dg;
      dump_state();
      ctx.skip_ranges.resize(ctx.error_index);
      ++ctx.skip_ranges.back();
      ctx.error_index = 0;
      ctx.skip_index = 0;
      assert(ctx.fault_registred);
      ctx.fault_registred = false;
      continue;
    }
    assert(!ctx.fault_registred);
    break;
  }
  context = nullptr;
}

fault_injection_disable::fault_injection_disable() : was_disabled(disabled) {
  disabled = true;
}

fault_injection_disable::~fault_injection_disable() {
  disabled = was_disabled;
}

void* operator new(std::size_t count) {
  if (should_inject_fault())
    throw std::bad_alloc();

  void* ptr = malloc(count);
  if (!ptr)
    throw std::bad_alloc();

  return ptr;
}

void* operator new[](std::size_t count) {
  if (should_inject_fault())
    throw std::bad_alloc();

  void* ptr = malloc(count);
  if (!ptr)
    throw std::bad_alloc();

  return ptr;
}

void operator delete(void* ptr) noexcept {
  free(ptr);
}

void operator delete[](void* ptr) noexcept {
  free(ptr);
}

void operator delete(void* ptr, std::size_t) noexcept {
  free(ptr);
}

void operator delete[](void* ptr, std::size_t) noexcept {
  free(ptr);
}
