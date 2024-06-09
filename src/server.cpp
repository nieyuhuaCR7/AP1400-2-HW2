#include "server.h"
#include "client.h"
#include <memory>
#include <string>
#include <map>
#include <stdexcept>
#include <random>
#include <sstream>
#include <iomanip>

// Constructor
Server::Server() {}

std::vector<std::string> Server::pending_trxs;

std::shared_ptr<Client> Server::add_client(std::string id) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, 9999);
    bool id_exists = true;
    // Check if there is already an id in server's map
    while (id_exists) {
        id_exists = false;
        for (const auto& pair: clients) {
            if (pair.first->get_id() == id) {
                // found the existing client id, append four random digits to id
                int random_number = dis(gen);
                std::ostringstream oss;
                oss << std::setw(4) << std::setfill('0') << random_number;
                id += oss.str();
                id_exists = true;
                break;
            }
        }
    }   
    auto client = std::make_shared<Client>(id, *this);
    clients[client] = 5.0;
    return client;
}

std::shared_ptr<Client> Server::get_client(std::string id) const {
    for (const auto& pair: clients) {
        if (pair.first->get_id() == id) {
            return pair.first;
        }
    }
    std::cout << "Client not found for id: " << id << std::endl;
    return nullptr;
}

double Server::get_wallet(std::string id) const{
    for (const auto& pair: clients) {
        if (pair.first->get_id() == id) {
            return pair.second;
        }
    }
    throw std::logic_error("there is no such client");
}

bool Server::parse_trx(std::string trx, std::string& sender, std::string& receiver, double& value) {
    size_t first_dash = trx.find('-');
    size_t second_dash = trx.find('-', first_dash + 1);
    if (first_dash == std::string::npos || second_dash == std::string::npos || first_dash >= second_dash) {
        throw std::runtime_error("invalid format");  // Invalid format
    }
    sender = trx.substr(0, first_dash);
    receiver = trx.substr(first_dash + 1, second_dash - first_dash - 1);
    std::string value_str = trx.substr(second_dash + 1);

    if (sender.empty() || receiver.empty() || value_str.empty()) {
        return false;
    }

    try {
        value = std::stod(value_str);
    } catch (const std::invalid_argument& e) {
        return false;
    } catch (const std::out_of_range& e) {
        return false;
    }

    // Check the client id
    // if (get_client(sender) == nullptr || get_client(receiver) == nullptr) {
    //     return false;
    // }

    // clients[get_client(sender)] -= value;
    // clients[get_client(receiver)] += value;

    return true;
} 

bool Server::add_pending_trx(std::string trx, std::string signature) {
    std::string sender{};
    std::string receiver{};
    // std::cout << trx << std::endl;
    double value;
    if (!parse_trx(trx, sender, receiver, value)) {
        return false;
    }
    // std::cout << sender << std::endl;
    // std::cout << receiver << std::endl;
    std::shared_ptr<Client> senderPtr = get_client(sender);
    if (senderPtr == nullptr) {
        return false;
    }
    std::shared_ptr<Client> receiverPtr = get_client(receiver);
    if (receiverPtr == nullptr) {
        return false;
    }
    std::cout << "senderPtr is not nullptr" << std::endl;
    // std::cout << signature << std::endl;
    std::cout << "sender id" + senderPtr->get_id() << std::endl;
    
    if (crypto::verifySignature(senderPtr->get_publickey(), senderPtr->get_id(), signature) == false) {
        return false;
    }
    std::cout << "verifySignature Success" << std::endl;
    if (senderPtr->get_wallet() < value) {
        return false;
    }
    pending_trxs.push_back(trx);
    std::cout << "add_pending_trx is true" + trx << std::endl;
    return true;
}