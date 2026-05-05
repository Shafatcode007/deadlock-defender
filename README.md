# Deadlock Defender (IPC Engine)

---

## 📽️ Demo Video

**[Watch the Demo Video](https://drive.google.com/file/d/1wB6oK5XMZk12GTN78Xd1QvIM4_sPnyyY/view?usp=sharing)**

A 3-minute demonstration showing all three modes:
- Vulnerable Mode (deadlock may occur)
- Defender Mode (Banker's Algorithm prevents deadlock)
- Manual Input Mode (interactive testing)

---

## 📂 Project Structure

```
deadlock-defender/
├── src/
│   ├── main.c          # Entry point, menu, fork workers
│   ├── manager.c       # Manager process - grants/denies requests
│   ├── worker.c       # Worker processes - request resources
│   ├── banker.c       # Banker's Algorithm implementation
│   └── ipc.c          # Shared memory and semaphore setup
├── include/
│   ├── banker.h       # Banker function declarations
│   ├── ipc.h          # Shared data structures
│   ├── manager.h     # Manager function declarations
│   └── worker.h      # Worker function declarations
├── report/
│   └── README.md      # Technical report info
├── docs/
│   └── project_report.html  # HTML version of report
├── screenshots/       # Execution screenshots
├── Makefile          # Build system
├── README.md        # This file
└── .gitignore       # Git ignore rules
```

---

## 📄 Technical Report

📥 **[Download PDF Report](report/CSE_323_project_report.pdf)**

Also available in HTML: `docs/project_report.html`

---

## 1. Introduction

**What is Deadlock?**

Deadlock is a situation where two or more processes get stuck waiting for each other. Each process holds some resources and waits for other resources that another process has. Since none can proceed, the system freezes completely.

**Why is Deadlock Dangerous?**

In real computer systems (like banking servers, operating systems, or databases), deadlock can:
- Crash the entire system
- Stop all services and make them unavailable
- Cause data loss or corruption
- Require manual restart to recover

**What This Project Does?**

This project simulates how to prevent deadlock in a multi-process system. It uses the **Banker's Algorithm** to check if granting a resource request will keep the system in a "safe state." If the system would become unsafe, the request is denied.

---

## 2. Real-Life Example

**The Two Friends Example:**

Imagine two friends, Alice and Bob:
- Alice has a car and needs a bike to go to the gym
- Bob has a bike and needs a car to go to work

Both are waiting for each other to give what they need. Neither can give up what they already have. They are stuck forever!

**The Bank Account Example:**

When you transfer money between two accounts, the system locks both accounts during the transaction. If another process tries to lock these accounts in reverse order (Account B first, then Account A), both processes will wait forever — this is deadlock.

---

## 3. What This Project Does

This project creates a simple simulation with multiple worker processes:

1. **Multiple processes are created** — Using `fork()`, we create 3 worker processes
2. **Each process requests resources** — Workers need resources (like memory, locks, or files)
3. **A manager process checks each request** — The manager uses Banker's Algorithm to decide
4. **If safe → GRANTED** — Resources are given to the worker
5. **If unsafe → DENIED** — Request is rejected to prevent deadlock

The manager acts as a safety guard — it only allows requests that keep the system safe.

---

## 4. Core Concepts Used (Simple Explanation)

### fork()

`fork()` is a system call that creates a new process. The new process (called child) gets a copy of the parent's memory. In this project, `fork()` creates multiple worker processes from the main program.

### Shared Memory

Shared memory is a way for different processes to share data. We use:
- `shmget()` — creates shared memory
- `shmat()` — attaches shared memory to a process

This allows the manager and workers to communicate and share the same system state.

### Semaphore

A semaphore is like a traffic light for processes. It prevents two processes from modifying data at the same time:
- `sem_wait()` — waits (decrements) — like stopping at a red light
- `sem_post()` — signals (increments) — like turning the light green

This ensures data consistency when multiple processes access shared memory.

### Banker's Algorithm

Banker's Algorithm is a deadlock prevention method. Before granting any request, it answers this question:

> "If I grant this request, will there still be a way for ALL processes to finish?"

If **yes** → GRANT the request
If **no** → DENY the request

This keeps the system in a "safe state" where deadlock cannot occur.

---

## 5. How the System Works (Step by Step)
1. **Program starts** — Main process shows a menu and waits for your choice
2. **Resources are initialized** — Available resources and worker maximum needs are set
3. **Worker processes are created** — Using `fork()`, 3 workers are created
4. **Processes send requests** — Each worker sends a resource request to the manager
5. **Manager checks request using Banker's Algorithm** — Safety is verified
6. **Decision is made:**
   - **GRANTED** — Request is safe, resources are allocated
   - **DENIED** — Request would cause unsafe state
7. **System updates state** — Allocation and available resources change
8. **Workers finish and release resources** — Resources are returned to the system
9. **Program ends cleanly** — No deadlock occurred in defender mode

---

### Visual Flow Diagram

```text
    +-------------------+
    |   Worker Process  |
    +-------------------+
              |
              v
    +-------------------+
    |   Request Resource|
    +-------------------+
              |
              v
    +-------------------+
    |  Manager Process  |
    | (Banker Algorithm)|
    +-------------------+
      |           |
 SAFE |           | UNSAFE
      v           v
 GRANTED       DENIED
```

---

## How the Decision is Made

When a worker requests resources, the manager follows this decision process:

**Step 1: Basic Checks**

1. Is the request less than or equal to the worker's **Need**?
2. Is the request less than or equal to **Available** resources?

**Step 2: Simulation**

If both basic checks pass:
- The system **simulates** granting the request
- It runs Banker's Algorithm to check if all processes can still finish

**Step 3: Final Decision**

- If a **safe sequence exists** → **GRANTED**
- If **no safe sequence** → **DENIED**

This two-step checking ensures:
- Workers cannot request more than they declared
- Resources are actually available
- The system never enters an unsafe state
- **Deadlock is prevented before it can happen**

---

## Safe Sequence (Key Idea of Banker's Algorithm)

The most important concept in Banker's Algorithm is the **Safe Sequence**.

**What is a Safe Sequence?**

A safe sequence is an order in which all processes can complete without getting stuck. The system looks ahead and asks: "Is there an order where every process can finish?"

**Example:**

If processes can finish in this order:
```
P2 → P1 → P3
```
Then the system is **SAFE**.

If no such order exists, the system is **UNSAFE** and the request is denied.

**Why Does This Matter?**

- The system doesn't just check current resources
- It checks if there's a path to completion for ALL processes
- Even with enough available resources, if no safe sequence exists → DENIED

This is the "look-ahead" nature of Banker's Algorithm that prevents deadlock before it happens.

---

## 6. Modes of Operation

### Vulnerable Mode (Option 1)

- **No safety checking** — Workers can grab any resources they want
- **Deadlock may occur** — Circular wait is possible
- **No manager oversight** — Workers self-grant their own requests
- **Use case:** See what happens when there's no protection

### Defender Mode (Option 2)

- **Banker's Algorithm is ON** — Manager checks every request
- **Only safe requests are allowed** — Unsafe requests are denied
- **System runs to completion** — No deadlock can happen
- **Use case:** See deadlock prevention in action

### Manual Input Mode (Option 3)

- **You enter requests manually** — Control which worker makes what request
- **Banker's Algorithm is still active** — Your requests are checked
- **Interactive testing** — Learn by experimenting
- **Use case:** Test specific scenarios and understand the algorithm

---

## 7. Example Input and Output

### Example 1: Safe Request Granted

**Input:**
```
Worker 1 requests: R0=0 R1=1 R2=0
```

**Result:** GRANTED

**Why?**
- The request is within the worker's maximum need
- After granting, the system still has enough available resources
- A safe sequence exists for all workers to finish

### Example 2: Exceeds Maximum Need

**Input:**
```
Worker 2 requests: R0=3 R1=3 R2=0
```

**Result:** DENIED

**Why?**
- The worker declared a maximum need in advance
- This request exceeds that declared maximum
- The system rejects it because the worker lied about its needs

### Example 3: Unsafe State

**Input:**
```
Worker 3 requests: R0=1 R1=1 R2=1
```

**Result:** DENIED

**Why?**
- Even though resources might be available, granting this would lead to an unsafe state
- After granting, no safe sequence would exist for all workers to finish
- The system denies it to prevent future deadlock

---

## 8. Why Requests Are DenIED

A request can be denied for three reasons:

### Reason 1: Exceeds Declared Maximum Need
- Each worker declares its maximum resource need at the start
- If a request exceeds this declared maximum, it's denied
- This prevents workers from requesting more than they said they would need

### Reason 2: Insufficient Resources Available
- The system doesn't have enough free resources to grant the request
- The worker must wait until other workers release resources

### Reason 3: Would Lead to Unsafe State
- Even if resources are available, granting would create a situation where no safe sequence exists
- This is the core of Banker's Algorithm — looking ahead to prevent future deadlock
- The system denies to keep all processes able to finish

---

## 9. Sample Output

Here is what the actual program output looks like in Defender Mode:

```
╔══════════════════════════════════════════════╗
║      DEADLOCK DEFENDER — IPC ENGINE          ║
║  Language : C  |  IPC: shmget + semaphores  ║
╚══════════════════════════════════════════════╝

Select mode:
  1) Vulnerable Mode  (no deadlock prevention)
  2) Defender Mode    (Banker's Algorithm ON)
  3) Manual Input Mode (interactive requests)
Choice: 2

[Main] Starting DEFENDER mode...

[Manager] PID=12345 — mode: DEFENDER

┌─────────────────────────────────────────────────┐
│              SYSTEM STATE                       │
├──────────┬──────────────┬──────────────┬────────┤
│ Worker   │ Allocation   │ Max          │ Need   │
├──────────┼──────────────┼──────────────┼────────┤
│ W1       │ [0 0 0]      │ [7 7 7]      │ [7 7 7] │
│ W2       │ [0 0 0]      │ [3 2 2]      │ [3 2 2] │
│ W3       │ [0 0 0]      │ [2 2 2]      │ [2 2 2] │
├──────────┴──────────────┴──────────────┴────────┤
│ Available: R0=10  R1=8   R2=9                  │
└─────────────────────────────────────────────────┘

[Worker 1] PID=12346 started
[Worker 1] Requesting R0=0 R1=1 R2=0
[Manager] Request from Worker 1: R0=0 R1=1 R2=0
[Manager] Decision: GRANTED — system still SAFE
[Worker 1] >> GRANTED

[Worker 2] Requesting R0=1 R1=0 R2=0
[Manager] Decision: GRANTED — system still SAFE
[Worker 2] >> GRANTED

[Manager] All workers finished. No deadlock occurred.
```

In **Vulnerable Mode**, you would see:
```
[Worker 1] >> self-granted (no check)
```
(no safety checking — deadlock may occur)

---

## Understanding the Output

When you see the program output, here's what each message means:

**Example 1:**
```
[Manager] Request from Worker 2: R0=3 R1=3 R2=0
[Manager] Decision: DENIED  — exceeds declared maximum need
```

**What it means:**
- The worker requested 3 of resource R0, 3 of R1, and 0 of R2
- The worker had declared it would never need more than its maximum need
- This request exceeds that declared maximum
- The manager denies it because workers must declare their needs in advance

**Example 2:**
```
[Manager] Request from Worker 1: R0=1 R1=1 R2=1
[Manager] Decision: DENIED  — would lead to unsafe state
```

**What it means:**
- Even though the resources might be available
- Even though the request is within the worker's declared maximum
- The system still denies it because:
  - After granting, **no safe sequence** would exist
  - Some worker(s) would not be able to finish
  - Future deadlock would be likely

**Key Insight:**

> The system may DENY a request even when resources ARE available. This is by design — the system looks AHEAD to prevent deadlock before it happens.

---

## 10. How to Run

### Step 1: Compile the program

```bash
make
```

### Step 2: Run the program

```bash
./deadlock
```

### Step 3: Choose a mode

When the program starts, it shows a menu:

```
Select mode:
  1) Vulnerable Mode  (no deadlock prevention)
  2) Defender Mode    (Banker's Algorithm ON)
  3) Manual Input Mode (interactive requests)
Choice: 
```

Type **1**, **2**, or **3** and press Enter.

### Understanding the Results

| Mode | What Happens | Can Deadlock Occur? |
|------|---------------|---------------------|
| 1 - Vulnerable | Workers grab resources without checking | Yes |
| 2 - Defender | Manager checks every request with Banker's Algorithm | No |
| 3 - Manual | You enter requests, Banker's Algorithm still active | Depends on your input |

---

## 11. Technologies Used

- **C Programming** — Core language used for the entire project
- **Linux System Calls** — `fork()` for process creation
- **POSIX Shared Memory** — `shmget()`, `shmat()` for inter-process communication
- **POSIX Semaphores** — `sem_init()`, `sem_wait()`, `sem_post()` for synchronization
- **Banker's Algorithm** — Safety check implementation for deadlock avoidance

---

## System Architecture Summary

The system consists of four main components working together:

| Component | Role |
|-----------|------|
| **Worker Processes** | Generate resource requests |
| **Shared Memory** | Stores system state (allocation, max, need, available) |
| **Manager Process** | Controls allocation and runs Banker's Algorithm |
| **Semaphore** | Ensures safe concurrent access to shared memory |

**Flow:**
1. Workers write their requests to shared memory
2. Manager reads requests and checks safety using Banker's Algorithm
3. Decision is written back to shared memory
4. Workers read the decision and proceed accordingly

---

## Why Command-Line (CLI) Application?

This project uses a command-line interface instead of a GUI:

- **Focus on OS-level logic** — Direct interaction with system calls (fork, shmget, sem_init)
- **Avoid UI complexity** — No time spent on graphics, menus, or mouse handling
- **Direct system call exposure** — Students can see exactly what Linux syscalls are being used
- **Better for demonstrating IPC** — Clear view of inter-process communication and synchronization
- **Easier to debug** — Simple text output makes tracing logic easier

This design choice keeps the project focused on core Operating Systems concepts rather than software engineering complexities.

---

## Real World Importance

Deadlock prevention is critical in many real-world systems:

### Where It's Used

- **Operating Systems** — Process scheduling and resource management
- **Databases** — Transaction locking and concurrency control
- **Banking Systems** — Resource allocation during transactions
- **Cloud Computing** — Allocating servers, storage, and network resources
- **Embedded Systems** — Managing hardware resources in real-time

### Why It Matters

Without deadlock prevention:
- **Systems can freeze** — Users experience complete hangs
- **Transactions fail** — Money transfers may get stuck
- **Services crash** — Websites and apps become unavailable
- **Data can be lost** — Incomplete operations leave corrupted data

### Our Solution

This project demonstrates the core concept that banks, operating systems, and databases use to prevent deadlock — checking safety BEFORE granting any resource.

---

## Limitations of Banker's Algorithm

While Banker's Algorithm is a foundational concept, it has practical limitations:

- **Requires advance knowledge** — Each process must declare its maximum resource need in advance
- **Not practical for large systems** — The algorithm becomes computationally expensive with many processes
- **Adds overhead** — Every request must be checked, slowing down resource allocation
- **Rarely used directly in modern OS** — Modern systems use simpler approaches like resource ordering or deadlock detection

Despite these limitations, understanding Banker's Algorithm is essential for:
- Learning the fundamentals of deadlock avoidance
- Understanding why deadlock prevention matters
- Building a foundation for more advanced OS concepts

---

## 12. What I Learned

Through this project, I gained hands-on experience with:

- **Process Creation** — Using `fork()` to create multiple concurrent processes
- **Inter-Process Communication (IPC)** — Sharing data between processes using shared memory
- **Process Synchronization** — Using semaphores to prevent race conditions
- **Deadlock Avoidance** — Implementing Banker's Algorithm to keep system in safe state

This project helped me understand how operating systems manage resources and prevent deadlock in real-world scenarios.

---

## 13. Future Improvements

Ideas for expanding this project:

- **GUI Version** — Add a graphical interface to visualize the system state in real-time
- **Logging System** — Record all requests and decisions to a log file for analysis
- **Visualization** — Show the safe sequence graphically so users can see how Banker's Algorithm works
- **Dynamic Resources** — Allow adding/removing resources while the program is running
- **More Workers** — Increase the number of workers to test with larger systems

---

## 14. Author

**Md Atauz Zoha Shafat (2312785642)**  
CSE 323 — Operating Systems  
Spring 2026
