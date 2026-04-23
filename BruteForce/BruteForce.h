#pragma once

#ifndef __AFXWIN_H__
#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // основные символы

class CBruteForceApp : public CWinApp
{
public:
    CBruteForceApp();

public:
    virtual BOOL InitInstance();

    DECLARE_MESSAGE_MAP()
};

extern CBruteForceApp theApp;