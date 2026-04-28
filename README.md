# Thread Pool

## Operating Systems Course

### Overview

Implemented a crude threadpool system using Posix standard Pthread API to print input lines in parralel

### Features

- Thread pool of user specified thread count
- Threads sleep while they wait for work (no burning CPU cycles)
- Race conditions handled with a mutex
- Condition used to prevent deadlock and hanging
