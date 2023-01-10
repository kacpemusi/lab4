#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include "monitor.h"

#define MAX_QUEUE_SIZE 50

struct queue : public Monitor
{
    int items[MAX_QUEUE_SIZE];
    int front;
    int rear;
    int count;
    int id;
    Condition full, empty;

};
void* producer(void*);
void* consumer(void*);
queue q1;
queue q2;
queue q3;
queue q4;
void init_queue(queue *q, int id_a)
{
    q->front = 0;
    q->rear = -1;
    q->count = 0;
    q->id =  id_a;
}

void insert(queue *q, int item, int isSpecial)
{
    printf("\n");
    printf("M%d full check\n", q->id);
    if(q->count == MAX_QUEUE_SIZE)
        q->wait(q->full);
    printf("M%d full done\n", q->id);
    q->rear = (q->rear + 1) % MAX_QUEUE_SIZE;
    q->items[q->rear] = item;
    q->count++;
    if(q->count == 1){
        q->signal(q->empty);
        printf("M%d empty !!!\n", q->id);
    }
    if(isSpecial)
        sleep(2);
}

int pop(queue *q, int isSpecial)
{
    printf("\n");
    printf("M%d empty check\n", q->id);
    if(q->count == 0)
        q->wait(q->empty);
    printf("M%d empty done\n", q->id);
    int item = q->items[q->front];
    q->front = (q->front + 1) % MAX_QUEUE_SIZE;
    q->count--;
    if(q->count == MAX_QUEUE_SIZE-1){
        q->signal(q->full);
        printf("M%d full !!!\n", q->id);
    }
    return item;
}


void* producer(void *p)
{
    queue *q = (queue*)p;
    int isSpecial = q->id == 4;
    int item;
    while (1)
    {
        item = rand()%2+1;
        sleep(item);
        insert(q, item, isSpecial);
        printf("Q%d PROD item %d\n", q->id, item);
    }
}

void* consumer(void *p) {
    queue *q = (queue*)p;
    int item;
    while (1) {
        item = rand()%2+1;
        sleep(item);
        if (q4.count >= 0) {
            item = pop(&q4, 1);
            printf("Consumed item %d from special buffer\n", item);
        } else {
        item = pop(q,0);
        printf("Q%d CONS item %d\n", q->id, item);
        }
    }
}

int main(int argc, char* argv[])
{
    init_queue(&q1,1);
    init_queue(&q2,2);
    init_queue(&q3,3);
    init_queue(&q4,4);

    pthread_t prod1, prod2, prod3, prod4, cons1, cons2, cons3;
    pthread_create(&prod1, nullptr, producer, &q1);
    pthread_create(&prod2, nullptr, producer, &q2);
    pthread_create(&prod3, nullptr, producer, &q3);
    pthread_create(&prod4, nullptr, producer, &q4);

    pthread_create(&cons1, nullptr, consumer, &q1);
    pthread_create(&cons2, nullptr, consumer, &q2);
    pthread_create(&cons3, nullptr, consumer, &q3);

    pthread_join(prod1, nullptr);
    pthread_join(prod2, nullptr);
    pthread_join(prod3, nullptr);
    pthread_join(prod4, nullptr);
    pthread_join(cons1, nullptr);
    pthread_join(cons2, nullptr);
    pthread_join(cons3, nullptr);

    return 0;
}