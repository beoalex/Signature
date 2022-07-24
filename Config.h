//------------------------------------------------------------------------------------------------------------
// Confidential and proprietary source code of BEOALEX.
// This source code may not be used, reproduced or disclosed in any manner without permission.
// Copyright (c) 2022, BEOALEX. All rights reserved.
// https://github.com/beoalex
//------------------------------------------------------------------------------------------------------------

#pragma once

#include <cstdint>
#include <string>

//------------------------------------------------------------------------------------------------------------

class Config
{
public:
    Config(int argc, char* argv[]) noexcept(false);

    const std::string& getInputFile() const;
    const std::string& getOutputFile() const;
    std::uint32_t getBlockSize() const;

private:
    static constexpr std::uint32_t s_minParamCount = 3;
    static constexpr std::uint32_t s_maxParamCount = 4;
    static constexpr std::uint32_t s_inputFileParamIndex = 1;
    static constexpr std::uint32_t s_outputFileParamIndex = 2;
    static constexpr std::uint32_t s_blockSizeParamIndex = 3;
    static constexpr std::uint32_t s_minBlockSize = 512;
    static constexpr std::uint32_t s_defaultBlockSize = 1 * 1024 * 1024;
    static constexpr std::uint32_t s_maxBlockSize = 10 * 1024 * 1024;

    std::string m_inputFile;
    std::string m_outputFile;
    std::uint32_t m_blockSize = s_defaultBlockSize;
};

//------------------------------------------------------------------------------------------------------------
