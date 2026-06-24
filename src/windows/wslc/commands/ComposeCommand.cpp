/*++

Copyright (c) Microsoft. All rights reserved.

Module Name:

    ComposeCommand.cpp

Abstract:

    Implementation of compose command execution logic.

--*/
#include "CLIExecutionContext.h"
#include "ComposeCommand.h"
#include "ComposeParser.h"
#include "WslUtil.h"
#include "SessionTasks.h"
#include "ContainerService.h"
#include "NetworkService.h"

using namespace wsl::windows::wslc::execution;
using namespace wsl::windows::wslc::core;
using namespace wsl::windows::wslc::task;
using namespace wsl::windows::wslc::services;
using namespace wsl::windows::wslc::models;
using namespace wsl::shared;

namespace wsl::windows::wslc {

// ----------------------------------------------------------------------
// ComposeCommand
// ----------------------------------------------------------------------
std::vector<std::unique_ptr<Command>> ComposeCommand::GetCommands() const
{
    std::vector<std::unique_ptr<Command>> commands;
    commands.push_back(std::make_unique<ComposeUpCommand>(FullName()));
    commands.push_back(std::make_unique<ComposeDownCommand>(FullName()));
    return commands;
}

std::vector<Argument> ComposeCommand::GetArguments() const
{
    return {};
}

std::wstring ComposeCommand::ShortDescription() const
{
    return L"Define and run multi-container applications";
}

std::wstring ComposeCommand::LongDescription() const
{
    return L"Define and run multi-container applications using a compose.yaml file.";
}

void ComposeCommand::ExecuteInternal(CLIExecutionContext& context) const
{
    OutputHelp();
}

// ----------------------------------------------------------------------
// ComposeUpCommand
// ----------------------------------------------------------------------
std::vector<Argument> ComposeUpCommand::GetArguments() const
{
    return {
        Argument::Create(ArgType::Detach),
    };
}

std::wstring ComposeUpCommand::ShortDescription() const
{
    return L"Create and start containers";
}

std::wstring ComposeUpCommand::LongDescription() const
{
    return L"Builds, (re)creates, starts, and attaches to containers for a service.";
}

void ComposeUpCommand::ExecuteInternal(CLIExecutionContext& context) const
{
    // Resolve session
    ResolveSession(context);

    if (!context.Data.Contains(Data::Session)) {
        wsl::windows::common::wslutil::PrintMessage(L"Error: Could not resolve WSL container session.\n");
        return;
    }

    auto& session = context.Data.Get<Data::Session>();

    ComposeProject project;
    // We try docker-compose.yml first, then compose.yaml
    if (!ComposeParser::TryParse("docker-compose.yml", project) && !ComposeParser::TryParse("compose.yaml", project)) {
        wsl::windows::common::wslutil::PrintMessage(L"Error: Could not find or parse docker-compose.yml or compose.yaml in current directory.\n");
        return;
    }

    wsl::windows::common::wslutil::PrintMessage(L"Starting compose project...\n");

    // Basic orchestration (simplified for MVP)
    for (const auto& service : project.Services) {
        wsl::windows::common::wslutil::PrintMessage(std::format(L"Starting service {} (Image: {})...\n", wsl::windows::common::string::MultiByteToWide(service.Name), wsl::windows::common::string::MultiByteToWide(service.Image)));
        
        ContainerOptions options;
        options.Name = project.Name + "_" + service.Name; // Not setting network for now, MVP
        
        // Setup ports
        for (const auto& port : service.Ports) {
            options.Ports.push_back(port);
        }

        // Setup env
        for (const auto& env : service.Environment) {
            options.EnvironmentVariables.push_back(env.first + "=" + env.second);
        }

        options.Detach = context.Args.Contains(ArgType::Detach);
        
        int exitCode = ContainerService::Run(session, service.Image, options);
        if (exitCode != 0) {
            wsl::windows::common::wslutil::PrintMessage(std::format(L"Failed to start service {}. Exit code: {}\n", wsl::windows::common::string::MultiByteToWide(service.Name), exitCode));
        }
    }

    wsl::windows::common::wslutil::PrintMessage(L"Compose Up executed.\n");
}

// ----------------------------------------------------------------------
// ComposeDownCommand
// ----------------------------------------------------------------------
std::vector<Argument> ComposeDownCommand::GetArguments() const
{
    return {};
}

std::wstring ComposeDownCommand::ShortDescription() const
{
    return L"Stop and remove containers, networks";
}

std::wstring ComposeDownCommand::LongDescription() const
{
    return L"Stops containers and removes containers, networks, volumes, and images created by `up`.";
}

void ComposeDownCommand::ExecuteInternal(CLIExecutionContext& context) const
{
    // Resolve session
    ResolveSession(context);

    if (!context.Data.Contains(Data::Session)) {
        wsl::windows::common::wslutil::PrintMessage(L"Error: Could not resolve WSL container session.\n");
        return;
    }

    auto& session = context.Data.Get<Data::Session>();

    ComposeProject project;
    // We try docker-compose.yml first, then compose.yaml
    if (!ComposeParser::TryParse("docker-compose.yml", project) && !ComposeParser::TryParse("compose.yaml", project)) {
        wsl::windows::common::wslutil::PrintMessage(L"Error: Could not find or parse docker-compose.yml or compose.yaml in current directory.\n");
        return;
    }

    wsl::windows::common::wslutil::PrintMessage(L"Stopping and removing compose project...\n");

    for (const auto& service : project.Services) {
        std::string containerName = project.Name + "_" + service.Name;
        wsl::windows::common::wslutil::PrintMessage(std::format(L"Stopping and removing service {}...\n", wsl::windows::common::string::MultiByteToWide(service.Name)));
        
        StopContainerOptions stopOptions;
        
        try {
            ContainerService::Stop(session, containerName, stopOptions);
            ContainerService::Delete(session, containerName, true);
        } catch (...) {
            wsl::windows::common::wslutil::PrintMessage(std::format(L"Warning: Could not remove container {}.\n", wsl::windows::common::string::MultiByteToWide(containerName)));
        }
    }

    wsl::windows::common::wslutil::PrintMessage(L"Compose Down executed.\n");
}

} // namespace wsl::windows::wslc
