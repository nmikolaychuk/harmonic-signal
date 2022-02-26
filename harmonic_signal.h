
// harmonic_signal.h: главный файл заголовка для приложения PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить pch.h до включения этого файла в PCH"
#endif

#include "resource.h"		// основные символы


// CharmonicsignalApp:
// Сведения о реализации этого класса: harmonic_signal.cpp
//

class CharmonicsignalApp : public CWinApp
{
public:
	CharmonicsignalApp();

// Переопределение
public:
	virtual BOOL InitInstance();

// Реализация

	DECLARE_MESSAGE_MAP()
};

extern CharmonicsignalApp theApp;
