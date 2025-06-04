#include <iostream>
using namespace std;
#include <fstream>
#include <string>
#include <vector>
#pragma once

class Log
{
    private:
        
        int level;

    public:
        void initialize(int level);
        enum { INFO = 2, ERROR = 1, WARNING = 0};
        ofstream logFile;
        vector<string> logMessages;
        void log(int level, string message);
        void setLevel(int level);
        void displayLog();
        Log();
        ~Log();
};



