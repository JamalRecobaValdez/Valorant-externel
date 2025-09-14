#include <logs/logs.hxx>

namespace logs {

    std::ofstream log_stream;

    auto init_log(LPSTR logname) -> void {
        if (!log_stream) {
            std::exception(skCrypt("[fracture] Log undefined!").decrypt());
            return;
        }

        if (!log_stream.is_open()) {
            log_stream.open(logname, std::ios::out | std::ios::app);
        }
    }

    auto log(LPSTR message, LPSTR branding, bool cout) -> void {
        init_log((char*)skCrypt("fracture.log").decrypt());

        time_t now = time(0);
        tm* ltm = localtime(&now);
        log_stream << "[" << 1900 + ltm->tm_year << "-"
            << 1 + ltm->tm_mon << "-"
            << ltm->tm_mday << " "
            << ltm->tm_hour << ":"
            << ltm->tm_min << ":"
            << ltm->tm_sec << "] "
            << branding << " "
            << message << std::endl;

        if (cout) {
            std::cerr << branding << " " << message << std::endl;
        }
    }

}