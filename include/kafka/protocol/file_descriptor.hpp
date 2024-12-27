#ifndef CODECRAFTERS_KAFKA_PROTOCOL_FILE_DESCRIPTOR_HPP_INCLUDED
#define CODECRAFTERS_KAFKA_PROTOCOL_FILE_DESCRIPTOR_HPP_INCLUDED

#include <unistd.h>
#include <utility>

#include "kafka/protocol/ireadable.hpp"
#include "kafka/protocol/iwritable.hpp"

namespace kafka {

// Wrapper class of UNIX file descriptor.
class FileDescriptor : public IReadable, public IWritable {
public:
    explicit FileDescriptor(int fd) : fd_(fd) {}
    FileDescriptor(FileDescriptor &&other) noexcept : fd_(std::exchange(other.fd_, -1)) {}

    ~FileDescriptor() {
        if (fd_ >= 0) {
            close(fd_);
        }
    }

    FileDescriptor &operator=(FileDescriptor &&other) noexcept {
        std::swap(fd_, other.fd_);
        return *this;
    }

    // Reads a specified number of bytes from this file descriptor.
    void read(void *dst, std::size_t nbytes) override;

    // Writes a specified number of bytes to this file descriptor.
    void write(const void *src, std::size_t nbytes) override;

    FileDescriptor(const FileDescriptor &other) = delete;
    FileDescriptor &operator=(const FileDescriptor &other) = delete;

private:
    int fd_;
};

}

#endif  // CODECRAFTERS_KAFKA_PROTOCOL_FILE_DESCRIPTOR_HPP_INCLUDED
