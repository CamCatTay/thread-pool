# Thread Pool

## Operating Systems Course

### Overview

Implemented a crude threadpool system using Posix standard Pthread API to print input lines in parralel

### How to compile and run

Compile with this command:
```bash
gcc -std=c2x threadpool.c -o threadpool -pthread
```

Example usage:
```bash
echo -e "hello\nworld\nfoo" | ./threadpool 4
```

### Features

- Thread pool of user specified thread count
- Threads sleep while they wait for work (no burning CPU cycles)
- Race conditions handled with a mutex
- Condition used to prevent deadlock and hanging
