#ifndef RACOON_NET_FD_EVENT_H
#define RACOON_NET_FD_EVENT_H

#include <functional>
#include <sys/epoll.h>

namespace racoon {

class FdEvent {
public:
    using handler_t = std::function<void()>;

    FdEvent();

    FdEvent(int fd);

    virtual ~FdEvent() {};

    int getFd() { return m_fd; }

    epoll_event getEpollEvent() { return m_epoll_event; }

    void setNonBlocking();

    void register_handler(uint32_t event_type, handler_t handler);

    handler_t getHandler(uint32_t event_type);

    void removeEvent(uint32_t event_type);

    void addEvent(uint32_t event_type);

private:
    int m_fd;
    epoll_event m_epoll_event;
    handler_t m_read_handler;
    handler_t m_write_handler;
    handler_t m_error_handler;
};

}

#endif