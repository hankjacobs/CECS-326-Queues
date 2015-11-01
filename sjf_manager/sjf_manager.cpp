// Henry Jacobs and Kenny Huang
// CECS 326
// Queues
// sjf_manager.cpp
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
#include <queue>
#include "compute.h"
#include "common.h"
#include "device.h"

using namespace std;

static volatile sig_atomic_t sig_caught = 0;

int handle_request(request_msg request, int qid);

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
    
    // Use a priority_queue where the smallest size has the highest priority
    priority_queue< request_msg, vector<request_msg>, greater<request_msg> > request_queue;
    while (true) {
        
        request_msg req_msg = {};
        
        ssize_t ret = 0;
        
        // If we have no backlog of requests to process,
        // we wait until we receive one.
        if (request_queue.size() == 0) {
            ret = msgrcv(qid, &req_msg, sizeof(request_msg) - sizeof(long), MTYPE_MANAGER, 0);
            request_queue.push(req_msg);
        }
        
        
        // Drain any pending messages from our queue (if any)
        // and add them to our priority queue
        while (ret != -1) {
            ret = msgrcv(qid, &req_msg, sizeof(request_msg) - sizeof(long), MTYPE_MANAGER, IPC_NOWAIT);
            
            if (ret != -1) {
                request_queue.push(req_msg);
            }
        }
        
        // Once we empty the queue, we receive a no message error
        // we supress this error since it is expected
        if (errno == ENOMSG) {
            ret = 0;
        }

        if (sig_caught) {
            break;
        }
        
        if (ret == -1) {
            cout << "Receive Error: " << errno << endl;
            return 0;
        }
        
        request_msg next_request = request_queue.top();
        request_queue.pop();

        start_device_throughput_timer();
        ret = handle_request(next_request, qid);
        stop_device_throughput_timer(next_request.req.size);
        
        total_requests++;
        
        if (sig_caught) {
            break;
        }
        
        if (ret == -1) {
            cout << "Send Error: " << errno << endl;
            return 0;
        }
    }
    
    cout << endl << "Exiting..." << endl;
    cout << "### Stats ###" << endl;
    cout << "Total requests: " << total_requests << endl;
    cout << "Total bytes read: " << get_total_bytes_read() << endl;
    cout << "Average bytes per millisecond: " << get_average_device_throughput() << endl;
    
    msgctl (qid, IPC_RMID, NULL);
    
    return 0;
}

