#pragma once

#include"resource.h"
// VideoDlg �Ի���

class VideoDlg : public CDialogEx
{
	DECLARE_DYNAMIC(VideoDlg)

public:
	VideoDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~VideoDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
//	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton3();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnCancel();
	afx_msg void OnBnClickedButton1();
};
