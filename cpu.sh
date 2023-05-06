#!/bin/bash

while (true)
do
    read cpu user nice system idle iowait irq softirq steal guest < /proc/stat
    cpu_active_prev=$((user+system+nice+softirq+steal))
    cpu_total_prev=$((user+system+nice+softirq+steal+idle+iowait))

    sleep 0.05

    # Read /proc/stat file (for second datapoint)
    read cpu user nice system idle iowait irq softirq steal guest< /proc/stat

    # compute active and total utilizations
    cpu_active_cur=$((user+system+nice+softirq+steal))
    cpu_total_cur=$((user+system+nice+softirq+steal+idle+iowait))

    # compute CPU utilization (%)
    cpu_util=$((100*( cpu_active_cur-cpu_active_prev ) / (cpu_total_cur-cpu_total_prev) ))

    printf " Current CPU Utilization : %s\n" "$cpu_util"
done