#include "server.h"
#include "client.h"
#include <memory>
#include <string>
#include <map>
#include <stdexcept>
#include <random>
#include <sstream>
#include <iomanip>
#include "crypto.h"

// Constructor
Client::Client(std::string id, Server& server) 
    : id(id), server(&server) {   
        crypto::generate_key(public_key, private_key);
}

std::string Client::get_id() {
   return id; 
}

std::string Client::get_publickey() const {
    return public_key;
}

double Client::get_wallet() {
    if (server) {
        double result = server->get_wallet(id);
        return result;
    } else {
        throw std::runtime_error("Server pointer is null");
    }
}

std::string Client::sign(std::string txt) const {
    std::string signature = crypto::signMessage(private_key, txt);
    return signature;
}

bool Client::transfer_money(std::string receiver, double value) {
    std::string trx;
    std::string value_str = std::to_string(value);
    trx = id + "-" + receiver + "-" + value_str;
    std::string signature = sign(id);
    // std::cout << signature << std::endl;
    if (server->add_pending_trx(trx, signature) == false) {
        return false;
    }
    return true;
}