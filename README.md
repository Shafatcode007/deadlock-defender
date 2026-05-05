# Deadlock Defender (IPC Engine)

## 1. Introduction

**What is Deadlock?**

Deadlock is a situation where two or more processes are stuck waiting for each other to release resources. None of them can continue, so the system freezes.

**Why is Deadlock Dangerous?**

In real systems (like银行 servers, operating systems, or database systems), deadlock can:
- Crash the entire system
- Stop all services
- Cause data loss or corruption

**What This Project Does?**

This project shows how to prevent deadlock in a multi-process system. It uses the Banker's Algorithm to check if granting a resource request will keep the system safe. If unsafe, the request is denied.

---

## 2. Real Life Example

Imagine two people, Alice and Bob:

- Alice has a car and needs a bike
- Bob has a bike and needs a car

Both are waiting for each other. Neither can give up what they have. They are stuck forever!

**Another Example - Bank Account Locking:**

When you transfer money from one account to another, the system locks both accounts. If another process tries to lock them in reverse order, both processes wait forever.

---

## 3. What This Project Does

This project creates a simple simulation with multiple worker processes:

1. **Multiple processes are created** - Using fork(), we create worker processes
2. **Each process requests resources** - Workers need resources like memory, files, or locks
3. **A manager checks whether request is safe** - The manager uses Banker's Algorithm
4. **If safe → GRANTED** - Resources are given to the worker
5. **If unsafe → DENIED** - Request is rejected to prevent deadlock

---

## 4. Core Concepts Used (Simple Explanation)

### fork()

fork() is a system call that creates a new process. The new process (child) gets a copy of the parent's memory. In our project, fork() creates multiple worker processes.

### Shared Memory

Shared memory is a way for different processes to share data. We use shmget() to create it and shmat() to attach it. This allows the manager and workers to communicate.

### Semaphore

A semaphore is like a traffic light for processes. It prevents two processes from updating data at the same time. We use sem_wait() to wait and sem_post() to signal.

### Banker's Algorithm

Banker's Algorithm is a deadlock prevention method. Before granting any request, it checks: "If I grant this, will the system still be safe?" If yes, grant it. If no, deny it.

---

## 5. How the System Works (Step-by-step)

1. **Program starts** - Main process initializes everything
2. **Processes are created** - fork() creates worker processes
3. **Resources are initialized** - Each process gets initial resources
4. **Processes send requests** - Workers ask for more resources
5. **Manager checks using Banker's Algorithm** - Safety is verified
6. **Decision is made** - Request is GRANTED or DENIED
7. **System state updates** - Allocation and available resources change

---

## 6. Modes of Operation

### Vulnerable Mode

- No protection
- Deadlock may happen
- Workers can grant their own requests
- System may freeze (circular wait)

### Defender Mode

- Banker's Algorithm is ON
- Manager checks every request
- Unsafe requests are denied
- System runs to completion without deadlock

### Manual Mode

- User enters requests manually
- Can test specific deadlock scenarios
- Useful for learning

---

## 7. Example Input and Output

### Example 1:

```
Worker 1 requests: 1 0 2
Result: GRANTED (safe)
```

**Why?** The system has enough resources. After granting, the system remains in a safe state (all processes can finish).

### Example 2:

```
Worker 2 requests: 3 3 0
Result: DENIED (exceeds need or unsafe)
```

**Why?** Either the worker needs more than available, or granting this would create an unsafe state that could lead to deadlock.

---

## 8. How to Run

### Step 1: Compile

```bash
make
```

### Step 2: Run

```bash
./deadlock
```

For vulnerable mode (may deadlock):

```bash
./deadlock --vulnerable
```

---

## 9. Technologies Used

- **C programming** - Core language
- **Linux system calls** - fork(), shmget(), shmat()
- **POSIX Semaphores** - sem_init(), sem_wait(), sem_post()
- **Banker's Algorithm** - Safety check implementation

---

## 10. Author

**Md Atauz Zoha Shafat**
CSE 323 Operating Systems Project