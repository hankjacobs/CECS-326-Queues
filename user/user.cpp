// Henry Jacobs and Kenny Huang
// CECS 326
// Queues
// user.cpp
//
// [Project Description]

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include "compute.h"
#include "common.h"
#include <sys/time.h>

using namespace std;


int main() {
    
    int qid = msgget(ftok(".",'u'), 010600);
    
    if (errno) {
        cout << "Error: " << errno << endl;
        return 0;
    }

    for (int i = 0; i < 100; i++) {
        
        // The following chunk of code is to work around the fact that
        // the "compute()" function calls srand internally.
        // Due that fact, calling compute() within a loop (as we are)
        // will cause the rand() function used below to return the same value
        // we work around this by seeding every loop iteration using
        // microseconds (so we are sure it will change)
        struct timeval tv;
        gettimeofday(&tv,NULL);
        srand(tv.tv_usec);
        
        request_msg req_msg = {};
        req_msg.mtype = MTYPE_MANAGER;
        req_msg.req.sender = getpid();
        req_msg.req.size = (rand() % 1921) + 128; //random size with range [128, 2048]

        
        ssize_t ret = msgsnd(qid, &req_msg, sizeof(request_msg) - sizeof(long), 1);
        
        if (ret == -1) {
            cout << "Send Error: " << errno << endl;
            return 0;
        }
        
        response_msg res_msg = {};
        start_completion_timer();
        ret = msgrcv(qid, &res_msg, sizeof(response_msg) - sizeof(long), req_msg.req.sender, 0);
        stop_completion_timer();
        
        if (ret == -1) {
            cout << "Receive Error: " << errno << endl;
            return 0;
        }
        
        compute(res_msg.res.payload);
    }
    
    cout << "##### Stats #####" << endl;
    cout << "Average completion time per request (ns): " << get_average_completion_time() << endl;
    cout << "Total completion time (ns): " << get_total_completion_time() << endl;
    
    return 0;
}


