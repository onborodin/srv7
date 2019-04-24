
#ifndef HTTP_CONNECTION_MANAGER_HPP
#define HTTP_CONNECTION_MANAGER_HPP

#include <set>

#include "connection.hpp"

namespace server {

class manager {
    private:
        std::set<connection_ptr> connections;
    public:
        manager(const manager&) = delete;
        manager& operator=(const manager&) = delete;

        manager();
        void start(connection_ptr c);
        void stop(connection_ptr c);
        void stop_all();
};

} // namespace server

#endif
