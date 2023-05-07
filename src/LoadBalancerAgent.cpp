#include "LoadBalancerAgent.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fstream>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <spdlog/spdlog.h>

#include <iostream>

LoadBalancerAgent::LoadBalancerAgent()
{
    std::ifstream configFile("./config.json");
    if (configFile.is_open()) {
        nlohmann::json config;
        configFile >> config;
        for (auto& [key, value] : config.items()) {
            if (m_configHandlers.find(key) != m_configHandlers.end())
                (this->*m_configHandlers[key])(value);
        }
        configFile.close();
    }

    for (auto& server : m_servers) {
        int socket = ::socket(AF_INET, SOCK_STREAM, 0);
        if (socket == -1) {
            throw std::runtime_error("socket() failed: " + std::string(strerror(errno)));
            continue;
        }
        if (::connect(socket, (struct sockaddr *)&server, sizeof(server)) < 0) {
            throw std::runtime_error("connect() failed: " + std::string(strerror(errno)));
            continue;
        }
        m_sockets.push_back(socket);
    }

    if (m_sockets.empty())
        throw std::runtime_error("No servers to connect to!");
}

LoadBalancerAgent::~LoadBalancerAgent()
{
    for (auto& socket : m_sockets)
        ::close(socket);
}

void LoadBalancerAgent::run()
{
    while (true) {
        getProcStat();
        if (m_cpuUsage > 80) {
            SPDLOG_ERROR("CPU usage is too high: {}", m_cpuUsage);
        } else if (m_cpuUsage > 40) {
            SPDLOG_WARN("CPU usage is high: {}", m_cpuUsage);
        } else if (m_cpuUsage > 20) {
            SPDLOG_INFO("CPU usage is normal: {}", m_cpuUsage);
        }
        std::string message = "LOAD_BALANCER_AGENT\n";
        message += m_associatedHost + ":" + m_associatedPort + "\n";
        message += std::to_string(m_cpuUsage) + "\n";
        for (auto& socket : m_sockets)
            if (::send(socket, message.c_str(), message.length(), 0) < 0)
                throw std::runtime_error("send() failed: " + std::string(strerror(errno)));
        std::this_thread::sleep_for(std::chrono::milliseconds(m_timePeriod));
    }
}

void LoadBalancerAgent::getProcStat() {
    std::ifstream stat_file("/proc/stat");
    std::string line;

    getline(stat_file, line);
    if (line.substr(0, 3) == "cpu") {
        std::istringstream iss(line);
        std::vector<std::string> tokens;
        std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), std::back_inserter(tokens));
        
        std::string cpu_name = tokens[0];
        int user = stoi(tokens[1]);
        int nice = stoi(tokens[2]);
        int system = stoi(tokens[3]);
        int idle = stoi(tokens[4]);
        int iowait = stoi(tokens[5]);
        int irq = stoi(tokens[6]);
        int softirq = stoi(tokens[7]);
        int steal = stoi(tokens[8]);
        int guest = stoi(tokens[9]);

        m_cpuActivePrev = m_cpuActive;
        m_cpuTotalPrev = m_cpuTotal;

        m_cpuActive = user + nice + system + irq + softirq + steal;
        m_cpuTotal = user + nice + system + idle + iowait + irq + softirq + steal + guest;

        m_cpuUsage = (m_cpuActive - m_cpuActivePrev) / (m_cpuTotal - m_cpuTotalPrev) * 100;
    }

    stat_file.close();
}