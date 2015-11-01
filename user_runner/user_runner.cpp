// Henry Jacobs and Kenny Huang
// CECS 326
// Queues
// user_runner.cpp
//
// [Project Description]

#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

using namespace std;

int main(int argc, const char * argv[]) {
    
    int number_of_runs = 0;
    cout << "Enter number of times to run: ";
    cin >> number_of_runs;
    
    for (int i = 0; i < number_of_runs; i++) {
        
        cout << "#### RUN " << i + 1 << " ####" << endl;
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
        
        cout << endl << endl;
    }
    
    return 0;
}
