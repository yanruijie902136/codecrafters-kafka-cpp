#ifndef codecrafters_kafka_requests_api_versions_request_hpp
#define codecrafters_kafka_requests_api_versions_request_hpp

#include <string>

#include "kafka/protocol/readable.hpp"
#include "kafka/requests/request.hpp"

namespace kafka {

// ApiVersions request version 4.
class ApiVersionsRequest : public Request {
private:
        std::string client_software_name_;
        std::string client_software_version_;

        void read_request_body(Readable &readable) override {
                client_software_name_ = read_compact_string(readable);
                client_software_version_ = read_compact_string(readable);
                read_tagged_fields(readable);
        }
};

}

#endif
