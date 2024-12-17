#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

using boost::asio::ip::tcp;

const int max_length = 1024;

// ����� ��� ��������� ������ �������
class Session : public std::enable_shared_from_this<Session> {
public:
    // �����������, ����������� ����� �������
    Session(tcp::socket socket) : socket_(std::move(socket)) {}

    void start() {
        do_read();  // �������� ������ ������ �� �������
    }

    // ��������� ������, ����� ���������� ��� � `async_accept`
    tcp::socket& socket() {
        return socket_;
    }

private:
    void do_read() {
        auto self(shared_from_this());  // ������� shared_ptr ��� �������� �������
        socket_.async_read_some(
            boost::asio::buffer(data_, max_length),
            [this, self](boost::system::error_code error, std::size_t length) {
                if (!error) {
                    std::cout << "Received message: " << std::string(data_, length) << "\n";
                    do_write(length);  // ���������� ������ ������� �������
                }
                else {
                    std::cerr << "Error reading data: " << error.message() << "\n";
                }
            });
    }

    void do_write(std::size_t length) {
        auto self(shared_from_this());
        boost::asio::async_write(
            socket_, boost::asio::buffer(data_, length),
            [this, self](boost::system::error_code error, std::size_t /*length*/) {
                if (!error) {
                    std::cout << "Message sent back to client.\n";
                }
                else {
                    std::cerr << "Error sending data: " << error.message() << "\n";
                }
            });
    }

    tcp::socket socket_;  // ����� ��� ������� � ��������
    char data_[max_length];  // ����� ��� �������� ������
};

// ����� ��� ������ � ��������
class Server {
public:
    // �����������, ��������� ������, ��������� ��������� ����
    Server(boost::asio::io_service& io_service, short port)
        : acceptor_(io_service, tcp::endpoint(tcp::v4(), port)) {
        start_accept();  // ��������� ����� �����������
    }

private:
    void start_accept() {
        // ������� ����� ������ ������ ��� ������ �������
        auto new_session = std::make_shared<Session>(tcp::socket(acceptor_.get_executor()));

        // ���������� ��������� ����������� �� �������
        acceptor_.async_accept(new_session->socket(),
            [this, new_session](boost::system::error_code error) {
                if (!error) {
                    new_session->start();  // �������� ������ � ��������
                }
                else {
                    std::cerr << "Error accepting connection: " << error.message() << "\n";
                }
                start_accept();  // ���������� ������� ����� �����������
            });
    }

    tcp::acceptor acceptor_;  // �������� ��� ������������� �������� �����������
};

int main() {
    try {
        boost::asio::io_service io_service;
        short port = 60000;  // ����, �� ������� ������ ����� �������
        Server server(io_service, port);  // ��������� ������ �� ��������� �����
        std::cout << "Server is running on port " << port << ". Waiting for clients...\n";

        io_service.run();  // ��������� ��������� �������
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
