// VideoDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Chatting.h"
#include "VideoDlg.h"
#include "afxdialogex.h"
#include"thread.h"

// VideoDlg �Ի���

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


// VideoDlg ��Ϣ�������


void VideoDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	
	DestroyWindow();
	CDialogEx::OnClose();
}




void VideoDlg::OnBnClickedButton3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	end_thread_vc();
}


BOOL VideoDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		return true;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void VideoDlg::OnCancel()
{
	// TODO: �ڴ����ר�ô����/����û���

	CDialogEx::OnCancel();
}


void VideoDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (capscreen)
		return;
	CString dir;
	GetDlgItem(IDC_EDIT1)->GetWindowTextW(dir);
	if (dir.GetLength() > 255)
	{
		MessageBox(L"·����̫��");
		return;
	}
	if (dir == "")
	{
		sprintf(capdir, "./");
	}
	W2C(dir.GetBuffer(0), capdir);
	capscreen = 1;
}
