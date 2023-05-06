# LoadBalancerAgent

## Description

This is a simple load balancer agent that can be used to distribute load across multiple servers. It is written in C++. It simply computes the CPU usage and send it periodically to the load balancer server. The load balancer server then decides which server to send the request to based on the CPU usage of the servers.
