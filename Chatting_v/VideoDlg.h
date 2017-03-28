#pragma once

#include"resource.h"
// VideoDlg 对话框

class VideoDlg : public CDialogEx
{
	DECLARE_DYNAMIC(VideoDlg)

public:
	VideoDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~VideoDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
//	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton3();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnCancel();
	afx_msg void OnBnClickedButton1();
};
