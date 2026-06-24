/*++

Copyright (c) Microsoft. All rights reserved.

Module Name:

    ComposeParser.cpp

Abstract:

    YAML parser for compose files.

--*/
#include "ComposeParser.h"
#include <yaml-cpp/yaml.h>
#include <iostream>

namespace wsl::windows::wslc::core {

bool ComposeParser::TryParse(const std::string& filePath, ComposeProject& outProject)
{
    try {
        YAML::Node config = YAML::LoadFile(filePath);

        if (!config["services"]) {
            return false;
        }

        auto cwd = std::filesystem::current_path().filename().string();
        if (cwd.empty() || cwd == "/" || cwd == "\\") {
            outProject.Name = "compose";
        } else {
            outProject.Name = cwd;
        }

        YAML::Node servicesNode = config["services"];
        for (YAML::const_iterator it = servicesNode.begin(); it != servicesNode.end(); ++it) {
            ComposeService service;
            service.Name = it->first.as<std::string>();

            YAML::Node serviceNode = it->second;
            if (serviceNode["image"]) {
                service.Image = serviceNode["image"].as<std::string>();
            }

            if (serviceNode["ports"]) {
                for (auto port : serviceNode["ports"]) {
                    service.Ports.push_back(port.as<std::string>());
                }
            }

            if (serviceNode["environment"]) {
                YAML::Node envNode = serviceNode["environment"];
                if (envNode.IsMap()) {
                    for (YAML::const_iterator envIt = envNode.begin(); envIt != envNode.end(); ++envIt) {
                        service.Environment[envIt->first.as<std::string>()] = envIt->second.as<std::string>();
                    }
                } else if (envNode.IsSequence()) {
                    for (auto env : envNode) {
                        std::string envStr = env.as<std::string>();
                        size_t eqPos = envStr.find('=');
                        if (eqPos != std::string::npos) {
                            service.Environment[envStr.substr(0, eqPos)] = envStr.substr(eqPos + 1);
                        } else {
                            service.Environment[envStr] = "";
                        }
                    }
                }
            }

            if (serviceNode["depends_on"]) {
                for (auto dep : serviceNode["depends_on"]) {
                    service.DependsOn.push_back(dep.as<std::string>());
                }
            }

            outProject.Services.push_back(service);
        }

        return true;
    } catch (const YAML::Exception&) {
        return false;
    }
}

} // namespace wsl::windows::wslc::core
