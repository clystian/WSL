/*++

Copyright (c) Microsoft. All rights reserved.

Module Name:

    ComposeCommand.h

Abstract:

    Declaration of compose command classes for wslc.

--*/
#pragma once
#include "Command.h"

namespace wsl::windows::wslc {

// Root Compose Command
struct ComposeCommand final : public Command
{
    constexpr static std::wstring_view CommandName = L"compose";
    ComposeCommand(const std::wstring& parent) : Command(CommandName, parent)
    {
    }
    std::vector<Argument> GetArguments() const override;
    std::wstring ShortDescription() const override;
    std::wstring LongDescription() const override;

    std::vector<std::unique_ptr<Command>> GetCommands() const override;

protected:
    void ExecuteInternal(CLIExecutionContext& context) const override;
};

// Compose Up Command
struct ComposeUpCommand final : public Command
{
    constexpr static std::wstring_view CommandName = L"up";
    ComposeUpCommand(const std::wstring& parent) : Command(CommandName, parent)
    {
    }
    std::vector<Argument> GetArguments() const override;
    std::wstring ShortDescription() const override;
    std::wstring LongDescription() const override;

protected:
    void ExecuteInternal(CLIExecutionContext& context) const override;
};

// Compose Down Command
struct ComposeDownCommand final : public Command
{
    constexpr static std::wstring_view CommandName = L"down";
    ComposeDownCommand(const std::wstring& parent) : Command(CommandName, parent)
    {
    }
    std::vector<Argument> GetArguments() const override;
    std::wstring ShortDescription() const override;
    std::wstring LongDescription() const override;

protected:
    void ExecuteInternal(CLIExecutionContext& context) const override;
};

} // namespace wsl::windows::wslc
