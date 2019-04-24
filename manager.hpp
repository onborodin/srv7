
#ifndef HTTP_CONNECTION_MANAGER_HPP
#define HTTP_CONNECTION_MANAGER_HPP

#include <set>

#include "connection.hpp"

namespace srv6 {

class manager {
    private:
        std::set<std::shared_ptr<connection>> connections;
    public:
        manager(const manager&) = delete;
        manager& operator=(const manager&) = delete;

        manager();
        void start(std::shared_ptr<connection> c);
        void stop(std::shared_ptr<connection> c);
        void stop_all();
};

} // namespace srv6

#endif
