#ifndef CONFIG_H
#define CONFIG_H

#include <fstream>
#include <iostream>
#include <optional>
#include <string>

struct ConfigItem {
    int label{};
    int layers{};
};
struct Config {
    std::vector<ConfigItem> items;
};

inline Config ReadConfig(const std::string &path) {
    SPDLOG_INFO("Read config file...");
    std::ifstream input_file(path);
    if (!input_file.is_open()) {
        std::cerr << "Unable to open file: " << path << std::endl;
        exit(-1);
    }

    Config config;
    std::string line;
    while (std::getline(input_file, line)) {
        std::istringstream iss(line);
        ConfigItem config_item{};
        iss >> config_item.label >> config_item.layers;
        config.items.push_back(config_item);
    }
    input_file.close();
    return config;
}

#endif
