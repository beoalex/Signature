//------------------------------------------------------------------------------------------------------------
// Confidential and proprietary source code of BEOALEX.
// This source code may not be used, reproduced or disclosed in any manner without permission.
// Copyright (c) 2022, BEOALEX. All rights reserved.
// https://github.com/beoalex
//------------------------------------------------------------------------------------------------------------

#include <iostream>
#include <chrono>
#include <locale>

#include "Config.h"
#include "Calculator.h"

using namespace std::chrono;

//------------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
	int result = EXIT_FAILURE;
	std::cout.imbue(std::locale("en_US"));
	const std::uint64_t startSec = duration_cast<seconds>(steady_clock::now().time_since_epoch()).count();

	try
	{
		const Config config(argc, argv);
		Calculator calc(config);
		calc.run();
		result = EXIT_SUCCESS;
	}
	catch (const std::exception& exc)
	{
		std::cout << std::endl << exc.what() << std::endl;
	}

	const std::uint64_t diffSec = duration_cast<seconds>(steady_clock::now().time_since_epoch()).count() - startSec;
	std::cout << std::endl << "Total working time: " << diffSec << " second(s)." << std::endl;

	return result;
}

//------------------------------------------------------------------------------------------------------------
