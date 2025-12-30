#!/usr/bin/env bash

# Configuration
REFRESH_RATE=2

# Function: Fetch NVIDIA GPU stats
get_nvidia_stats() {
    nvidia-smi \
        --query-gpu=index,name,utilization.gpu,memory.used,memory.total \
        --format=csv,noheader,nounits | awk -F', ' '{
        printf "%-5s %-20s %-10s %-15s\n", $1, $2, $3"%", $4"MB / "$5"MB"
    }'
}

# Function: Fetch Intel GPU stats (sysfs-based)
get_intel_stats() {
    local gpu_busy_file="/sys/class/drm/card0/device/gpu_busy_percent"

    if [[ -f "$gpu_busy_file" ]]; then
        local usage
        usage=$(cat "$gpu_busy_file")

        printf "%-5s %-20s %-10s\n" "0" "Intel Integrated GPU" "${usage}%"
    else
        echo "Intel GPU detected, but usage data not available."
        echo "Kernel/driver may not expose gpu_busy_percent."
    fi
}

# Function: Presentation Layer
print_gpu_dashboard() {
    clear
    echo "==============================================================="
    echo "  VisionOS GPU Monitor                      $(date +%H:%M:%S)"
    echo "==============================================================="

    if command -v nvidia-smi >/dev/null; then
        printf "%-5s %-20s %-10s %-15s\n" "ID" "MODEL" "LOAD" "VRAM USAGE"
        echo "---------------------------------------------------------------"
        get_nvidia_stats

    elif [[ -d "/sys/class/drm/card0/device" ]]; then
        printf "%-5s %-20s %-10s\n" "ID" "MODEL" "LOAD"
        echo "---------------------------------------------------------------"
        get_intel_stats

    else
        echo "Error: No supported GPU detected (NVIDIA / Intel)."
    fi

    echo "---------------------------------------------------------------"
    echo "[CTRL+C to Exit] | Refresh: ${REFRESH_RATE}s"
}

# Main Loop
run_gpu_monitor() {
    while true; do
        print_gpu_dashboard
        sleep "$REFRESH_RATE"
    done
}

run_gpu_monitor
