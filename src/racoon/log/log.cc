#include <chrono>
#include <ctime>
#include <thread>
#include <sstream>
#include <iomanip>
#include <sys/stat.h>
#include "log.h"
#include "../util/util.h"

namespace racoon {

std::string LogLevelToString(LogLevel lv) {
    switch (lv) {
        case DEBUG: return "DEBUG";
        case INFO: return "INFO";
        case WARN: return "WARN";
        case ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

LogLevel StringToLogLevel(const std::string &lv) {
    if (lv == "DEBUG") {
        return DEBUG;
    } else if (lv == "INFO") {
        return INFO;
    } else if (lv == "WARN") {
        return WARN;
    } else if (lv == "ERROR") {
        return ERROR;
    } else {
        return UNKNOWN;
    }
}

std::string LogEvent::ToString() {
    m_pid = GetPid();
    m_tid = std::this_thread::get_id();

    auto now_tp = std::chrono::system_clock::now();
    auto now_dur = now_tp.time_since_epoch();
    
    char fmt_tm_buf[64] = {0};
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now_tp);
    struct tm now_tm;
    localtime_r(&now_time_t, &now_tm);
    auto len = std::strftime(fmt_tm_buf, sizeof(fmt_tm_buf), "%Y-%m-%d %H:%M:%S", &now_tm);
    // 计算微秒
    auto micro_sec = std::chrono::duration_cast<std::chrono::microseconds>(
        now_dur - std::chrono::duration_cast<std::chrono::seconds>(now_dur)).count();
    snprintf(fmt_tm_buf+len, sizeof(fmt_tm_buf)-len-1, ".%ld", micro_sec);

    std::ostringstream oss;
    oss << "[" << std::setw(5) << std::left << LogLevelToString(m_lv) << "] ["
        << std::setw(26) << std::left << fmt_tm_buf << "] ["
        << m_pid << ":" << m_tid << "] ";
    if (m_lv == DEBUG) {
        oss << "[" << m_filename << ":" << m_line_no << "] ";
    }

    return oss.str();
}

const char * const AsyncLogger::ext = ".log";

AsyncLogger &AsyncLogger::GetInstance() {
    static AsyncLogger logger;
    return logger;
}

AsyncLogger::AsyncLogger()
: m_type(0), m_log_level(UNKNOWN), m_closed(true), m_fp(nullptr) {}

AsyncLogger::~AsyncLogger() {}

void AsyncLogger::Flush() {
    m_queue->flush();
    std::fflush(m_fp);
}

void AsyncLogger::Init(uint8_t type, const std::string &logdir, const std::string &filename,
int max_file_size, LogLevel log_level) {
    std::unique_lock<std::mutex> lck(m_mtx);
    m_type = type;
    m_log_level = log_level;
    m_logdir = logdir;
    if (*m_logdir.rbegin() != '/') {
        m_logdir += '/';
    }
    m_filename = filename;
    m_max_file_size = max_file_size;
    m_seq_no = 1;
    m_queue.reset(new racoon::BlockingQueue<std::string>());
    m_write_thread.reset(new std::thread([] {
        GetInstance().AsyncWrite();
    }));
    if (m_fp) {
        Flush();
        std::fclose(m_fp);
    }
    lck.unlock();

    int ret = mkdir(logdir.c_str(), 0755);
    if (ret < 0 && errno != EEXIST) {
        perror("mkdir failed");
        exit(EXIT_FAILURE);
    }

    tm now_tm = GetCurrentTime();
    char suffix[24] = {0};
    int len = std::strftime(suffix, sizeof(suffix)-1, "_%Y%m%d", &now_tm);
    lck.lock();
    int seq_no = m_seq_no;
    std::string fullpath = m_logdir + m_filename;
    lck.unlock();
    snprintf(suffix+len, sizeof(suffix)-len-1, "_%d%s", seq_no, ext);
    FILE * tmp =  std::fopen((fullpath + suffix).c_str(), "a");
    if (!tmp) {
        std::perror("fopen failed");
        exit(EXIT_FAILURE);
    }
    lck.lock();
    m_fp = tmp;
    m_closed = false;
}

void AsyncLogger::PushLog(const std::string &log_str) {
    m_queue->push(log_str);
}

void AsyncLogger::AsyncWrite() {
    std::string msg;
    while (m_queue->pop(msg)) {
        std::lock_guard<std::mutex> lck(m_mtx);
        if (m_type && STDOUT_MASK) {
            printf("%s", msg.c_str());
        }
        if (m_type && FILE_MASK) {
            while (std::ftell(m_fp) >= m_max_file_size) {
                tm now_tm = GetCurrentTime();
                char suffix[24] = {0};
                int len = std::strftime(suffix, sizeof(suffix)-1, "_%Y%m%d", &now_tm);
                snprintf(suffix+len, sizeof(suffix)-len-1, "_%d%s", ++m_seq_no, ext);
                std::fclose(m_fp);
                m_fp = nullptr;
                FILE *tmp = std::fopen((m_logdir+m_filename+suffix).c_str(), "a");
                if (!tmp) break;
                m_fp = tmp;
            }
            std::fputs(msg.c_str(), m_fp);
            Flush();
        }
    }
}

bool AsyncLogger::Closed() const {
    std::lock_guard<std::mutex> lck(m_mtx);
    return m_closed;
}

void AsyncLogger::CloseLogger() {
    std::unique_lock<std::mutex> lck(m_mtx);
    if (m_queue) {
        lck.unlock();
        while (!m_queue->empty()) {
            m_queue->flush();
        }
        m_queue->close();
    }
    lck.lock();
    if (m_write_thread && m_write_thread->joinable()) {
        lck.unlock();
        m_write_thread->join();
    }
    lck.lock();
    if (m_fp) {
        Flush();
        std::fclose(m_fp);
        m_fp = nullptr;
    }
    m_closed = true;
}

LogLevel AsyncLogger::GetLogLevel() {
    return m_log_level;
}

}