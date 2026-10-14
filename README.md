# xv6 Lottery Scheduler

A ticket-based **Lottery Scheduler** implemented in the **xv6 RISC-V** teaching operating system.

This project replaces xv6's default round-robin scheduler with a probabilistic scheduling mechanism in which each runnable process owns a number of tickets. Processes with more tickets have a higher probability of being selected for CPU time.

---

## Project Overview

Lottery scheduling is a proportional-share CPU scheduling algorithm. During each scheduling decision:

1. The scheduler calculates the total number of tickets owned by all `RUNNABLE` processes.
2. A random winning ticket is generated.
3. The process that owns the winning ticket is selected.
4. The selected process is marked `RUNNING`.
5. Its scheduling round counter is incremented.
6. A context switch transfers CPU control to that process.

For example, if three processes have **10**, **5**, and **2** tickets, their long-term CPU scheduling frequency should gradually approach an approximate **10:5:2** ratio.

---

## Features

- Ticket-based probabilistic process scheduling
- Default ticket allocation for newly created processes
- Ticket inheritance across `fork()`
- Per-process scheduling round counter
- Custom `settickets()` system call
- Random winner selection using the assignment-provided pseudo-random generator
- Extended `procdump()` output for observing tickets and scheduling rounds
- Dedicated `test_scheduler` user program
- Single-CPU configuration for controlled scheduler testing

---

## Implementation Details

### 1. Process Information

Two fields were added to `struct proc`:

```c
int tickets;
int rounds;
```

- `tickets` stores the number of lottery tickets assigned to a process.
- `rounds` stores how many times that process has been selected by the scheduler.

Newly allocated processes start with:

```c
tickets = 10;
rounds = 0;
```

---

### 2. Ticket Inheritance

When a process is created using `fork()`, the child inherits the parent's ticket count while its scheduling history starts from zero:

```c
np->tickets = p->tickets;
np->rounds = 0;
```

---

### 3. `settickets()` System Call

A new system call was added:

```c
int settickets(int n);
```

It allows a user process to change its number of lottery tickets.

Only positive values are accepted.

Example:

```c
settickets(10);
```

The system call is connected through the normal xv6 syscall path:

```text
user/user.h
    ↓
user/usys.pl
    ↓
kernel/syscall.h
    ↓
kernel/syscall.c
    ↓
kernel/sysproc.c
```

---

### 4. Lottery Scheduler

The scheduler first calculates the total number of tickets owned by runnable processes.

A winner is then selected with:

```c
int winner = rand() % total;
```

The scheduler traverses the process table while accumulating ticket counts. When the accumulated count passes the winning ticket, that process is selected.

The selected process then:

```c
p->state = RUNNING;
p->rounds++;
```

before xv6 performs the context switch.

---

### 5. Process Statistics

`procdump()` was extended so that pressing `Ctrl + P` displays each process's ticket count and scheduling rounds.

Example:

```text
1 sleep  init tickets=10 rounds=37
2 sleep  sh tickets=10 rounds=17
4 run    test_scheduler tickets=10 rounds=121
6 runble test_scheduler tickets=5 rounds=60
8 runble test_scheduler tickets=2 rounds=29
```

The exact values vary because lottery scheduling is probabilistic, but the long-term scheduling frequency should reflect the ticket proportions.

---

## Test Program

The project includes:

```text
user/test_scheduler.c
```

Usage:

```bash
test_scheduler <number_of_tickets>
```

Example:

```bash
test_scheduler 10 &
test_scheduler 5 &
test_scheduler 2 &
```

After allowing the processes to run for a while, press:

```text
Ctrl + P
```

A successful test should show that the processes with more tickets generally accumulate more scheduling rounds.

For the example above, the expected long-term relationship is approximately:

```text
10 : 5 : 2
```

The values are not expected to be exact at every instant because each scheduler decision is random.

---

## Files Modified

The Lottery Scheduler implementation changes the following files:

```text
Makefile
kernel/proc.c
kernel/proc.h
kernel/syscall.c
kernel/syscall.h
kernel/sysproc.c
user/test_scheduler.c
user/user.h
user/usys.pl
```

### Main Responsibilities

| File | Purpose |
|---|---|
| `Makefile` | Adds `test_scheduler` and configures xv6 to use one CPU |
| `kernel/proc.h` | Adds `tickets` and `rounds` to the process structure |
| `kernel/proc.c` | Initializes scheduling fields, inherits tickets, implements RNG, lottery scheduling, and process statistics |
| `kernel/syscall.h` | Defines the `settickets` syscall number |
| `kernel/syscall.c` | Registers the `settickets` syscall |
| `kernel/sysproc.c` | Implements `sys_settickets()` |
| `user/user.h` | Exposes `settickets()` to user programs |
| `user/usys.pl` | Generates the user-space syscall stub |
| `user/test_scheduler.c` | Tests proportional lottery scheduling |

---

## Build and Run

### Requirements

A Linux environment with:

- RISC-V GNU toolchain
- QEMU with RISC-V support
- GNU Make
- Git

### Build xv6

```bash
make clean
make
```

### Run xv6

```bash
make qemu
```

This project uses:

```make
CPUS := 1
```

to make the lottery scheduling experiment easier to observe and evaluate.

---

## Quick Test

Inside the xv6 shell:

```bash
test_scheduler 10 &
test_scheduler 5 &
test_scheduler 2 &
```

Then press `Ctrl + P` multiple times.

You should observe:

```text
10-ticket process > 5-ticket process > 2-ticket process
```

in accumulated scheduling rounds over time.

---

## Repository Structure

```text
xv6-riscv/
├── kernel/
│   ├── proc.c
│   ├── proc.h
│   ├── syscall.c
│   ├── syscall.h
│   └── sysproc.c
├── user/
│   ├── test_scheduler.c
│   ├── user.h
│   └── usys.pl
├── Makefile
├── LICENSE
└── README.md
```

---

## Concepts Demonstrated

This project provides practical experience with:

- Operating-system CPU scheduling
- Proportional-share scheduling
- xv6 process management
- Process state transitions
- Context switching
- Kernel synchronization and process locks
- System-call implementation
- User-to-kernel communication
- Process creation and inheritance
- Pseudo-random scheduling decisions
- Testing kernel-level scheduling behavior

---

## About xv6

This project is built on **xv6**, a re-implementation of Dennis Ritchie's and Ken Thompson's Unix Version 6.

xv6 follows the structure and style of Unix V6 while targeting modern RISC-V systems and is used primarily as a teaching operating system for MIT's **6.1810: Operating System Engineering** course.

Official resources:

- **xv6 RISC-V repository:** https://github.com/mit-pdos/xv6-riscv
- **MIT 6.1810:** https://pdos.csail.mit.edu/6.1810/

The underlying xv6 source code, authorship, acknowledgements, and licensing belong to the original xv6 project and its contributors.

---

## Acknowledgements

The underlying operating system is developed and maintained by the **MIT PDOS** group and the xv6 contributors.

xv6 is inspired by Unix Version 6 and John Lions's *Commentary on UNIX 6th Edition*.

This repository contains modifications made specifically to implement and evaluate a **Lottery Scheduling algorithm** on xv6 RISC-V. Original xv6 code and comments remain attributed to their respective authors and contributors.

---

## Author

**Mejbah Uddin Bhuiyan**

- GitHub: [MejbahUddinBhuiyan](https://github.com/MejbahUddinBhuiyan)
- Project Repository: [xv6-lottery-scheduler](https://github.com/MejbahUddinBhuiyan/xv6-lottery-scheduler)

---

## License

This repository is based on the original **xv6 RISC-V** source code and retains the original xv6 `LICENSE` file.

The original xv6 copyright and permission notice should remain intact.

---

> Educational implementation created to study proportional-share CPU scheduling, process management, and system-call integration in xv6 RISC-V.
