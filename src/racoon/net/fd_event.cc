#include <cstring>
#include <fcntl.h>
#include "fd_event.h"

namespace racoon {

FdEvent::FdEvent()
: m_fd(-1), m_read_handler(nullptr), m_write_handler(nullptr), m_error_handler(nullptr) {
    std::memset(&m_epoll_event, 0, sizeof(m_epoll_event));
    m_epoll_event.data.ptr = this;
}

FdEvent::FdEvent(int fd)
: m_fd(fd), m_read_handler(nullptr), m_write_handler(nullptr), m_error_handler(nullptr) {
    std::memset(&m_epoll_event, 0, sizeof(m_epoll_event));
    m_epoll_event.data.ptr = this;
}

void FdEvent::setNonBlocking() {
    int old_opt = fcntl(m_fd, F_GETFL);
    if (old_opt & O_NONBLOCK) return;
    fcntl(m_fd, F_SETFL, old_opt | O_NONBLOCK);
}

void FdEvent::register_handler(uint32_t event_type, handler_t handler) {
    switch (event_type) {
        case EPOLLIN: m_read_handler = handler; break;
        case EPOLLOUT: m_write_handler = handler; break;
        case EPOLLERR: m_error_handler = handler; break;
        default: break;
    }
}

FdEvent::handler_t FdEvent::getHandler(uint32_t event_type) {
    switch (event_type) {
        case EPOLLIN: return m_read_handler;
        case EPOLLOUT: return m_write_handler;
        case EPOLLERR: return m_error_handler;
        default: return nullptr;
    }
}

void FdEvent::removeEvent(uint32_t event_type) {
    m_epoll_event.events &= (~event_type);
}

void FdEvent::addEvent(uint32_t event_type) {
    m_epoll_event.events |= event_type;
}

}