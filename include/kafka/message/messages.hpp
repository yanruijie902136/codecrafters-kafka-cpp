#ifndef CODECRAFTERS_KAFKA_MESSAGE_MESSAGES_HPP_INCLUDED
#define CODECRAFTERS_KAFKA_MESSAGE_MESSAGES_HPP_INCLUDED

#include <memory>
#include <utility>

#include "kafka/message/abstract.hpp"
#include "kafka/message/api_versions.hpp"
#include "kafka/message/describe_topic_partitions.hpp"
#include "kafka/message/fetch.hpp"
#include "kafka/message/headers.hpp"
#include "kafka/protocol/constants.hpp"
#include "kafka/protocol/ireadable.hpp"
#include "kafka/protocol/iwritable.hpp"
#include "kafka/protocol/readable_buffer.hpp"
#include "kafka/protocol/writable_buffer.hpp"

namespace kafka {

class RequestMessage {
public:
    // Reads this `RequestMessage` (including the size prefix) from a byte stream.
    void read(IReadable &readable) {
        ReadableBuffer rb(read_bytes(readable));
        header_.read(rb);
        switch (header_.request_api_key()) {
            case ApiKey::FETCH:
                request_ = std::make_unique<FetchRequest>();
                break;
            case ApiKey::API_VERSIONS:
                request_ = std::make_unique<ApiVersionsRequest>();
                break;
            case ApiKey::DESCRIBE_TOPIC_PARTITIONS:
                request_ = std::make_unique<DescribeTopicPartitionsRequest>();
                break;
        }
        request_->read(rb);
    }

    const RequestHeader &header() const {
        return header_;
    }

    template<class ConcreteRequest>
    const ConcreteRequest *request() const {
        return dynamic_cast<ConcreteRequest *>(request_.get());
    }

private:
    RequestHeader header_;
    std::unique_ptr<AbstractRequest> request_;
};

class ResponseMessage {
public:
    ResponseMessage(ResponseHeader header, std::unique_ptr<AbstractResponse> response)
        : header_(std::move(header)), response_(std::move(response)) {}

    // Writes this `ResponseMessage` (including the size prefix) to a byte stream.
    void write(IWritable &writable) const {
        WritableBuffer wb;
        short header_version = response_->api_key() == ApiKey::API_VERSIONS ? 0 : 1;
        header_.write(wb, header_version);
        response_->write(wb);
        write_bytes(writable, wb.buffer());
    }

private:
    ResponseHeader header_;
    std::unique_ptr<AbstractResponse> response_;
};

}

#endif  // CODECRAFTERS_KAFKA_MESSAGE_MESSAGES_HPP_INCLUDED
