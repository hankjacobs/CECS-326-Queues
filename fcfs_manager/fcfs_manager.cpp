//
//  fcfs_manager.cpp
//  Queues
//
//  Created by Hank Jacobs on 10/15/15.
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
        
        cout << "Received request from " << req_msg.req.sender << " for data size " << req_msg.req.size << endl;
        
        response_msg res_msg = {};
        res_msg.mtype = req_msg.req.sender;
        res_msg.res.size = req_msg.req.size;
        
        fetch(res_msg.res.size, res_msg.res.payload);
        
        ret = msgsnd(qid, &res_msg, sizeof(request_msg) - sizeof(long), 0);
        
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
