# NetworkMonitor – A Linux-Based Network Monitoring System

## Overview

**NetworkMonitor** is a robust, Linux-based system designed to monitor real-time metrics for multiple network interfaces. Built with performance, resilience, and automation in mind, it tracks the status, throughput, and error rates of interfaces, providing essential insights for diagnostics and analysis.

The system features inter-process communication via Unix domain sockets and supports graceful shutdowns through signal handling. It also implements self-healing mechanisms by detecting and recovering from interface downtimes automatically.

## Features

### Multi-Interface Monitoring
- Monitors multiple interfaces concurrently.
- Tracks:
  - Interface state (up/down)
  - Carrier transitions (up/down count)
  - Bytes transmitted/received
  - Error, dropped, and successful packet counts

### Socket-Based Communication
- Uses **Unix domain sockets** for communication between:
  - `networkMonitor`: Main controller
  - `intfMonitor`: One child process per interface
- Ensures synchronized message passing for event reporting and control.

### Resilient & Self-Healing Design
- Automatically attempts to bring downed interfaces back up.
- Reduces manual intervention, increasing uptime.

### Graceful Termination with Signal Handling
- Handles `SIGINT` (`Ctrl+C`) for smooth shutdown:
  - Notifies child processes to exit
  - Cleans up socket files
  - Frees all resources gracefully

### Real-Time Metric Reporting
- Each child monitor polls data **once per second**
- Live terminal output provides real-time insights into interface behavior

## Makefile Automation

A custom `Makefile` simplifies the development and execution process:

| Command        | Description |
|----------------|-------------|
| `make`         | Compiles all project files |
| `make debug`   | Builds with debugging enabled (`-DDEBUG`) |
| `make run`     | Compiles and runs the monitor using `sudo` |
| `make clean`   | Deletes binaries and object files |

## How to Run

### Requirements
- Linux system
- `g++` compiler
- `sudo` privileges (to control interfaces)

### Build the Project
```bash
make
```

### Run the Network Monitor
```bash
sudo ./networkMonitor
```

### Input Prompt
```text
Enter number of interfaces: 2
Enter interface name 1: lo
Enter interface name 2: ens33
```

### Simulate Interface Failure
To test auto-recovery and error handling:
```bash
sudo ip link set ens33 down
```

## Results

During testing, the system demonstrated:

- **Reliable real-time monitoring** of multiple interfaces
- **Accurate polling and statistical tracking**
- **Robust auto-recovery** from interface downtime
- **Clean shutdown with no resource leaks**
