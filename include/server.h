#ifndef SERVER_H
#define SERVER_H

#include "client.h"
#include <memory> // Include for std::shared_ptr
#include <string> // Include for std::string
#include <map>    // Include for std::map


class Client;

class Server
{
    public:
        Server();
        std::shared_ptr<Client> add_client(std::string id);
        std::shared_ptr<Client> get_client(std::string id) const;
        double get_wallet(std::string id) const;
        bool parse_trx(std::string trx, std::string& sender, std::string& receiver, double& value);
        bool add_pending_trx(std::string trx, std::string signature);
        size_t mine();
        friend void show_wallets(const Server& server);
        static std::vector<std::string> pending_trxs;
        
    private:
        std::map<std::shared_ptr<Client>, double> clients;
        
};

#endif //SERVER_H