//
//  sjf_manager.cpp
//  Queues
//
//  Created by Hank Jacobs on 10/27/15.
//  Copyright © 2015 Hank. All rights reserved.
//

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
    
    if (qid == -1) {
        cout << "msgget Error: " << errno << endl;
        return 0;
    }
    
    signal(SIGINT, handle_sigint);
    
    //Use a priority_queue where the smallest size has the highest priority
    priority_queue< request_msg, vector<request_msg>, greater<request_msg> > request_queue;
    
    while (true) {
        
        request_msg req_msg = {};
        
        ssize_t ret = 0;
        
        // If we have no backlog of requests to process,
        // we wait until we receiving one.
        if (request_queue.size() == 0) {
            cout << "No backlog of requests. Waiting for new request" << endl;
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
        
        // Once we empty the queue, we receive and no message error
        // we supress this error
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
        
        cout << request_queue.size() << " pending requests." << endl;
        
        request_msg next_request = request_queue.top();
        request_queue.pop();
        cout << "Handling request from " << next_request.req.sender << " for data size " << next_request.req.size << endl;
        
        ret = handle_request(next_request, qid);
        
        if (sig_caught) {
            break;
        }
        
        if (ret == -1) {
            cout << "Send Error: " << errno << endl;
            return 0;
        }
    }
    
    cout << endl << "Exiting..." << endl;
    msgctl (qid, IPC_RMID, NULL);
    
    return 0;
}

int handle_request(request_msg request, int qid) {
    response_msg res_msg = {};
    res_msg.mtype = request.req.sender;
    res_msg.res.size = request.req.size;
    
    fetch(res_msg.res.size, res_msg.res.payload);
    
    return msgsnd(qid, &res_msg, sizeof(request_msg) - sizeof(long), 0);
}
