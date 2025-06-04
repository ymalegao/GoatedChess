#include "Log.h"
#include "../imgui/imgui.h"


void Log::initialize(int level){
    this->level = level;
    cout<<"Log initialized"<<endl;
    logMessages.push_back("Log initialized");
    logFile.open("log.txt");
    if (!logFile) {
        cerr << "Error opening log file." << endl;
        return;
    }else{
        cout<<"Log file opened"<<endl;
    }
    logFile << "Log initialized" << endl;


}


void Log::log(int level, string message){
    if(level >= this->level){
        cout<<message<<endl;
        logMessages.push_back(message);
        logFile << message << endl;
    }
}

void Log::setLevel(int level){
    this->level = level;
}



void Log::displayLog(){
    ImGui::Begin("Log");
    
    for (const string& msg: logMessages) {
        ImGui::TextUnformatted(msg.c_str());
    }
    ImGui::End();

    
}   

Log::Log()
{
    level = INFO;
    // Log::initialize(level);

}
//destructor
Log::~Log()
{
    logFile.close();
}   

