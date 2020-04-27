// SendDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Mail.h"
#include "SendDlg.h"
#include "afxdialogex.h"
#include "smtpSock.h"
#include "Base64.h"

#define SMTP_MAXLINE  76

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// SendDlg 对话框

IMPLEMENT_DYNAMIC(SendDlg, CDialog)

SendDlg::SendDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SendDlg::IDD, pParent)
	, m_Name(_T(""))
	, m_Addr(_T(""))
	, m_Server(_T(""))
	, m_Port(0)
	, m_User(_T(""))
	, m_Pass(_T(""))
	, m_Receiver(_T(""))
	, m_Title(_T(""))
	, m_CC(_T(""))
	, m_BCC(_T(""))
	, m_Attach(_T(""))
	, m_Letter(_T(""))
	, m_Info(_T(""))
	, m_encrypt(FALSE)
{

}

SendDlg::~SendDlg()
{
}

void SendDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_NAME, m_Name);
	DDX_Text(pDX, IDC_EDIT_ADDR, m_Addr);
	DDX_Text(pDX, IDC_EDIT_SERVER, m_Server);
	DDX_Text(pDX, IDC_EDIT_PORT, m_Port);
	DDX_Text(pDX, IDC_EDIT_USER, m_User);
	DDX_Text(pDX, IDC_EDIT_PASS, m_Pass);
	DDX_Text(pDX, IDC_EDIT_RECEIVER, m_Receiver);
	DDX_Text(pDX, IDC_EDIT_TITLE, m_Title);
	DDX_Text(pDX, IDC_EDIT_CC, m_CC);
	DDX_Text(pDX, IDC_EDIT_BCC, m_BCC);
	DDX_Text(pDX, IDC_EDIT_ATTACH, m_Attach);
	DDX_Text(pDX, IDC_EDIT_LETTER, m_Letter);
	DDX_Text(pDX, IDC_RICH_LIST, m_Info);
	DDX_Check(pDX, IDC_CHECK1, m_encrypt);
	DDX_Control(pDX, IDC_CHECK1, m_ctrEncrypt);
	DDX_Control(pDX, IDC_EDIT_LETTER, m_edit_Letter);
}


BEGIN_MESSAGE_MAP(SendDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON2, &SendDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BTN_VIEW, &SendDlg::OnBtnView)
	ON_BN_CLICKED(IDOK, &SendDlg::OnOK)
	ON_BN_CLICKED(IDCANCEL, &SendDlg::OnBtnWriteagain)
//	ON_BN_CLICKED(IDOK2, &SendDlg::OnOK2)
//	ON_BN_CLICKED(IDC_CHECK1, &SendDlg::OnBnClickedCheck1)
END_MESSAGE_MAP()


// SendDlg 消息处理程序


void SendDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
}


void SendDlg::OnBtnView()
{
  UpdateData(TRUE); 
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("All Files (*.*)|*.*||"));
	if (dlg.DoModal() == IDOK) 
	{
		CString sNewFile = dlg.GetPathName();
		if (m_Attach.GetLength())
		{
			m_Attach += _T(", ");
			m_Attach += sNewFile;
		}
		else
			m_Attach = sNewFile;
		UpdateData(FALSE);
	}
	LPSTR pszBody = NULL;
	int nBodySize = 0;
	if (!GetBody(pszBody, nBodySize))
	{
		TRACE(_T("Failed in call to send body parts body, GetLastError returns: %d\n"), GetLastError());
	}
	CString s;
	s = pszBody;
	m_Info += s;
	UpdateData(FALSE);

}


void SendDlg::OnOK()
{
	// TODO: 在此添加控件通知处理程序代码
		// TODO: 在此添加控件通知处理程序代码
	smtpSocket.SetParent(this);
	//GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
	UpdateData(TRUE);           //取来用户在对话框中输入的数据
	smtpSocket.Create();        //创建套接字对象的底层套接字
	smtpSocket.Connect((LPCSTR)m_Server, m_Port); //连接pop3服务器
    GetDlgItem(IDOK)->EnableWindow(FALSE);
}
//根据不同的情况，向用户显示不同的信息
void SendDlg::Disp(LONG flag)
{
	CString s;
	switch(flag)
	{
	case S_CONNECT: //已连接到服务器，显示信息
		s = "已经连接到"+                                                    m_Server+"服务器\r\n";
		m_Info += s;
		//m_listInfo.AddString(s);
		break;
	case S_RECEIVE: //收到服务器发来的数据,显示该数据
		m_Info += smtpSocket.lastMsg;
		//m_listInfo.AddString(smtpSocket.lastMsg);
		break; 
	case S_CLOSE: //显示关闭连接的信息
		m_Info += smtpSocket.error;
//		m_listInfo.AddString(smtpSocket.error);
		s = "连接已经关闭\r\n";
		m_Info += s;
		//m_listInfo.AddString(s); 
		break;
	}
	UpdateData(FALSE);   //更新用户界面
}

void SendDlg::GetHeader()  //获取信件的内容
{
	UpdateData(TRUE);
	CString sepa;
	CString sReply;
	sReply = _T("");

	//创建 "Date:" 标题行内容
	CTime now(CTime::GetCurrentTime());
	CString sDate(now.Format(_T("%a, %d %b %Y %H:%M:%S ")));
	sDate +="+0800 (CST)";

	CString sBuf(sReply);
	if (m_Attach.GetLength())
	{
		sReply.Format(_T("MIME-Version: 1.0\r\n"));
		sBuf += sReply;
	}

	//添加 From 和 to 字段
	//From: "=?GB2312?B?XXXXXXXXXX=?=" // 发信人，进行了编码
	//To: Li@163.com 

	coder.Encode(m_Addr);
	sReply.Format(_T("From: %s\r\n"), m_Addr);
//	sReply.Format(_T("From: =?gb2312?B?%s?=\r\n"), coder.EncodedMessage());
	sBuf += sReply;
	
	sReply.Format(_T("To: %s\r\n"),m_Receiver);
	sBuf += sReply;

	//添加 Date字段
	//Date: Wed, 8 Apr 2004 16:16:16 +0800 // 发信的日期时间
	sReply.Format(_T("Date: %s\r\n"),sDate);
	sBuf += sReply;

	//添加 subject字段
	//Subject: =?GB2312?B?XXXXXX=?= // 主题，进行了编码
		if(m_encrypt) {
			CString jiami;
				jiami.Format("(有内鬼，加密邮件)");
				m_Title +=jiami;
			coder.Encode(m_Title);
			sReply.Format(_T("Subject: %s\r\n"),m_Title);
		}
		else {
			coder.Encode(m_Title);
				sReply.Format(_T("Subject: %s\r\n"),m_Title);
		}

//	sReply.Format(_T("Subject: =?gb2312?B?%s?=\r\n"),coder.EncodedMessage());
	sBuf += sReply;

	//如果有，添加 Cc 字段
	if (m_CC.GetLength())
	{
		sReply.Format(_T("Cc: %s\r\n"), m_CC);
		sBuf += sReply;
	}

	//如果有，添加Bcc 字段
	if (m_BCC.GetLength())
	{
		sReply.Format(_T("Bcc: %s\r\n"), m_BCC);
		sBuf += sReply;
	}

	//如果需要，添加 Mime 字段
	//MIME-Version: 1.0               // MIME版本
	//Content-type: multipart/mixed;  // 内容类型是多部分/混合型
	//boundary = "NextPart_000_00A"  // 指定一级边界特征字符串
	
	sepa= _T("Boundary-=_HfNFaIwtPvzJDUQrvChaEKIMklNx");
	if (m_Attach.GetLength())
	{
//		sReply.Format(_T("MIME-Version: 1.0\r\n"));
//		sBuf += sReply;
		sReply.Format("Content-Type:Multipart/mixed;boundary=%s\r\n",sepa);
		sBuf += sReply;
		sBuf += _T("\r\n");
	} else {
		sBuf += _T("\r\n");
		if(m_encrypt) {
			sReply.Format(_T(" %s\r\n") ,RC4_encrypt(m_Letter));
		}
		else {
			sReply.Format(_T(" %s\r\n") ,m_Letter);
		}
		sBuf += sReply;
		sReply.Format(_T("%c%c.%c%c"),13,10,13,10);
		sBuf += sReply;
	}
	smtpSocket.Send((LPCSTR)sBuf,sBuf.GetLength());
	m_Info+=sBuf;

	if (m_Attach.GetLength())
	{
		sReply.Format(_T("--%s\r\n"),sepa);
		sBuf = sReply;	
		
		sBuf += _T("Content-Type: text/plain; charset='gb2312'\r\n");
		sBuf += _T("Content-Transfer-Encoding: base64\r\n");
		sBuf += _T("\r\n");
		
		coder.Encode(m_Letter);
		sReply.Format(_T("%s\r\n"),coder.EncodedMessage());
		sBuf += sReply;
		
		sReply.Format(_T("--%s\r\n"), sepa);
		sBuf += sReply;

		sBuf += _T("Content-Type: text/plain; charset='gb2312'\r\n");
		sBuf += _T("Content-Transfer-Encoding: base64\r\n");
		sBuf += _T("\r\n");
		
		//add 附件到Letter
		LPSTR pszBody = NULL;
		int nBodySize = 0;
		if (!GetBody(pszBody, nBodySize))
		{
			TRACE(_T("Failed in call to send body parts body, GetLastError returns: %d\n"), GetLastError());
		}
		sReply = pszBody;
		sBuf += sReply;
		sReply.Format(_T("--%s\r\n"), sepa);
		sBuf += sReply;
		
		sReply.Format(_T("%c%c.%c%c"),13,10,13,10);  //13是回车键 10是换行键
		sBuf += sReply;
		smtpSocket.Send((LPCSTR)sBuf,sBuf.GetLength());
		m_Info+=sBuf;
	}
	UpdateData(FALSE);
}

BOOL SendDlg::GetBody(LPSTR& pszBody, int& nBodySize) //获取附件内容
{
	BOOL bSuccess = FALSE;

	//打开附件文件
	CFile infile;
	if (infile.Open(m_Attach, CFile::modeRead | CFile::shareDenyWrite))
	{
		DWORD dwSize = infile.GetLength();
		if (dwSize)
		{
			//读入数据
			BYTE* pszIn = new BYTE[dwSize];
			try
			{
				infile.Read(pszIn, dwSize);
				bSuccess = TRUE;
			}
			catch(CFileException* pEx)
			{
				bSuccess = FALSE;
				pEx->Delete();
			}

			if (bSuccess)
			{
				coder.Encode(pszIn, dwSize); //编码
				delete [] pszIn;               //删除了输入缓冲区
				infile.Close();                //关闭输入文件

				//形成编码后的发送内容
				LPSTR pszEncoded = coder.EncodedMessage();
				int nEncodedSize = coder.EncodedMessageSize();
				nBodySize = nEncodedSize + (((nEncodedSize/76)+1)*2) + 1;
				pszBody = new char[nBodySize];
				--nBodySize; 

				int nInPos = 0;
				int nOutPos = 0;
				while (nInPos < nEncodedSize)
				{
					int nThisLineSize = min(nEncodedSize - nInPos, SMTP_MAXLINE);
					CopyMemory(&pszBody[nOutPos], &pszEncoded[nInPos], nThisLineSize);
					nOutPos += nThisLineSize;
					CopyMemory(&pszBody[nOutPos], "\r\n", 2);
					nOutPos += 2;
					nInPos += nThisLineSize;
				}
				pszBody[nOutPos] = '\0'; //以空字符串结束
			}
		}  else  {
			bSuccess = TRUE;
			pszBody = NULL;
			nBodySize = 0;
		}
	}  else
		TRACE(_T("No bodypart body text or filename specified!\n"));

	return bSuccess;
}

void SendDlg::InitSendDialog(void)
{
	m_Name = _T("Kinson");               //发信人
	m_Addr = _T("wongkinson@163.com");     //发信地址
	m_Server = _T("smtp.163.com");   //smtp服务器
	m_Port = 25;                     //smtp的保留端口
	m_User = _T("wongkinson@163.com");             //用户名
	m_Pass = _T("");           //口令

	m_Receiver = _T("18928791752@163.com"); //收信人地址
	m_Title = _T("");            //主题
	m_CC = _T("");                   //抄送
	m_BCC = _T("");                  //暗送
	m_Letter = _T("");               //信件内容
	m_Attach = _T("");               //附件
	UpdateData(FALSE);               //更新用户界面
}

//RC4加密
CString SendDlg::RC4_encrypt(CString letter)
{
	string str;
    CString encrypt ;
	str = CA2A(letter.GetString()); //加密的明文
	unsigned char Tbox[256] = {0x00}; //全部初始化为0
	unsigned char Sbox[256] = {0x00};//全部初始化为0
	char *k="wjl";//使用密钥wjl进行加密
	int len=strlen(k); //获取密钥长度
    for(int i=0;i<256;i++) Sbox[i]=i; //初始化S
	if(len>=256) for(int i=0;i<256;i++) Tbox[i]=k[i];   //密钥扩展到盒中
	if(len<256) for(int i=0;i<256;i++) Tbox[i]=k[i%len];
	int j=0; 
	unsigned char swap;
	int n;
	for(int i=0;i<256;i++)  //S和T的初始置换
	{
		n=j+(int)Sbox[i]+(int)Tbox[i]; 
		j=n%256;
		swap=Sbox[i];
		Sbox[i]=Tbox[i];
		Tbox[i]=swap;
	}
	//实现加密
	  int i=0;
      j=0;
	  int t;
	  for (int q = 0; q != str.length(); ++q){
	  char  ch = str.at(q);
		  i=(i+1)%256;
		   j=(j+Sbox[i])%256;
		   swap=Sbox[i];
		   Sbox[i]=Sbox[j];
		   Sbox[j]=swap;
		   int t1=(int)Sbox[i]+(int)Sbox[j];
		   t=t1%256;
		   char k1=Sbox[t];          //k1为经过多步操作置换得到的密钥
		   char cipherchar = ch^k1; //cipherchar为得到的密文
		   str.insert(q,1,cipherchar);
		   str.erase(q + 1, 1);
	  }
	  encrypt += CA2A(str.c_str());
	  return encrypt;


}

void SendDlg::OnBtnWriteagain()
{
	// TODO: 在此添加控件通知处理程序代码
	::PostMessage(AfxGetMainWnd()->m_hWnd,WM_SYSCOMMAND,SC_CLOSE,NULL);
    //获取exe程序当前路径
    extern CMailApp theApp;
    TCHAR szAppName[MAX_PATH];
    :: GetModuleFileName(theApp.m_hInstance, szAppName, MAX_PATH);
    CString strAppFullName;
    strAppFullName.Format(_T("%s"),szAppName);
    //重启程序
    STARTUPINFO StartInfo;
    PROCESS_INFORMATION procStruct;
    memset(&StartInfo, 0, sizeof(STARTUPINFO));
    StartInfo.cb = sizeof(STARTUPINFO);
    ::CreateProcess(
        (LPCTSTR)strAppFullName,
        NULL,
        NULL,
        NULL,
        FALSE,
        NORMAL_PRIORITY_CLASS,
        NULL,
        NULL,
        &StartInfo,
        &procStruct);
}



BOOL SendDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	CString s;
	if(pMsg->message== WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{ 
		case VK_RETURN: //截获回车 
			if(GetDlgItem(IDC_EDIT_LETTER) == GetFocus())
			{ 

		    m_edit_Letter.GetWindowText(m_Letter);
            // 给显示字符串添加回车和换行
            m_Letter += "\r\n";
		    m_edit_Letter.SetWindowText(m_Letter);
            int len = m_Letter.GetLength();
			m_edit_Letter.SetSel(len,len,false);
			m_edit_Letter.SetFocus();

			}
			return true;
			break;
		case VK_ESCAPE:
		        	s="\r\n";
					m_Letter += s;   //换行
                 UpdateData(true);  
			return true;
			break;
		default:
			break;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}
