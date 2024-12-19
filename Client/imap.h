#pragma once
#ifndef IMAP_H
#define IMAP_H

#include <iostream>
#include <fstream>
#include <string>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

const int max_length = 1024;

class IMAPClient {
public:
    // �����������
    IMAPClient();

    // ����� ��� �������������� ������������
    bool authenticate(const std::string& email, const std::string& password);

    // ����� ��� �������� ��������� �� ������
    void send_message_to_server(const std::string& message, const std::string& recipient_email);

    // ����� ��� ���������� ��������� � ����
    void save_message_to_file(const std::string& recipient_email, const std::string& sender_email, const std::string& message);

private:
    boost::asio::io_service io_service_; // Boost IO service ��� �������� ������
};

#endif // IMAP_H
