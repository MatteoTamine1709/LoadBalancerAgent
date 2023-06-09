#ifndef LOAD_BALANCER_AGENT_H
#define LOAD_BALANCER_AGENT_H

#include <nlohmann/json.hpp>

class LoadBalancerAgent
{
public:
    LoadBalancerAgent();
    ~LoadBalancerAgent();

    void run();

private:
    std::unordered_map<std::string, void (LoadBalancerAgent::*)(nlohmann::json &)> m_configHandlers = {
        {"host", &LoadBalancerAgent::handleConfigHost},
        {"port", &LoadBalancerAgent::handleConfigPort},
        {"timePeriod_ms", &LoadBalancerAgent::handleConfigTimePeriod},
        {"LB_servers", &LoadBalancerAgent::handleConfigServers},
        {"associatedServer", &LoadBalancerAgent::handleConfigAssociatedServer}
    };

    void handleConfigHost(nlohmann::json &value);
    void handleConfigPort(nlohmann::json &value);
    void handleConfigTimePeriod(nlohmann::json &value);
    void handleConfigServers(nlohmann::json &value);
    void handleConfigAssociatedServer(nlohmann::json &value);

    std::string m_host = "127.0.0.1";
    std::string m_port = "4000";
    std::string m_associatedHost = "127.0.0.1";
    std::string m_associatedPort = "3000";
    uint64_t m_timePeriod = 1000;
    std::vector<struct sockaddr_in> m_servers;
    std::vector<int> m_sockets;

    double m_cpuActivePrev = 0;
    double m_cpuTotalPrev = 0;
    double m_cpuActive = 0;
    double m_cpuTotal = 0;
    double m_cpuUsage = 0;
    void getProcStat();
};

#endif
