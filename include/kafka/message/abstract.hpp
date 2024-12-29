#ifndef CODECRAFTERS_KAFKA_MESSAGE_ABSTRACT_HPP_INCLUDED
#define CODECRAFTERS_KAFKA_MESSAGE_ABSTRACT_HPP_INCLUDED

#include "kafka/protocol/constants.hpp"
#include "kafka/protocol/ireadable.hpp"
#include "kafka/protocol/iwritable.hpp"

namespace kafka {

class AbstractRequest {
public:
    virtual ~AbstractRequest() = default;

    // Reads this request from a byte stream.
    virtual void read(IReadable &readable) = 0;
};

class AbstractResponse {
public:
    virtual ~AbstractResponse() = default;

    // The API key of this response.
    virtual constexpr ApiKey api_key() const = 0;

    // Writes this response to a byte stream.
    virtual void write(IWritable &writable) const = 0;
};

}

#endif  // CODECRAFTERS_KAFKA_MESSAGE_ABSTRACT_HPP_INCLUDED
