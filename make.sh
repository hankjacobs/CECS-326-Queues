#!/bin/bash

g++ -o bin/user user/user.cpp common/library.o -I user -I common
g++ -o bin/fcfs_manager fcfs_manager/fcfs_manager.cpp common/library.o -I user -I common