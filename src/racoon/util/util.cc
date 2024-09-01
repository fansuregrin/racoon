#include "util.h"

namespace racoon {

static pid_t g_pid = 0;

pid_t GetPid() {
    if (g_pid != 0) {
        return g_pid;
    }
    g_pid = getpid();
    return g_pid;
}

tm GetCurrentTime() {
    time_t now = time(nullptr);
    tm now_tm;
    localtime_r(&now, &now_tm);
    return now_tm;
}

}