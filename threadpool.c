// threadpool - a concurrent work queue

// ============================================================
// Libraries
// ============================================================

// Added by me
#include <time.h>
#include <stdbool.h>

// Pre-existing
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

// ============================================================
// Type Definitions
// ============================================================

typedef struct task_node {
    char task[1024];
    struct task_node *next;
} task_node_t;
typedef struct {
    task_node_t *head;
    task_node_t *tail;
    int done;
} task_queue_t;

// ============================================================
// Globals
// ============================================================

task_queue_t queue = { .head = NULL, .tail = NULL, .done = 0 };

// ============================================================
// Grader Helpers
// ============================================================

// Returns a integer given an input string deterministically
unsigned long djb2_hash(const char *str) {
unsigned long hash = 5381;
int c;

while ((c = *str++))
    hash = ((hash << 5) + hash) + c;

return hash;
}

// Takes in a string sleeps briefly to simulate meaningful work
// then returns a hash
void process_task(const char *task) {

    // Replaced deprecated usleep() with nanosleep()
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 100000000; // 100ms
    nanosleep(&ts, NULL);

    unsigned long hash = djb2_hash(task);
    printf("%lu %s\n", hash, task);
    fflush(stdout);
}

// ============================================================
// Core
// ============================================================

// Returns thread count and performs a sanity check on user input
int parse_thread_count(int argc, char **argv) {
    char *argc_usage_message = "Usage: echo -e 'hello\\nworld\\nfoo' | ./threadpool <num_of_threads>\n";
    char *argv_usage_message = "num_of_threads must be 1 or more";

    // Check if there are at least 2 arguments
    if (argc < 2) {
        fprintf(stderr, "%s", argc_usage_message);
        return -1;
    }

    // Parse the thread count
    char *end;
    long thread_count = strtol(argv[1], &end, 10);
    if (*end != '\0' || thread_count < 1) {
        fprintf(stderr, "%s", "num_of_threads must be a number >= 1\n");
        return -1;
    }

    return (int)thread_count;
}

// Generic worker function
void *worker(void *arg) {
    int id = *(int *)arg;
    printf("Worker %d ready\n", id);
    fflush(stdout);
    return NULL;
}

int main(int argc, char *argv[]) {

    int thread_count = parse_thread_count(argc, argv);
    if (thread_count == -1) return EXIT_FAILURE;

    // Buffer to hold user input
    char line[1024];

    while (true) {

        // Get user input
        if  (fgets(line, sizeof(line), stdin) != NULL) {
            // Finds newline char and sets it to null
            char *parse_newline = strchr(line, '\n');
            if (parse_newline) *parse_newline = '\0';
        }
        else {
            // End of file
            printf("%s", "\n");
            break;
        }

        // Check if string is empty; skip if it is
        if (strcmp(line, "") != 0) {
            process_task(line);
        } else {
            continue;
        }

        // Create threads
        pthread_t threads[thread_count];
        int thread_ids[thread_count];

        for (int i = 0; i < thread_count; ++i) {
            thread_ids[i] = i;
            pthread_create(&threads[i], NULL, worker, &thread_ids[i]);
        }

        for (int i = 0; i < thread_count; ++i){
            // Wait until thread finishes executing
            pthread_join(threads[i], NULL);
        }



    }

    return EXIT_SUCCESS;
}