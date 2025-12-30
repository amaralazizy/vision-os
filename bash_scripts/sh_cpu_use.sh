#!/usr/bin/env bash

# Configuration
REFRESH_RATE=2

# Store previous CPU stats
PREV_TOTAL=0
PREV_IDLE=0

# Function 0: Get total CPU usage
get_cpu_usage() {
    read cpu user nice system idle iowait irq softirq steal < /proc/stat

    local idle_time=$((idle + iowait))
    local total_time=$((user + nice + system + idle + iowait + irq + softirq + steal))

    local total_diff=$((total_time - PREV_TOTAL))
    local idle_diff=$((idle_time - PREV_IDLE))

    local cpu_usage=0
    if (( total_diff > 0 )); then
        cpu_usage=$(( (100 * (total_diff - idle_diff)) / total_diff ))
    fi

    PREV_TOTAL=$total_time
    PREV_IDLE=$idle_time

    echo "$cpu_usage"
}

# Function 1: Logic to get process data
get_process_data() {
    local clk_tck
    clk_tck=$(getconf CLK_TCK)

    echo "PID      COMMAND              CPU%"
    echo "-----------------------------------"

    for pid in /proc/[0-9]*; do
        pid=${pid##*/}

        if [[ -r /proc/$pid/stat ]]; then
            read -r _ comm _ _ _ _ _ _ _ _ _ _ utime stime _ < /proc/$pid/stat

            total_time=$((utime + stime))
            cpu_percent=$((total_time / clk_tck))

            printf "%-8s %-20s %-8s\n" "$pid" "${comm//[\(\)]/}" "$cpu_percent"
        fi
    done | head -n 20
}




# Function 2: Logic to format and print the UI
print_dashboard() {
    local timestamp=$(date +%H:%M:%S)
    local cpu_usage=$(get_cpu_usage)

    clear
    echo "==============================================================="
    echo "  VisionOS System Monitor                  $timestamp"
    echo "  Total CPU Usage: ${cpu_usage}%"
    echo "  Monitoring All Processes (Top 20 by CPU)"
    echo "==============================================================="

    get_process_data | awk '
        NR==1 { printf "%-8s %-20s %-8s %-8s %-10s\n", "PID", "COMMAND", "CPU%", "MEM%", "TIME"; next }
        { printf "%-8s %-20s %-8.1f %-8.1f %-10s\n", $1, $2, $3, $4, $5 }
    '

    echo "---------------------------------------------------------------"
    echo "[CTRL+C to Exit] | Refresh: ${REFRESH_RATE}s"
}

# Main Execution Loop
run_monitor() {
    while true; do
        print_dashboard
        sleep "$REFRESH_RATE"
    done
}

# Start the script
run_monitor
