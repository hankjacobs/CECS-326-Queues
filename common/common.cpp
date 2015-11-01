// Henry Jacobs and Kenny Huang
// CECS 326
// Queues
// common.cpp
//
// [Project Description]

#include <inttypes.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include "common.h"
#include <sys/ipc.h>
#include <sys/msg.h>
#include "compute.h"
#include "device.h"

static long total_milliseconds = 0;
static int total_bytes_read = 0;

int handle_request(request_msg request, int qid) {
    response_msg res_msg = {};
    res_msg.mtype = request.req.sender;
    res_msg.res.size = request.req.size;
    
    fetch(res_msg.res.size, res_msg.res.payload);
    
    return msgsnd(qid, &res_msg, sizeof(request_msg) - sizeof(long), 0);
}

long get_time_milliseconds() {
    
    struct timeval tv;
    gettimeofday (&tv, NULL);
    return (tv.tv_sec * 1000) + (tv.tv_usec/1000);
}

long get_time_nanoseconds() {
    
    struct timespec tps;
    clock_gettime(CLOCK_REALTIME, &tps);
    
    return tps.tv_sec * 1e+9 + tps.tv_nsec;
    
}

static long start_time = 0.0;
void start_device_throughput_timer() {
    
    start_time = get_time_milliseconds();
}

void stop_device_throughput_timer(int bytes_read) {
    
    total_milliseconds += (get_time_milliseconds() - start_time);
    total_bytes_read += bytes_read;
}

double get_average_device_throughput() {
    
    return (double)total_bytes_read/(double)total_milliseconds;
}

int get_total_bytes_read() {
    return total_bytes_read;
}

static long double completion_start_time = 0.0;
static long double completion_total = 0.0;
static int num_of_requests = 0;


void start_completion_timer() {
    num_of_requests++;
    completion_start_time = get_time_nanoseconds();
}

void stop_completion_timer() {
    completion_total += get_time_nanoseconds() - completion_start_time;
}

long double get_average_completion_time() {
    return completion_total / num_of_requests;
}

long double get_total_completion_time() {
    return completion_total;
}