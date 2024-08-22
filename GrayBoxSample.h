// GrayBoxSample.h : main header file for the GrayBoxSample DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include <map>
#include <string>

struct IdPair {
	long id1;
	long id2;
	IdPair(long i1 = 0, long i2 = 0) : id1(i1), id2(i2) {}
};

// CGrayBoxSampleApp
// See GrayBoxSample.cpp for the implementation of this class
//

class CGrayBoxSampleApp : public CWinApp, public L_Observer
{
public:
	CGrayBoxSampleApp();

	void StartExtension();
	void StopExtension();
	void StartHttpApp();

// Overrides
public:
	virtual BOOL InitInstance();
	// L_Observer
	void HandleMessage(L_Message const* msg) override;
	DECLARE_MESSAGE_MAP()
	std::map<long, IdPair> idsMap;
	L_Account* account;
};
