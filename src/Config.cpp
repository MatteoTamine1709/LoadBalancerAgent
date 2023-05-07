#include "./LoadBalancerAgent.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>

void LoadBalancerAgent::handleConfigHost(nlohmann::json &value)
{
    if (value.is_string())
        m_host = value;
}

void LoadBalancerAgent::handleConfigPort(nlohmann::json &value)
{
    if (value.is_string())
        m_port = value;
}

void LoadBalancerAgent::handleConfigTimePeriod(nlohmann::json &value)
{
    if (value.is_number_unsigned())
        m_timePeriod = value;
}

void LoadBalancerAgent::handleConfigServers(nlohmann::json &value)
{
    if (value.is_array()) {
        for (auto& server : value) {
            if (server.is_object()) {
                std::string host;
                int port;
                if (server.contains("host") && server["host"].is_string())
                    host = server["host"];
                else
                    host = m_host;
                if (server.contains("port") && server["port"].is_number())
                    port = server["port"];
                else
                    port = std::stoi(m_port);
                struct sockaddr_in serverAddress;
                serverAddress.sin_family = AF_INET;
                serverAddress.sin_addr.s_addr = inet_addr(host.c_str());
                serverAddress.sin_port = htons(port);
                std::cout << "Server: " << host << ":" << port << std::endl;
                m_servers.push_back(serverAddress);
            }
        }
    }
}

void LoadBalancerAgent::handleConfigAssociatedServer(nlohmann::json &value)
{
    if (value.is_object()) {
        if (value.contains("host") && value["host"].is_string())
            m_associatedHost = value["host"];
        if (value.contains("port") && value["port"].is_number())
            m_associatedPort = to_string(value["port"]);
    }
}
