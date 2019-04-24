
#ifndef HTTP_CONNECTION_MANAGER_HPP
#define HTTP_CONNECTION_MANAGER_HPP

#include <set>

#include "connection.hpp"

namespace server {

class connection_manager {
    private:
        std::set<connection_ptr> connections;
    public:
        connection_manager(const connection_manager&) = delete;
        connection_manager& operator=(const connection_manager&) = delete;

        connection_manager();
        void start(connection_ptr c);
        void stop(connection_ptr c);
        void stop_all();
};

} // namespace server

#endif
