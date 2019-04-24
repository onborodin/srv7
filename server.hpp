
#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include <asio.hpp>
#include <string>

#include "connection.hpp"
#include "manager.hpp"

namespace server {

class server {
  private:
    asio::io_context io_context;
    asio::signal_set signals;
    asio::ip::tcp::acceptor acceptor;

    manager manager;

    void do_accept();
    void do_await_stop();

  public:
    server(const server&) = delete;
    server& operator=(const server&) = delete;

    explicit server(const std::string& address, const std::string& port);
    void run();

};

} // namespace server

#endif
