// Henry Jacobs and Kenny Huang
// CECS 326
// Queues
// common.h
//
// [Project Description]

#ifndef common_h
#define common_h

#include <sys/types.h>

static const int PAYLOAD_SIZE = 2048;
static const int MAX_QUEUE_SIZE = PAYLOAD_SIZE + 1024;

static const long MTYPE_MANAGER = 1;

typedef struct {
    int size;
    pid_t sender;
} request;

typedef struct request_msg request_msg;
struct request_msg {
    long mtype;
    request req;
    
    bool operator<(const request_msg& rhs) const {
        return req.size < rhs.req.size;
    }
    
    bool operator>(const request_msg& rhs) const {
        return req.size > rhs.req.size;
    }
};

typedef struct {
    int size;
    char payload[PAYLOAD_SIZE];
} response;

typedef struct {
    long mtype;
    response res;
    int buff;
} response_msg;

//Time Helpers
long get_time_milliseconds();
long get_time_nanoseconds();

//Device Manager Helpers
int handle_request(request_msg request, int qid);

//Data throughput
void start_device_throughput_timer();
void stop_device_throughput_timer(int bytes_read);
double get_average_device_throughput();
int get_total_bytes_read();

//Completion time
void start_completion_timer();
void stop_completion_timer();
long double get_average_completion_time();
long double get_total_completion_time();

#endif /* common_h */
