/*++

Copyright (c) Microsoft. All rights reserved.

Module Name:

    WSLCE2EComposeTests.cpp

Abstract:

    This file contains end-to-end tests for WSLC compose.
--*/

#include "precomp.h"
#include "windows/Common.h"
#include "WSLCExecutor.h"
#include "WSLCE2EHelpers.h"
#include <fstream>
#include <filesystem>

namespace WSLCE2ETests {
using namespace wsl::shared;

class WSLCE2EComposeTests
{
    WSLC_TEST_CLASS(WSLCE2EComposeTests)

    WSLC_TEST_METHOD(WSLCE2E_Compose_HelpCommand)
    {
        auto result = RunWslc(L"compose --help");
        if (result.ExitCode != 0) {
            WEX::Logging::Log::Comment(WEX::Common::String().Format(L"ExitCode was: %d", result.ExitCode));
        }
        VERIFY_ARE_EQUAL(0, result.ExitCode);
        VERIFY_IS_TRUE(result.Stdout->find(L"Define and run multi-container applications") != std::wstring::npos);
    }

    WSLC_TEST_METHOD(WSLCE2E_Compose_UpAndDown)
    {
        // 1. Write a compose file
        std::string composeContent = "services:\n  test1:\n    image: alpine\n  test2:\n    image: alpine\n";
        std::ofstream out("compose.yaml");
        out << composeContent;
        out.close();

        // 2. Up
        auto upResult = RunWslc(L"compose up -d");
        if (upResult.ExitCode != 0) {
            WEX::Logging::Log::Comment(WEX::Common::String().Format(L"upResult ExitCode was: %d", upResult.ExitCode));
        }
        VERIFY_ARE_EQUAL(0, upResult.ExitCode);
        VERIFY_IS_TRUE(upResult.Stdout->find(L"Compose Up executed.") != std::wstring::npos);

        // 3. Down
        auto downResult = RunWslc(L"compose down");
        VERIFY_ARE_EQUAL(0, downResult.ExitCode);
        VERIFY_IS_TRUE(downResult.Stdout->find(L"Compose Down executed.") != std::wstring::npos);

        // Clean up
        std::filesystem::remove("compose.yaml");
    }
};
} // namespace WSLCE2ETests
