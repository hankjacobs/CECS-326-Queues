//
//  common.h
//  Queues
//
//  Created by Hank Jacobs on 10/22/15.
//  Copyright © 2015 Hank. All rights reserved.
//

#ifndef common_h
#define common_h

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


#endif /* common_h */
