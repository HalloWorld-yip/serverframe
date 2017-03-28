// VideoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Chatting.h"
#include "VideoDlg.h"
#include "afxdialogex.h"
#include"thread.h"

// VideoDlg 对话框

IMPLEMENT_DYNAMIC(VideoDlg, CDialogEx)

VideoDlg::VideoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(VideoDlg::IDD, pParent)
{

}

VideoDlg::~VideoDlg()
{
}

void VideoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(VideoDlg, CDialogEx)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON3, &VideoDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON1, &VideoDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// VideoDlg 消息处理程序


void VideoDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	
	DestroyWindow();
	CDialogEx::OnClose();
}




void VideoDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	end_thread_vc();
}


BOOL VideoDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		return true;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void VideoDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	CDialogEx::OnCancel();
}


void VideoDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	if (capscreen)
		return;
	CString dir;
	GetDlgItem(IDC_EDIT1)->GetWindowTextW(dir);
	if (dir.GetLength() > 255)
	{
		MessageBox(L"路径名太长");
		return;
	}
	if (dir == "")
	{
		sprintf(capdir, "./");
	}
	W2C(dir.GetBuffer(0), capdir);
	capscreen = 1;
}
