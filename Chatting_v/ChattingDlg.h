
// ChattingDlg.h : 头文件
//

#pragma once
#include"VideoDlg.h"
#include"network.h"
#include"v_SDLwrapper.h"
// CChattingDlg 对话框
class CChattingDlg : public CDialogEx
{
// 构造
public:
	CChattingDlg(CWnd* pParent = NULL);	// 标准构造函数
	VideoDlg		*m_video;
	network_dealer	m_ndealer;
	CWinThread*		m_wthread[7];
	HANDLE			m_hthread[7];
	HWND			m_wVideo;
	HWND			m_lVideo;
	SDL2Wrapper		m_sdl2;
	ULONG			m_startip;
	ULONG			m_endip;
	// 对话框数据
	enum { IDD = IDD_CHATTING_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnNcDestroy();
//	virtual void OnOK();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButton2();
	afx_msg void OnLbnDblclkList1();
	

	void HandleMsg(net_msg *msg);
	void HandleHeartbeats(const char* name);
	void HandleAsk(net_msg *msg);
	void HandleAck(net_msg *msg);
	bool SetStatus(const char *name, int status);
	void SetStatus(net_clt *pclt, int status);
	bool UnSetStatus(const char *name, int status);
	void UnSetStatus(net_clt *pclt, int status);
	void AddText(CString& text);
private:
	bool OpenVideoWnd();

	void AddMsg(CString& text, const char* name);
	void ClearText();
	bool GetEdit(CString& text);
	void ClearEdit();
	void StoreMsg(net_msg *msg);


	bool OnChatting(const char* name);
	void ShowRecord();
	bool WriteRecord(const char* ltime, const char* name, const char* txt);
public:
//	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
//	afx_msg void OnEnVscrollRichedit21();
	afx_msg void OnEnMsgfilterRichedit21(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnEnSetfocusRichedit21();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnEnChangeEdit2();
	afx_msg void OnBnClickedButton4();
};
