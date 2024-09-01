#ifndef RACOON_CONFIG_H
#define RACOON_CONFIG_H

#include <string>
#include <tinyxml2.h>

namespace racoon {

class Config {
public:
    Config(const Config &) = delete;

    Config &operator=(const Config &) = delete;
    
    static Config &GetInstance(const char *filename) {
        static Config cfg(filename);
        return cfg;
    }

public:
    int port;
    int io_threads;
    std::string log_level;
    std::string log_filename;
    std::string log_dir;
    int log_max_file_size;

private:
    using node_type = tinyxml2::XMLNode;

    Config(const char *filename);


    ~Config() {}

    node_type* ReadNode(const char *node_name, node_type *parent);

    std::string ReadStrFromNode(const char *node_name, node_type *parent);

    int ReadIntFromNode(const char *node_name, node_type *parent);

    tinyxml2::XMLDocument m_xml_doc;
};

}

#endif // end of RACOON_CONFIG_H