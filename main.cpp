#include <iostream>
#include <array>
#include <set>
#include <memory>
#include "async.h"
#include <boost/asio/io_context.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

std::mutex session_mutex;

struct CfgConnect
{
    unsigned short Port = 0;
    std::size_t BulkSize = 0;
} cnt_Cfg;


constexpr auto BUFF_SIZE{ 4096 };

class session : public std::enable_shared_from_this<session>
{
public:
    session(std::shared_ptr<tcp::socket> socket, std::set<std::shared_ptr<session>>& client, std::size_t block_size)
        : socket_(socket), client_(client), block_size_(block_size), handle_{0} {}

    ~session() {
        async::disconnect(handle_);
    }

    void start_session()
    {        
        session_mutex.lock();
        client_.insert(shared_from_this());
        session_mutex.unlock();
        handle_ = async::connect(block_size_);
        buffer_ = std::make_shared<std::array<char, BUFF_SIZE>>();
        do_read();
    }

private:
    void do_read()
    {
        auto self(shared_from_this());
        socket_->async_read_some(boost::asio::buffer(buffer_->data(), BUFF_SIZE),
            [this, self](boost::system::error_code ec, std::size_t length)
            {
                if (!ec)
                {
                    async::receive(handle_, buffer_->data(), length);
                    do_read();
                }
                else
                {
                    async::disconnect(handle_);
                    session_mutex.lock();
                    client_.erase(shared_from_this());
                    session_mutex.unlock();
                }
            });
    }

    std::shared_ptr<tcp::socket> socket_;
    std::set<std::shared_ptr<session>>& client_;
    std::size_t block_size_;
    async::handle_t handle_;
    std::shared_ptr<std::array<char, BUFF_SIZE>> buffer_;
};

class server
{
public:
    server(boost::asio::io_context& io_context, const tcp::endpoint& endpoint, std::size_t size)
        : io_context(io_context), acceptor_(io_context, endpoint), bulk_size_(size)
    {
        socket_ = std::make_shared<tcp::socket>(io_context);
        do_accept();
    }

private:
    void do_accept()
    {
        acceptor_.async_accept(*socket_,
            [this](boost::system::error_code ec)
            {
                if (!ec)
                {
                    std::make_shared<session>(socket_, clients_, bulk_size_)->start_session();
                }
                socket_ = std::make_shared<tcp::socket>(io_context);
                do_accept();
            });
    }

    boost::asio::io_context& io_context;
    tcp::acceptor acceptor_;
    std::shared_ptr<tcp::socket> socket_;
    std::set<std::shared_ptr<session>> clients_;
    std::size_t bulk_size_;
};

int main(int argc, char* argv[]) {
    try
    {
        if (argc != 3)
        {            
            std::cerr << "Usage async_tcp_echo_bulk_server: " << argv[0] << " <port> <bulk_size>\n";
            return -1;
        }

        cnt_Cfg.Port = static_cast<unsigned short>(std::stoi(argv[1]));
        cnt_Cfg.BulkSize = static_cast<unsigned short>(std::stoi(argv[2]));

        boost::asio::io_context io_context;
        tcp::endpoint endpoint(tcp::v4(), cnt_Cfg.Port);
        server server(io_context, endpoint, cnt_Cfg.BulkSize);

        io_context.run();
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Exception: " << ex.what() << "\n";
    }

    return 0;
}
