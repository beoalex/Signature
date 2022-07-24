//------------------------------------------------------------------------------------------------------------
// Confidential and proprietary source code of BEOALEX.
// This source code may not be used, reproduced or disclosed in any manner without permission.
// Copyright (c) 2022, BEOALEX. All rights reserved.
// https://github.com/beoalex
//------------------------------------------------------------------------------------------------------------

#pragma once

#include <string>
#include <format>
#include <string.h>
#include <stdexcept>

//------------------------------------------------------------------------------------------------------------

class ErrnoException : public std::exception
{
public:
    ErrnoException(const char* action, int errNo)
    {
        char errNoText[128];
        strerror_s(errNoText, sizeof(errNoText), errNo);
        m_what = std::format("{}: {}.", action, errNoText);
    }

    virtual const char* what() const override
    {
        return m_what.c_str();
    }

private:
    std::string m_what;
};

//------------------------------------------------------------------------------------------------------------
