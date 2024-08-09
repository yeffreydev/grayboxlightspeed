// MainDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GrayBoxSample.h"
#include "MainDlg.h"
#include "afxdialogex.h"


// MainDlg dialog

IMPLEMENT_DYNAMIC(MainDlg, CDialog)

MainDlg::MainDlg(CWnd* pParent /*=NULL*/)
	: CDialog(MainDlg::IDD, pParent)
	, account(0)
{
}

MainDlg::~MainDlg()
{
	if (account)
	{
		account->L_Detach(this);
	}
}

BOOL MainDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	account = L_GetAccount();
	account->L_Attach(this);
	SetDlgItemText(IDC_TRADERID, CString(account->L_TraderId()));
	SetDlgItemInt(IDC_PENDINGORDERS, account->L_PendingOrdersCount());
	CString netPl;
	netPl.Format(L"%.2f", account->L_NetPL());
	SetDlgItemText(IDC_NETPL, netPl);

	return TRUE;
}

void MainDlg::HandleMessage(L_Message const *msg)
{
	switch (msg->L_Type())
	
	{
	case L_MsgOrderChange::id:
		SetDlgItemInt(IDC_PENDINGORDERS, account->L_PendingOrdersCount());
		break;
	case L_MsgAccountChange::id:
		{
			CString netPl;
			netPl.Format(L"%.2f", account->L_NetPL());
			SetDlgItemText(IDC_NETPL, netPl);
		}
		break;
	}
}

void MainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(MainDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1,&MainDlg::OnBnClickedSendOrder)
	ON_WM_NCDESTROY()
	ON_WM_CLOSE()
END_MESSAGE_MAP()

void MainDlg::OnNcDestroy()
{
	delete this;
}

void MainDlg::OnClose()
{
	L_ExitLightspeedExtension(2);
}



void MainDlg::OnBnClickedSendOrder()
{
	account->L_SendOrderBasic(
		"ZXZZT",
		L_OrderType::LIMIT,
		L_Side::BUY,
		100,
		L_PriceBase::bid,
		-0.01,
		"NSDQ",
		L_TIF::DAY
		);

	L_AddMessageToExtensionWnd("OnBnClickedSendOrder");
}

