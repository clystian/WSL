/*++

Copyright (c) Microsoft. All rights reserved.

Module Name:

    ComposeParser.h

Abstract:

    YAML parser for compose files.

--*/
#pragma once
#include <string>
#include <vector>
#include <map>

namespace wsl::windows::wslc::core {

struct ComposeService {
    std::string Name;
    std::string Image;
    std::vector<std::string> Ports;
    std::map<std::string, std::string> Environment;
    std::vector<std::string> DependsOn;
};

struct ComposeProject {
    std::string Name;
    std::vector<ComposeService> Services;
};

class ComposeParser {
public:
    static bool TryParse(const std::string& filePath, ComposeProject& outProject);
};

} // namespace wsl::windows::wslc::core
