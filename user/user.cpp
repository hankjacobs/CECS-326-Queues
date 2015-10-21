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
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include "compute.h"

using namespace std;

static const int PAYLOAD_SIZE = 2048;
static const int MAX_QUEUE_SIZE = 3072;

typedef struct {
    int size;
    char payload[PAYLOAD_SIZE];
} request;

typedef struct {
    long mtype;
    request req;
} request_buf;

int main() {
    
    int qid = msgget(ftok(".",'u'), IPC_R|IPC_W|IPC_M);
    
    if (errno) {
        cout << "Error: " << errno << endl;
    }
    
    cout << "starting up" << endl;

    
    srand((unsigned int)time(NULL));
    
    for (int i = 0; i < 100; i++) {
        
        request_buf req_buf;
        req_buf.mtype = getpid();
        req_buf.req.size = (rand() % 1024) + 1025;

        cout << "Sending request with size: " << req_buf.req.size << endl;
        int ret = msgsnd(qid, &req_buf, sizeof(request_buf) - sizeof(long), 0);
        
        if (ret) {
            cout << "Error: " << errno << endl;
        }
        
        request_buf res_buf;
        msgrcv(qid, &res_buf, sizeof(request_buf), getpid(), 0);
        
        if (errno) {
            cout << "Error: " << errno << endl;
        }
        
        compute(res_buf.req.payload);
    }


    return 0;
}
