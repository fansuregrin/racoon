#ifndef RACOON_UTIL_H
#define RACOON_UTIL_H

#include <unistd.h>
#include <ctime>

namespace racoon {

pid_t GetPid();

tm GetCurrentTime();

}
#endif // end of RACOON_UTIL_H