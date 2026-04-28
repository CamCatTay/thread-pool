#include <time.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

typedef struct task_node {
    char task[1024];
    struct task_node *next;
} task_node_t;
typedef struct {
    task_node_t *head;
    task_node_t *tail;
    int done;
    pthread_mutex_t mutex;
} task_queue_t;

task_queue_t queue = { .head = NULL, .tail = NULL, .done = false };

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

int parse_arguments(int argc, char **argv) {
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

void push_task(task_queue_t *q, const char *task) {
    task_node_t *node = malloc(sizeof(task_node_t));
    strcpy(node->task, task);
    node->next = NULL;
    if (q->tail) {
        q->tail->next = node;
        q->tail = node;
    } else {
        q->head = node;
        q->tail = node;
    }
}

int pop_task(task_queue_t *q, char *buf) {
    if (q->head == NULL) return false;
    task_node_t *node = q->head;
    strcpy(buf, node->task);
    q->head = node->next;
    if (q->head == NULL) q->tail = NULL;
    free(node);
    return true;
}

// Generic worker function
void *worker(void *arg) {
    while (true) {
        char buffer[1024];
        pthread_mutex_lock(&queue.mutex);
        bool result = pop_task(&queue, buffer);
        pthread_mutex_unlock(&queue.mutex);
        if (result) {
            process_task(buffer);
        }
        else if (queue.done) {
            break;
        }
    }
    /* OLD
    int id = *(int *)arg;
    printf("Worker %d ready\n", id);
    fflush(stdout);
    return NULL;
    */
}

void read_input_and_push(char *line, int size) {
    while (true) {
        if  (fgets(line, size, stdin) != NULL) {
            char *parse_newline = strchr(line, '\n');
            if (parse_newline) *parse_newline = '\0';
            pthread_mutex_lock(&queue.mutex);
            push_task(&queue, line);
            pthread_mutex_unlock(&queue.mutex);
        }
        else {
            printf("%s", "\n");
            pthread_mutex_lock(&queue.mutex);
            queue.done = true;
            pthread_mutex_unlock(&queue.mutex);
            break;
        }
    }
}

int main(int argc, char *argv[]) {

    int thread_count = parse_arguments(argc, argv);
    if (thread_count == -1) return EXIT_FAILURE;

    pthread_mutex_init(&queue.mutex, NULL);

    pthread_t threads[thread_count];
    int thread_ids[thread_count];

    for (int i = 0; i < thread_count; ++i) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, worker, &thread_ids[i]);
    }

    int line_size = 1024;
    char line[line_size];

    read_input_and_push(line, line_size);

        /* OLD
        if (strcmp(line, "") != 0) {
            process_task(line);
        } else {
            continue;
        }
        */

    for (int i = 0; i < thread_count; ++i){
        pthread_join(threads[i], NULL);
    }

    return EXIT_SUCCESS;
}