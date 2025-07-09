# Virtual Memory Simulator
This project implements a user-space demand-paged virtual memory system, written in C++. It handles page faults, manages a virtual page table and disk, and implements multiple page replacement algorithms — including a custom LRU strategy. The system simulates how operating systems manage memory behind the scenes.

## What It Does
- Simulates virtual memory with on-demand paging.
- Handles page faults via signal interception (SIGSEGV).
- Implements and compares three page replacement algorithms:
    - FIFO (First In, First Out)
    - LRU (Least Recently Used)
    - Custom LRU with a linked list for efficient page replacement.
- Collects execution statistics: number of page faults, disk reads, and disk writes.
- Runs with four different memory-intensive programs: `alpha`, `beta`, `gamma`, `delta`.

## How to Use
1. **Compile the Code**: Use `make` to compile the project.
2. **Run the Simulator**: Execute the binary with the desired page replacement algorithm and the program to run. Use the following command format:
   ```bash
   ./virtmem <n_pages> <n_frames> <replacement_algo> <program>
   - `n_pages`: Number of virtual memory pages (e.g., 100)
   - `n_frames`: Number of physical memory frames (e.g., 10)
   - `replacement_algo`: One of rand, fifo, or custom
   - `program`: One of alpha, beta, gamma, or delta

### Output example
```
Page faults: 2040
Disk reads: 1020
Disk writes: 1020
```

## Real-World Applications
- Operating System Design: This project mirrors the foundational ideas behind Linux memory management.
- Systems Programming: Understanding how signals and memory access violations can be intercepted and handled.
- Performance Tuning: Demonstrates how access patterns affect memory system performance.
- Embedded Systems: Useful for designing lightweight memory managers in environments without native OS memory paging.