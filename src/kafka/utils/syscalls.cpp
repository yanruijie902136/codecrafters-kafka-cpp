#include "kafka/utils/syscalls.hpp"

#include <cstdlib>
#include <err.h>
#include <stdexcept>
#include <unistd.h>

namespace kafka {

void fullread(int fildes, void *buffer, std::size_t size) {
        unsigned char *p = static_cast<unsigned char *>(buffer);
        while (size > 0) {
                ssize_t nr = read(fildes, p, size);
                if (nr < 0) {
                        err(EXIT_FAILURE, "read");
                } else if (nr == 0) {
                        throw std::runtime_error("incomplete read");
                }
                p += nr;
                size -= nr;
        }
}

void fullwrite(int fildes, const void *buffer, std::size_t size) {
        const unsigned char *p = static_cast<const unsigned char *>(buffer);
        while (size > 0) {
                ssize_t nw = write(fildes, p, size);
                if (nw < 0) {
                        err(EXIT_FAILURE, "write");
                }
                p += nw;
                size -= nw;
        }
}

}
