
#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include <asio.hpp>
#include <string>

#include "connection.hpp"
#include "connection_manager.hpp"
#include "request_handler.hpp"

namespace server {

class server {
  private:
    asio::io_context io_context;
    asio::signal_set signals;
    asio::ip::tcp::acceptor acceptor;

    connection_manager connection_manager;
    request_handler request_handler;

    void do_accept();
    void do_await_stop();

  public:
    server(const server&) = delete;
    server& operator=(const server&) = delete;

    explicit server(const std::string& address, const std::string& port, const std::string& doc_root);
    void run();

};

} // namespace server

#endif
