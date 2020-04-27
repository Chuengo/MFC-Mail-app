// smtpSock.cpp : implementation file
//

#include "stdafx.h"
#include "Mail.h"
#include "smtpSock.h"
#include  "SendDlg.h"    //自己添加的包含语句
#include  "Base64.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAX_BUFF 20000
/////////////////////////////////////////////////////////////////////////////
// smtpSock

smtpSock::smtpSock()
{
	m_pDlg = NULL;
	state=smtpFIRST;
	//error="连接不上服务器\r\n";
}

smtpSock::~smtpSock()
{
	m_pDlg = NULL;
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(smtpSock, CAsyncSocket)
	//{{AFX_MSG_MAP(smtpSock)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// smtpSock member functions

//当套接字收到FD_CONNECT消息时，执行此函数，表明连接已建立
void smtpSock::OnConnect(int nErrorCode) 
{
	if(nErrorCode==0)  m_pDlg->Disp(S_CONNECT);
}

//当套接字收到服务器发来的数据时，执行此函数
void smtpSock::OnReceive(int nErrorCode) 
{
	if(nErrorCode==0) 
	{
		char buff[MAX_BUFF];             //接收缓冲区
		int rec=Receive(buff,MAX_BUFF);  //接收服务器发来的数据
		buff[rec]=NULL;                  //结尾置为NULL。
		lastMsg=buff;
		AnalyzeMsg();             //分析收到的数据，作不同的处理
	}  else  {
		error="接收期间发生了错误!\r\n";
		m_pDlg->Disp(S_CLOSE);   //显示信息
	}
}

//当关闭连接时，执行此函数
void smtpSock::OnClose(int nErrorCode) 
{
	if(nErrorCode==0)  m_pDlg->Disp(S_CLOSE);
}
//使套接字类的对话框指针变量指向主对话框
void smtpSock::SetParent(SendDlg *pDlg)
{
	m_pDlg = pDlg;
}

//退出服务器
void smtpSock::Close()
{
	CString str;
	str.Format("quit%c%c",13,10);
	Send((LPCSTR)str,str.GetLength());
	m_pDlg->Disp(S_CLOSE);
	state=smtpFIRST;
	CAsyncSocket::Close();
	error="与服务器的连接已经断开\r\n";
}

//分析服务器发来的数据，做出相应的处理
void smtpSock::AnalyzeMsg()
{
	CString s;
	strstream str;
	string check;
	str<<(LPCSTR)lastMsg;
	str>>check;

	//根据smtp会话所处的状态做出不同的处理	
	switch(state)
	{
	case smtpFIRST: //如果已经连接成功，类初始化的时候state为FIRST
		m_pDlg->Disp(S_RECEIVE); //发送消息到窗体
		if(!(check=="220")) {}
		s.Format("EHLO %s%c%c",m_pDlg->m_Name,13,10);
		Send((LPCSTR)s,s.GetLength()); //发送EHLO命令
       //SMTP客户机发送HELO命令给服务器，问候并告知SMTP服务器自己的域地址，与SMTP服务器通信。
		//如果服务器接收命令，成功则返回250响应码
		state=smtpEHLO;
		break;

		//向服务器发送经过Base64编码转换后的用户名和密码。
	  //如果验证成功，服务器向客户端发送一个OK命令，表示可以开始报文传输。
	case smtpEHLO:
		m_pDlg->Disp(S_RECEIVE);
		if(!(check=="250")) {}
		s.Format("AUTH LOGIN%c%c",13,10); 
		Send((LPCSTR)s,s.GetLength()); //发送AUTH LOGIN命令
		state=smtpAUTH;
		break;

	case smtpAUTH:
		m_pDlg->Disp(S_RECEIVE);
		if(!(check=="334")) {}
		coder.Encode(m_pDlg->m_User);
		s.Format(_T("%s%c%c"), coder.EncodedMessage(),13,10);
		Send((LPCSTR)s,s.GetLength()); //发送用户名
		state=smtpUSER; 
		break;

	case smtpUSER:
		m_pDlg->Disp(S_RECEIVE);
		if(!(check=="334")) {}
		
		coder.Encode(m_pDlg->m_Pass);
		s.Format(_T("%s%c%c"), coder.EncodedMessage(),13,10);
//	AfxMessageBox(s);
//		s.Format("NDkxMDE1%c%c",13,10); //491015
		Send((LPCSTR)s,s.GetLength()); //发送口令
		state=smtpPASS;
		break;

	case smtpPASS:
		m_pDlg->Disp(S_RECEIVE);
		if(!(check=="235")) {}
		s.Format("MAIL FROM:<%s>%c%c",m_pDlg->m_Addr,13,10); 
//		s.Format("MAIL FROM:<mu_lm@sina.com>%c%c",13,10); 
		Send((LPCSTR)s,s.GetLength()); //发送MAIL FROM命令
		//SMTP客户机发送MAIL命令格式，向服务器告知发信人的电子邮件地址，成功则返回250响应码
		state=smtpMAIL;
		break;
	case smtpMAIL:
		m_pDlg->Disp(S_RECEIVE);
		if(!(check=="250")) {}
//		s.Format("RCPT TO: <mu_lm@sina.com>%c%c",13,10); 
		s.Format("RCPT TO: <%s>%c%c",m_pDlg->m_Receiver,13,10); 
		Send((LPCSTR)s,s.GetLength()); //发送RECP命令
		//SMTP客户机再发送RCPT命令格式，告知服务器收信人的电子邮箱，成功则返回250响应码
		state=smtpRCPT;
		break;
	case smtpRCPT: 
		m_pDlg->Disp(S_RECEIVE);
		if(!(check=="250")) {}
		s.Format("DATA%c%c",13,10); 
		Send((LPCSTR)s,s.GetLength()); //发送DATA命令
		//SMTP客户机发送DATA命令格式，告知SMTP服务器，要开始发送邮件了，若成功，服务器回应354响应启动邮件输入
		state=smtpDATA;
		break;
	case smtpDATA: 
		//客户机发送电子邮件内容了，最终以<CRLF>.<CRLF>结束
		//当服务器接到结束符后，会答应250响应码标识发送成功
		m_pDlg->Disp(S_RECEIVE);
		if(!(check=="354")) {}

		m_pDlg->GetHeader();
		state=smtpQUIT;
		break;
	case smtpQUIT: 
		//发送QUIT命令格式结束会话，服务器一般以221响应来确认终止，并关闭TCP连接
		m_pDlg->Disp(S_RECEIVE);
		if(!(check=="250")) {}
		s.Format("QUIT%c%c",13,10); 
		Send((LPCSTR)s,s.GetLength()); //发送QUIT命令
		state=smtpDATA;
		break;
	default:
		m_pDlg->Disp(S_RECEIVE);
		break;
	}
}

