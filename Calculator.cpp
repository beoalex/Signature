//------------------------------------------------------------------------------------------------------------
// Confidential and proprietary source code of BEOALEX.
// This source code may not be used, reproduced or disclosed in any manner without permission.
// Copyright (c) 2022, BEOALEX. All rights reserved.
// https://github.com/beoalex
//------------------------------------------------------------------------------------------------------------

#include <memory>
#include <cassert>
#include <thread>
#include <iostream>
#include <filesystem>
#include <cmath>

#include "MD5.h"
#include "Exceptions.h"
#include "Calculator.h"

namespace fs = std::filesystem;

//------------------------------------------------------------------------------------------------------------

Calculator::Calculator(const Config& config) : m_config(config)
{
	m_hashTaskCount = std::min(s_maxHashTaskCount, std::max(s_minHashTaskCount, std::thread::hardware_concurrency()));
	m_inputBufferSize = m_config.getBlockSize() * m_hashTaskCount;

	if (m_inputBufferSize < s_minInputBufferSize)
	{
		m_inputBufferSize = (s_minInputBufferSize / m_inputBufferSize) * m_inputBufferSize;
	}

	m_outputBuffer.reserve((s_outputBufferVirtualSize / m_config.getBlockSize()) * s_hashLineSize);
	m_totalBlocks = (std::uint64_t)ceil((double)fs::file_size(m_config.getInputFile()) / (double)m_config.getBlockSize());
}

//------------------------------------------------------------------------------------------------------------

void Calculator::run() noexcept(false)
{
	m_blocksProcessed = 0;
	using FilePtr = std::unique_ptr<FILE, decltype(&Calculator::closeFile)>;

	const FilePtr pInputFile(_fsopen(m_config.getInputFile().c_str(), "rbS", _SH_DENYWR), &Calculator::closeFile);

	if (pInputFile.get() == nullptr)
	{
		throw ErrnoException("Error opening input file", errno);
	}

	const FilePtr pOutputFile(_fsopen(m_config.getOutputFile().c_str(), "wt", _SH_DENYWR), &Calculator::closeFile);

	if (pOutputFile.get() == nullptr)
	{
		throw ErrnoException("Error opening output file", errno);
	}

	const auto pInputBuffer = std::make_unique<std::byte[]>(m_inputBufferSize);

	while (true)
	{
		const std::size_t bytesRead = fread(pInputBuffer.get(), sizeof(std::byte), m_inputBufferSize, pInputFile.get());

		if (ferror(pInputFile.get()) != 0)
		{
			throw ErrnoException("Error reading input file", errno);
		}

		if (bytesRead == 0)
		{
			assert(feof(pInputFile.get()) != 0);
			break;
		}

		processInputBuffer(pInputBuffer.get(), bytesRead, pOutputFile.get());
	}

	flushOutputBuffer(pOutputFile.get());
	m_outputFuture.get();
}

//------------------------------------------------------------------------------------------------------------

void Calculator::processInputBuffer(std::byte* pInputBuffer, std::size_t bytesRead, FILE* pOutputFile)
{
	std::vector<HashTaskFuture> taskFutures;
	taskFutures.reserve(m_hashTaskCount);
	const std::byte* const pReadEnd = pInputBuffer + bytesRead;
	const std::uint32_t blocksPerTask = (std::uint32_t)ceil((double)bytesRead / (double)(m_config.getBlockSize() * m_hashTaskCount));

	for (std::uint32_t iTask = 0; iTask < m_hashTaskCount; ++iTask)
	{
		std::byte* const pTaskBlocks = pInputBuffer + m_config.getBlockSize() * blocksPerTask * iTask;

		if (pTaskBlocks >= pReadEnd)
		{
			break;
		}

		auto taskFuture = std::async(std::launch::async, [=]
		{
			HashTaskResult taskResult;
			taskResult.reserve(blocksPerTask);
				
			for (std::uint32_t iBlock = 0; iBlock < blocksPerTask; ++iBlock)
			{
				MD5 md5;
				std::byte* const pBlock = pTaskBlocks + (m_config.getBlockSize() * iBlock);

				if (pBlock >= pReadEnd)
				{
					break;
				}

				if (pReadEnd - pBlock < m_config.getBlockSize())
				{
					memset(pBlock + (pReadEnd - pBlock), 0, m_config.getBlockSize() - (pReadEnd - pBlock));
				}

				md5.update((const unsigned char*)pBlock, m_config.getBlockSize());
				md5.finalize();
				taskResult.push_back(md5.hexdigest());
			}

			assert(!taskResult.empty());
			return taskResult;
		});

		taskFutures.push_back(std::move(taskFuture));

		if (taskFutures.size() == m_hashTaskCount)
		{
			waitForHashTasks(taskFutures, pOutputFile);
		}
	}

	waitForHashTasks(taskFutures, pOutputFile);
}

//------------------------------------------------------------------------------------------------------------

void Calculator::waitForHashTasks(std::vector<HashTaskFuture>& taskFutures, FILE* pOutputFile)
{
	for (auto& future : taskFutures)
	{
		HashTaskResult taskResult(std::move(future.get()));
		assert(!taskResult.empty());

		for (const auto& hash : taskResult)
		{
			assert(hash.size() == s_hashStringSize);
			m_outputBuffer.resize(m_outputBuffer.size() + s_hashLineSize);
			memcpy(m_outputBuffer.data() + m_outputBuffer.size() - s_hashLineSize, hash.c_str(), s_hashStringSize);
			m_outputBuffer[m_outputBuffer.size() - 1] = s_hashSeparator;
			++m_blocksProcessed;

			if (m_outputBuffer.size() + s_hashLineSize > m_outputBuffer.capacity())
			{
				flushOutputBuffer(pOutputFile);
			}
		}
	}

	taskFutures.clear();
}

//------------------------------------------------------------------------------------------------------------

void Calculator::flushOutputBuffer(FILE* pOutputFile)
{
	if (m_outputFuture.valid())
	{
		m_outputFuture.get();
	}

	const std::size_t capacity = m_outputBuffer.capacity();

	m_outputFuture = std::async(std::launch::async, [outputBuffer = std::move(m_outputBuffer), pOutputFile]
	{
		if (fwrite(outputBuffer.data(), sizeof(decltype(outputBuffer)::value_type), outputBuffer.size(), pOutputFile) != outputBuffer.size())
		{
			throw std::runtime_error("Error writing output file.");
		}

		if (fflush(pOutputFile) != 0)
		{
			throw std::runtime_error("Error flushing output file.");
		}
	});

	m_outputBuffer.reserve(capacity);
	const std::uint32_t percentage = (std::uint32_t)ceil(100 * (double(m_blocksProcessed) / (double)m_totalBlocks));
	std::cout << m_blocksProcessed << " / " << m_totalBlocks << " (" << percentage << "%)" << std::endl;
}

//------------------------------------------------------------------------------------------------------------

// static
void Calculator::closeFile(FILE* pFile)
{
	if (pFile != nullptr)
	{
		fclose(pFile);
	}
}

//------------------------------------------------------------------------------------------------------------
