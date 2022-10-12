#pragma once

#include <filesystem>

const inline std::string LS_TEST_DIR = std::filesystem::path(__FILE__).parent_path().string();
