#include <fstream>
#include <mutex>
#include <string>
#include <chrono>
#include <ctime>
#include <iomanip>

class Logger{
    private:
        std::ofstream logfile;
        std::mutex logmtx;

        static std::string nowISO();
    
    public:
    explicit Logger(const std::string& filename);
    void info(const std::string& message);
    void error(const std::string& message, const std::exception& e);
};