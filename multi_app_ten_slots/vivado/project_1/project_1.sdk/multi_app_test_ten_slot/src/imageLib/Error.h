///////////////////////////////////////////////////////////////////////////
//
// NAME
//  Error.h -- a simple error handling class
//
// DESCRIPTION
//  The CError class is used to throw error messages back to the calling program.
//
// Copyright © Richard Szeliski, 2001.
// See Copyright.h for more details
//
///////////////////////////////////////////////////////////////////////////

using namespace std;

#include <string.h>
#include <stdio.h>
#include <exception>
#include "xil_printf.h"

struct CError : public exception
{
    CError(const char* msg)                 { xil_printf(msg); }
    CError(const char* fmt, int d)          { xil_printf(fmt, d); }
    CError(const char* fmt, float f)        { xil_printf(fmt, f); }
    CError(const char* fmt, const char *s)  { xil_printf(fmt, s); }
    CError(const char* fmt, const char *s,
            int d)                          { xil_printf(fmt, s, d); }
    char message[1024];         // longest allowable message
};
