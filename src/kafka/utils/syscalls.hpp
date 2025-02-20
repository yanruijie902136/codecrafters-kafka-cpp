#ifndef codecrafters_kafka_utils_syscalls_hpp
#define codecrafters_kafka_utils_syscalls_hpp

#include <cstddef>

namespace kafka {

// Reads exactly `size` bytes from a file descriptor. Throws runtime error if there's not enough bytes.
void fullread(int fildes, void *buffer, std::size_t size);

// Writes exactly `size` bytes to a file descriptor.
void fullwrite(int fildes, const void *buffer, std::size_t size);

}

#endif
