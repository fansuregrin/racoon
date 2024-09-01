#include "config.h"

namespace racoon {

Config::Config(const char *filename) {
    auto ret = m_xml_doc.LoadFile(filename);
    if (ret != tinyxml2::XML_SUCCESS) {
        exit(EXIT_FAILURE);
    }

    auto racoon = ReadNode("racoon", &m_xml_doc);

    auto server = ReadNode("server", racoon);
    port = ReadIntFromNode("port", server);
    io_threads = ReadIntFromNode("io_threads", server);
    std::fprintf(stdout, "Server -- port: %d, io_threads: %d\n", port, io_threads);

    auto log = ReadNode("log", racoon);
    log_level = ReadStrFromNode("log_level", log);
    log_dir = ReadStrFromNode("log_dir", log);
    log_filename = ReadStrFromNode("log_filename", log);
    log_max_file_size = ReadIntFromNode("log_max_file_size", log);
}

Config::node_type* Config::ReadNode(const char *node_name, node_type *parent) {
    auto node = parent->FirstChildElement(node_name);
    if (!node) {
        std::fprintf(stderr, "Load config error, failed to read node [%s]", node_name);
        exit(EXIT_FAILURE);
    }
    return node;
}

std::string Config::ReadStrFromNode(const char *node_name, node_type *parent) {
    auto node = parent->FirstChildElement(node_name);
    if (!node || !node->GetText()) {
        std::fprintf(stderr, "Load config error, failed to read node [%s]", node_name);
        exit(EXIT_FAILURE);
    }
    return node->GetText();
}

int Config::ReadIntFromNode(const char *node_name, node_type *parent) {
    auto node = parent->FirstChildElement(node_name);
    if (!node || !node->GetText()) {
        std::fprintf(stderr, "Load config error, failed to read node [%s]", node_name);
        exit(EXIT_FAILURE);
    }
    int val = 0;
    try {
        val = std::stoi(node->GetText());
    } catch (std::exception &e) {
        std::fprintf(stderr, "Load config error, failed to parse node [%s]", node_name);
        exit(EXIT_FAILURE);
    }
    return val;
}

}