#ifndef CODECRAFTERS_KAFKA_UTILS_HPP_INCLUDED
#define CODECRAFTERS_KAFKA_UTILS_HPP_INCLUDED

#include <bit>
#include <cerrno>
#include <stdexcept>
#include <system_error>

// Converts an integer from network byte order (big-endian) to host byte order.
template<typename IntType>
inline IntType to_host_byte_order(IntType n) {
    return std::endian::native == std::endian::big ? n : std::byteswap(n);
}

// Converts an integer from host byte order to network byte order (big-endian).
template<typename IntType>
inline IntType to_network_byte_order(IntType n) {
    return std::endian::native == std::endian::big ? n : std::byteswap(n);
}

// Throws a `std::runtime_error` with given error message.
inline void throw_runtime_error(const char *error_message) {
    throw std::runtime_error(error_message);
}

// Throws a `std::system_error` with given error message.
inline void throw_system_error(const char *error_message) {
    throw std::system_error(errno, std::system_category(), error_message);
}

#endif  // CODECRAFTERS_KAFKA_UTILS_HPP_INCLUDED
