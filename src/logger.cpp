#include "logger.hpp"
using namespace std;

string Logger::nowISO(){
    using clock = chrono::system_clock;
    auto t = clock::to_time_t(clock::now());
    tm tm{};
    localtime_r(&t, &tm);
    char buf[20];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
    return string(buf);
} 

Logger::Logger(const string& filename)
    : logfile(filename, ios::app) {}

void Logger::info(const string& msg){
    lock_guard<mutex> lock(logmtx);
    logfile << "[" << nowISO() << "] [INFO] " << msg << "\n";
    logfile.flush();
}
void Logger::error(const string &message, const exception &e){
    lock_guard<mutex> lock(logmtx);
    logfile << "[" << nowISO() << "] [ERROR] " << message
             << " | " << typeid(e).name() << " | " << e.what() << "\n";
    logfile.flush();
}