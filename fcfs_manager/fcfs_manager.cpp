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
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include "device.h"
#include <errno.h>

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
    
    int qid = msgget(ftok(".",'u'), IPC_CREAT|IPC_R|IPC_W|IPC_M);

    if (errno == EACCES) {
        qid = msgget(ftok(".",'u'), IPC_CREAT);
        msgctl (qid, IPC_RMID, NULL);
        qid = msgget(ftok(".",'u'), IPC_CREAT|IPC_R|IPC_W|IPC_M);
    }
    
    struct msqid_ds msgq;
    int rtn = msgctl(qid, IPC_STAT, &msgq);
    msgq.msg_qbytes = MAX_QUEUE_SIZE;

    if (rtn != 0) {
        cout << "Error: " << errno << endl;
    }
    
    while (true) {
        request_buf req_buf;
        msgrcv(qid, &req_buf, sizeof(request_buf), 0, 0);
        
        if (errno) {
            cout << "Error: " << errno << endl;
        }
        
        cout << "Received request from " << req_buf.mtype << " for data size " << req_buf.req.size << endl;
        
        
        request_buf res_buf;
        res_buf.mtype = req_buf.mtype;
        
        msgsnd(qid, &req_buf, sizeof(request_buf), 0);
        
        if (errno) {
            cout << "Error: " << errno << endl;
        }
        
    }
    
    return 0;
}