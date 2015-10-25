//
//  user.cpp
//  Queues
//
//  Created by Hank Jacobs on 10/20/15.
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
#include <sys/time.h>

using namespace std;


int main() {
    
    int qid = msgget(ftok(".",'u'), 010600);
    
    if (errno) {
        cout << "Error: " << errno << endl;
        return 0;
    }
    
    cout << "starting up" << endl;

    
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

        cout << "Sending request with size: " << req_msg.req.size << endl;
        
        ssize_t ret = msgsnd(qid, &req_msg, sizeof(request_msg) - sizeof(long), 1);
        
        if (ret == -1) {
            cout << "Send Error: " << errno << endl;
            return 0;
        }
        
        response_msg res_msg = {};
        ret = msgrcv(qid, &res_msg, sizeof(response_msg) - sizeof(long), req_msg.req.sender, 0);
        
        if (ret == -1) {
            cout << "Receive Error: " << errno << endl;
            return 0;
        }
        
        compute(res_msg.res.payload);
    }
    
    return 0;
}


