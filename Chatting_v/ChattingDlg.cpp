
// ChattingDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Chatting.h"
#include "ChattingDlg.h"
#include "afxdialogex.h"
#include "thread.h"
#include <string>
#include<fstream>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CChattingDlg �Ի���



CChattingDlg::CChattingDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CChattingDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	AfxInitRichEdit2(); 
}

void CChattingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CChattingDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_CHANGE(IDC_EDIT1, &CChattingDlg::OnEnChangeEdit1)
	ON_BN_CLICKED(IDOK, &CChattingDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CChattingDlg::OnBnClickedButton1)
	ON_WM_NCDESTROY()
	ON_BN_CLICKED(IDC_BUTTON2, &CChattingDlg::OnBnClickedButton2)
	ON_LBN_DBLCLK(IDC_LIST1, &CChattingDlg::OnLbnDblclkList1)
//	ON_WM_MOUSEWHEEL()
	ON_NOTIFY(EN_MSGFILTER, IDC_RICHEDIT21, &CChattingDlg::OnEnMsgfilterRichedit21)
	ON_BN_CLICKED(IDC_BUTTON3, &CChattingDlg::OnBnClickedButton3)
	ON_EN_CHANGE(IDC_EDIT2, &CChattingDlg::OnEnChangeEdit2)
	ON_BN_CLICKED(IDC_BUTTON4, &CChattingDlg::OnBnClickedButton4)
END_MESSAGE_MAP()


// CChattingDlg ��Ϣ�������

BOOL CChattingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	start_thread_ul(this);
	start_thread_rm(this);
	start_thread_ch(this);
	hoststatus = STATUS_ALIVE;
	GetDlgItem(IDC_BUTTON1)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON2)->EnableWindow(false);
	GetDlgItem(IDC_RICHEDIT21)->SendMessageW(EM_SETEVENTMASK, NULL, ENM_MOUSEEVENTS);
	//Ĭ��ͼƬ�����ַ
	sprintf(capdir, "E:/");
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CChattingDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CChattingDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CChattingDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CChattingDlg::OnEnChangeEdit1()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}


void CChattingDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CDialogEx::OnOK();
}


void CChattingDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CButton *pbton = (CButton*)GetDlgItem(IDC_BUTTON1);
	if (!pbton->IsWindowEnabled())
		return;
	CString	name;
	pbton->GetWindowTextW(name);

	hoststatus = STATUS_WAITING;

	//ndealer.tcpconnect(search_in_list(targetname)->ip);

	m_ndealer.udpsend(search_in_list(targetname)->ip, SIGNAL_VIDEOASK);

	pbton->EnableWindow(false);
	
	//
	AfxBeginThread(time_out, this);


}


void CChattingDlg::OnNcDestroy()
{
	//TODO: ENDTHREAD
	end_thread_ch();
	end_thread_ul();
	end_thread_rb();
	end_thread_vc();
	CDialogEx::OnNcDestroy();
	// TODO: �ڴ˴������Ϣ����������
}




BOOL CChattingDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if(pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_RETURN)
	{
		if (GetDlgItem(IDC_EDIT2)==GetFocus() || GetDlgItem(IDC_EDIT3)==GetFocus())
		{
			OnBnClickedButton4();
		}
		else
			OnBnClickedButton2();
		return true;
	}
	

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CChattingDlg::OnBnClickedButton2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!GetDlgItem(IDC_BUTTON2)->IsWindowEnabled())
		return;

	CString	text;
	char	buf[UDPBUF-2*sizeof(int)-NAMELEN];
	if(!GetEdit(text)||text.GetLength()>=sizeof(buf))
		return;
	
	AddMsg(text,hostname);

	W2C(text.GetBuffer(),buf);
	text.ReleaseBuffer();

	m_ndealer.udpsend(search_in_list(targetname)->ip,buf,SIGNAL_TEXT);
}



void CChattingDlg::OnLbnDblclkList1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CListBox	*pList = (CListBox*)GetDlgItem(IDC_LIST1);
	CStringW	text;
	net_clt		*pclt;
	char		switchname[NAMELEN];
	char		*psname;
	int			cursel;

	if ((cursel = pList->GetCurSel()) == -1)
		return;
	pList->GetText(cursel, text);
	W2C(text.GetBuffer(), switchname);
	text.ReleaseBuffer();

	if (switchname[0] == '*')
		psname = switchname + 1;
	else
		psname = switchname;
	/*if (OnChatting(psname))
		return;*/
	if (pclt = search_in_list(psname))
	{
		GetDlgItem(IDC_BUTTON1)->EnableWindow(true);
		GetDlgItem(IDC_BUTTON2)->EnableWindow(true);
	}
	else
		return;
	if (pclt->status & STATUS_BUSY)
	{
		GetDlgItem(IDC_BUTTON1)->EnableWindow(false);
	}
	if (switchname[0] == '*')
		strcpy(targetname, switchname + 1);
	else
		strcpy(targetname, switchname);
	ClearText();
	ShowRecord();
}

bool CChattingDlg::OpenVideoWnd()
{

	m_video = new VideoDlg();
	if (m_video != NULL) {
		if (!m_video->Create(IDD_DIALOG1, this))
			return false;
		m_wVideo = m_video->GetDlgItem(IDC_STATIC)->GetSafeHwnd();
		if (!m_wVideo)
			return false;
		m_lVideo = m_video->GetDlgItem(IDC_STATIC1)->GetSafeHwnd();
		if (!m_wVideo)
			return false;
		//m_video->ShowWindow(SW_SHOW);
		//m_video->SetWindowPos((CWnd*)HWND_TOP, 0, 0, VIDEO_W+10, VIDEO_H+20, SWP_NOMOVE);
		return (m_video->GetDlgItem(IDC_STATIC)->SetWindowPos((CWnd*)HWND_TOP, 0, 0, VIDEO_W, VIDEO_H, SWP_NOMOVE));
		
	}
	return false;
}


void CChattingDlg::AddMsg(CString& text,const char* name)
{
	
	CString			newtext;
	wchar_t			wname[NAMELEN],wtime[20];
	char			ltime[20];
	char			txt[UDPBUF];
	W2C(text, txt);
	GetTime(ltime);
	WriteRecord(ltime, name, txt);
	C2W(name,wname);
	C2W(ltime,wtime);
	newtext = CString(wname) +'\t'+ wtime + '\n'+text +'\n'+'\n';
	AddText(newtext);

}

void CChattingDlg::AddText(CString& text) {

	CString			oldtext;
	CRichEditCtrl	*pRichEdit;
	int				line;
	int				maxline;

	pRichEdit = (CRichEditCtrl*)GetDlgItem(IDC_RICHEDIT21);
	pRichEdit->GetWindowTextW(oldtext);
	pRichEdit->SetWindowTextW(oldtext + text);
	//pRichEdit->SetSel(0, -1);
	line = pRichEdit->GetFirstVisibleLine();
	//debug
	maxline = pRichEdit->GetLineCount();
	//
	if(maxline - line>13)
		pRichEdit->LineScroll(maxline - line-13);
	//pRichEdit->PostMessage(WM_HSCROLL, SB_BOTTOM, 0);
}

void CChattingDlg::ClearText()
{
	CRichEditCtrl	*pRichEdit;

	pRichEdit = (CRichEditCtrl*)GetDlgItem(IDC_RICHEDIT21);
	pRichEdit->SetSel(0, -1);
	pRichEdit->SetWindowTextW(L"");
}

bool CChattingDlg::GetEdit(CString& text)
{
	CEdit		*pEdit;
	pEdit = (CEdit*)GetDlgItem(IDC_EDIT1);
	pEdit->GetWindowTextW(text);
	if (!text.GetLength())
		return false;
	pEdit->SetWindowTextW(L"");
	return true;
}

void CChattingDlg::ClearEdit()
{
	CEdit		*pEdit;
	pEdit = (CEdit*)GetDlgItem(IDC_EDIT1);
	pEdit->SetWindowTextW(L"");
}


void CChattingDlg::HandleMsg(net_msg *msg)
{
	wchar_t textbuf[UDPBUF];
	C2W(msg->buf, textbuf);
	if (OnChatting(msg->name))
	{
		AddMsg(CString(textbuf), msg->name);
	}
	else {
		SetStatus(msg->name, STATUS_UNREAD);
		StoreMsg(msg);
	}
}

void CChattingDlg::HandleHeartbeats(const char* name)
{
	net_clt* pclt = search_in_list(name);
	if (!pclt)
		return;

	pclt->life	  = 3;
	m_ndealer.udpsend(pclt->ip, SIGNAL_HEARDBEATACK);
}

void CChattingDlg::HandleAsk(net_msg *msg)
{
	if (hoststatus != STATUS_ALIVE)
	{
		m_ndealer.udpsend(msg->saddr.sin_addr.S_un.S_addr, "0", SIGNAL_VIDEOACK);
		//ndealer.tcpend();
		return;
	}
	net_clt* pclt = search_in_list(msg->name);
	if (!pclt)
		return;
	wchar_t wname[NAMELEN];
	C2W(msg->name, wname);
	if (IDYES == MessageBoxW(CString(wname) + L":Լ��", 0, MB_YESNO | MB_DEFBUTTON1))
	{
		hoststatus = STATUS_READY;
		m_ndealer.udpsend(msg->saddr.sin_addr.S_un.S_addr, "1", SIGNAL_VIDEOACK);
		//����Ƶ����
		SendMessageW(WM_COMMAND, MAKEWPARAM(IDC_BUTTON3, BN_CLICKED), NULL);
		//GetDlgItem(IDC_BUTTON1)->SetWindowTextW(L"�����ʼ��Ƶ");

		start_thread_vc(this);
	}
	else
	{
		m_ndealer.udpsend(msg->saddr.sin_addr.s_addr, "0", SIGNAL_VIDEOACK);
		//ndealer.tcpend();
	}
}

void CChattingDlg::HandleAck(net_msg *msg)
{
	
	if (hoststatus == STATUS_WAITING)
	{
		if (msg->buf[0] == '1')
		{
			hoststatus = STATUS_BUSY;
			GetDlgItem(IDC_BUTTON1)->EnableWindow(true);
			m_ndealer.udpsend(msg->saddr.sin_addr.s_addr, "0", SIGNAL_VIDEOACK);
			m_ndealer.broadcast(STATUS_BUSY);
			videoip = msg->saddr.sin_addr.s_addr;

			SendMessageW(WM_COMMAND, MAKEWPARAM(IDC_BUTTON3, BN_CLICKED), NULL);
			//GetDlgItem(IDC_BUTTON1)->SetWindowTextW(L"�����ʼ��Ƶ");

			start_thread_vc(this);
		}
		else
		{
			if (m_video)
			{
				m_video->SendMessageW(WM_COMMAND, MAKEWPARAM(IDC_BUTTON3, BN_CLICKED), NULL);
				delete m_video;
				m_video = 0;
				m_wVideo = 0;
			}
			CString str(L"\n -----------------��Ƶ�ر�------------------- \n");
			AddText(str);
			GetDlgItem(IDC_BUTTON1)->EnableWindow(true);
			//ndealer.tcpend();
			hoststatus = STATUS_ALIVE;
		}
	}
	else if(hoststatus == STATUS_READY)
	{
		hoststatus = STATUS_BUSY;
		videoip = msg->saddr.sin_addr.s_addr;
		m_ndealer.broadcast(STATUS_BUSY);
	}
	
}

void CChattingDlg::StoreMsg(net_msg *msg) 
{
	char	ltime[20];
	GetTime(ltime);
	WriteRecord(ltime, msg->name, msg->buf);
}

void CChattingDlg::ShowRecord()
{
	char	path[NAMELEN + 5]="./record";
	wchar_t	buf[409];
	string s;
	if (!PathIsDirectoryA(path))
	{
		::CreateDirectoryA(path, NULL);
	}
	sprintf(path, "./record/%s.txt", targetname);
	std::ifstream fin(path,std::ios::in);
	if (!fin.is_open())
	{
		Log("open input");
		return;
	}
	while (std::getline(fin, s))
	{
		s += '\n';
		C2W(s.c_str(), buf);
		AddText(CString(buf));
	}
	fin.close();
	UnSetStatus(targetname, STATUS_UNREAD);
}

bool CChattingDlg::OnChatting(const char* name)
{
	return strcmp(name, targetname) == 0;
}


bool CChattingDlg::SetStatus(const char *name, int status)
{
	net_clt *pclt;
	if ((pclt = search_in_list(name)) == 0) {
		return false;
	}
	SetStatus(pclt, status);
	return true;
}

void CChattingDlg::SetStatus(net_clt *pclt, int status)
{
	if (status == STATUS_UNREAD)
		pclt->status |= status;
	else
		pclt->status = status;
}



bool CChattingDlg::UnSetStatus(const char *name, int status)
{
	net_clt *pclt;
	if ((pclt = search_in_list(name)) == 0) {
		return false;
	}
	UnSetStatus(pclt, status);
	return true;
}
void CChattingDlg::UnSetStatus(net_clt *pclt, int status)
{
	if (status == STATUS_UNREAD)
		pclt->status &= (~status);
	else
		pclt->status = STATUS_ALIVE;
}


bool CChattingDlg::WriteRecord(const char* ltime, const char* name, const char* txt)
{
	char	path[NAMELEN + 5];
	if (!PathIsDirectoryA(path))
	{
		::CreateDirectoryA(path, NULL);
	}
	sprintf(path, "./record/%s.txt", name);
	std::ofstream fout(path, std::ios::out | std::ios::app);
	if (!fout.is_open())
	{
		Log("open output");
		return false;
	}
	fout << name << '\t' << ltime << endl;
	fout << txt << endl;
	fout << endl;
	fout.close();
	return true;
}






void CChattingDlg::OnEnMsgfilterRichedit21(NMHDR *pNMHDR, LRESULT *pResult)
{
	MSGFILTER *pMsgFilter = reinterpret_cast<MSGFILTER *>(pNMHDR);
	// TODO:  �ؼ��������ʹ�֪ͨ����������д
	// CDialogEx::OnInitDialog() �������Խ� EM_SETEVENTMASK ��Ϣ����
	// ���ÿؼ���ͬʱ�� ENM_KEYEVENTS �� ENM_MOUSEEVENTS ��־
	//�������㵽 lParam �����С�
	
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (pMsgFilter->msg == WM_MOUSEWHEEL)
	{
		SHORT			zDelta	= HIWORD(pMsgFilter->wParam);
		CRichEditCtrl	*pEdit	= (CRichEditCtrl*)GetDlgItem(IDC_RICHEDIT21);
		if (zDelta > 0 && pEdit->GetFirstVisibleLine() > 0)
		{
			pEdit->LineScroll(-2);
		}
		if (zDelta < 0 && pEdit->GetFirstVisibleLine() < pEdit->GetLineCount())
		{
			pEdit->LineScroll(+2);
		}
		
	}
	*pResult = 0;
}



void CChattingDlg::OnBnClickedButton3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!OpenVideoWnd())
	{
		MessageBox(L"!");
	}
}


void CChattingDlg::OnEnChangeEdit2()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}


void CChattingDlg::OnBnClickedButton4()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	
	CEdit *pip, *pmask;
	CString ip, mask;
	char	ipA[20], maskA[20];
	ULONG	ipL, maskL;
	pip = (CEdit*)GetDlgItem(IDC_EDIT2);
	pmask = (CEdit*)GetDlgItem(IDC_EDIT3);
	if (!pip || !pmask)
		return;
	pip->GetWindowTextW(ip);
	pmask->GetWindowTextW(mask);
	if (ip == L"")
	{
		MessageBox(L"IP����Ϊ�գ�");
		return;
	}

	if (mask == L"")
	{
		MessageBox(L"����Ϊ�գ�");
		return;
	}

	W2C(ip.GetBuffer(0), ipA);
	W2C(mask.GetBuffer(0), maskA);
	ipL = inet_addr(ipA);
	maskL = inet_addr(maskA);
	m_startip	= ipL&maskL;
	m_endip		= (ipL&maskL) | (~maskL);
	m_ndealer.broadcast(m_endip, 0);
	AfxBeginThread(search_ip, this);
	return;
}
