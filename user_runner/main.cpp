//
//  main.cpp
//  user_runner
//
//  Created by Hank Jacobs on 10/27/15.
//  Copyright © 2015 Hank. All rights reserved.
//

#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, const char * argv[]) {
    
    pid_t child_pids[5];
    
    for (int i = 0; i < 5; i++) {
        pid_t pid = child_pids[i] = fork();
        
        if (pid == 0) {
            execl("./user", "user", NULL);
            exit(0);
        }
    }
    
    for (int i = 0; i < 5; i++) {
        int return_status;
        waitpid(child_pids[i], &return_status, 0);
    }
    
    return 0;
}
