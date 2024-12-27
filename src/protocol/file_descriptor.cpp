#include "kafka/protocol/file_descriptor.hpp"

#include <cerrno>
#include <stdexcept>
#include <system_error>

namespace kafka {

void FileDescriptor::read(void *dst, std::size_t nbytes) {
    char *p = static_cast<char *>(dst);
    while (nbytes > 0) {
        ssize_t nr = ::read(fd_, p, nbytes);
        if (nr < 0) {
            throw std::system_error(errno, std::system_category(), "read");
        } else if (nr == 0) {
            throw std::runtime_error("incomplete read");
        }
        p += nr;
        nbytes -= nr;
    }
}

void FileDescriptor::write(const void *src, std::size_t nbytes) {
    const char *p = static_cast<const char *>(src);
    while (nbytes > 0) {
        ssize_t nw = ::write(fd_, p, nbytes);
        if (nw < 0) {
            throw std::system_error(errno, std::system_category(), "write");
        }
        p += nw;
        nbytes -= nw;
    }
}

}
