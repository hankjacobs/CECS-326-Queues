// Henry Jacobs and Kenny Huang
// CECS 326
// Queues
// fcfs_manager.cpp
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
#include "device.h"

using namespace std;

static volatile sig_atomic_t sig_caught = 0;

void handle_sigint(int signum) {
    sig_caught = 1;
}

int main() {
    
    int qid = msgget(ftok(".",'u'), IPC_CREAT|010600);
    int total_requests = 0;
    if (qid == -1) {
        cout << "msgget Error: " << errno << endl;
        return 0;
    }
    
    signal(SIGINT, handle_sigint);
    
    while (true) {
        
        request_msg req_msg = {};
        
        ssize_t ret = msgrcv(qid, &req_msg, sizeof(request_msg) - sizeof(long), MTYPE_MANAGER, 0);
        
        if (sig_caught) {
            break;
        }
        
        if (ret == -1) {
            cout << "Receive Error: " << errno << endl;
            return 0;
        }
        
        start_device_throughput_timer();
        ret = handle_request(req_msg, qid);
        stop_device_throughput_timer(req_msg.req.size);
        
        if (sig_caught) {
            break;
        }
        
        if (ret == -1) {
            cout << "Send Error: " << errno << endl;
            return 0;
        }
        
        total_requests++;
    }
    
    cout << endl << "Exiting..." << endl;
    cout << "### Stats ###" << endl;
    cout << "Total requests: " << total_requests << endl;
    cout << "Total bytes read: " << get_total_bytes_read() << endl;
    cout << "Average bytes per millisecond: " << get_average_device_throughput() << endl;
    
    msgctl (qid, IPC_RMID, NULL);
    
    return 0;
}
