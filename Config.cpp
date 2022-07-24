//------------------------------------------------------------------------------------------------------------
// Confidential and proprietary source code of BEOALEX.
// This source code may not be used, reproduced or disclosed in any manner without permission.
// Copyright (c) 2022, BEOALEX. All rights reserved.
// https://github.com/beoalex
//------------------------------------------------------------------------------------------------------------

#include <stdexcept>
#include <format>
#include <filesystem>

#include "Config.h"

namespace fs = std::filesystem;

//------------------------------------------------------------------------------------------------------------

Config::Config(int argc, char* argv[]) noexcept(false)
{
	if (   argc < s_minParamCount
		|| argc > s_maxParamCount)
	{
		throw std::invalid_argument(std::format("The number of parameters must be between {} and {}.", s_minParamCount, s_maxParamCount));
	}

	m_inputFile = argv[s_inputFileParamIndex];
	m_outputFile = argv[s_outputFileParamIndex];

	if (!fs::exists(m_inputFile))
	{
		throw std::invalid_argument("The input file does not exist.");
	}

	if (fs::file_size(m_inputFile) == 0)
	{
		throw std::invalid_argument("The input file must not be empty.");
	}

	if (fs::weakly_canonical(m_inputFile) == fs::weakly_canonical(m_outputFile))
	{
		throw std::invalid_argument("The input and output files must not be the same.");
	}

	if (argc > s_blockSizeParamIndex)
	{
		try
		{
			m_blockSize = std::stoi(argv[s_blockSizeParamIndex]);
		}
		catch (const std::invalid_argument&)
		{
			throw std::invalid_argument("The block size parameter is not an integer.");
		}
		catch (const std::out_of_range&)
		{
			throw std::invalid_argument("The block size parameter is out of range.");
		}
	}

	if (   m_blockSize < s_minBlockSize
		|| m_blockSize > s_maxBlockSize)
	{
		throw std::invalid_argument(std::format("The block size must be between {} and {}.", s_minBlockSize, s_maxBlockSize));
	}
}

//------------------------------------------------------------------------------------------------------------

const std::string& Config::getInputFile() const
{
    return m_inputFile;
}

//------------------------------------------------------------------------------------------------------------

const std::string& Config::getOutputFile() const
{
    return m_outputFile;
}

//------------------------------------------------------------------------------------------------------------

std::uint32_t Config::getBlockSize() const
{
    return m_blockSize;
}

//------------------------------------------------------------------------------------------------------------
