#include "kafka/protocol/file_descriptor.hpp"

#include <unistd.h>

namespace kafka {

void FileDescriptor::read(void *dst, std::size_t nbytes) {
    char *p = static_cast<char *>(dst);
    while (nbytes > 0) {
        ssize_t nr = ::read(fd_, p, nbytes);
        if (nr < 0) {
            throw_system_error("read");
        } else if (nr == 0) {
            throw_runtime_error("incomplete read");
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
            throw_system_error("write");
        }
        p += nw;
        nbytes -= nw;
    }
}

}
