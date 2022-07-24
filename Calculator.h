//------------------------------------------------------------------------------------------------------------
// Confidential and proprietary source code of BEOALEX.
// This source code may not be used, reproduced or disclosed in any manner without permission.
// Copyright (c) 2022, BEOALEX. All rights reserved.
// https://github.com/beoalex
//------------------------------------------------------------------------------------------------------------

#pragma once

#include <string>
#include <vector>
#include <future>

#include "Config.h"

//------------------------------------------------------------------------------------------------------------

class Calculator
{
public:
    explicit Calculator(const Config& config);

    void run() noexcept(false);

private:
    using HashTaskResult = std::vector<std::string>;
    using HashTaskFuture = std::future<HashTaskResult>;

    static constexpr std::uint32_t s_minHashTaskCount = 1;
    static constexpr std::uint32_t s_maxHashTaskCount = 32;
    static constexpr std::uint32_t s_minInputBufferSize = 16 * 1024 * 1024;
    static constexpr std::uint32_t s_outputBufferVirtualSize = 1'000'000'000;
    static constexpr std::byte s_hashSeparator = (std::byte)'\n';
    static constexpr std::uint32_t s_hashStringSize = 32;
    static constexpr std::uint32_t s_hashSeparatorSize = sizeof(s_hashSeparator);
    static constexpr std::uint32_t s_hashLineSize = s_hashStringSize + s_hashSeparatorSize;

    const Config m_config;
    std::uint32_t m_hashTaskCount = 0;
    std::uint32_t m_inputBufferSize = 0;
    std::uint64_t m_totalBlocks = 0;
    std::uint64_t m_blocksProcessed = 0;
    std::vector<std::byte> m_outputBuffer;
    std::future<void> m_outputFuture;

    void processInputBuffer(std::byte* pInputBuffer, std::size_t bytesRead, FILE* pOutputFile);
    void waitForHashTasks(std::vector<HashTaskFuture>& taskFutures, FILE* pOutputFile);
    void flushOutputBuffer(FILE* pOutputFile);

    static void closeFile(FILE* pFile);
};

//------------------------------------------------------------------------------------------------------------
