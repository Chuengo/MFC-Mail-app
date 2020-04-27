// SendDlg.cpp : ʵ���ļ�
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

// SendDlg �Ի���

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


// SendDlg ��Ϣ�������


void SendDlg::OnBnClickedButton2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
		// TODO: �ڴ���ӿؼ�֪ͨ����������
	smtpSocket.SetParent(this);
	//GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
	UpdateData(TRUE);           //ȡ���û��ڶԻ��������������
	smtpSocket.Create();        //�����׽��ֶ���ĵײ��׽���
	smtpSocket.Connect((LPCSTR)m_Server, m_Port); //����pop3������
    GetDlgItem(IDOK)->EnableWindow(FALSE);
}
//���ݲ�ͬ����������û���ʾ��ͬ����Ϣ
void SendDlg::Disp(LONG flag)
{
	CString s;
	switch(flag)
	{
	case S_CONNECT: //�����ӵ�����������ʾ��Ϣ
		s = "�Ѿ����ӵ�"+                                                    m_Server+"������\r\n";
		m_Info += s;
		//m_listInfo.AddString(s);
		break;
	case S_RECEIVE: //�յ�����������������,��ʾ������
		m_Info += smtpSocket.lastMsg;
		//m_listInfo.AddString(smtpSocket.lastMsg);
		break; 
	case S_CLOSE: //��ʾ�ر����ӵ���Ϣ
		m_Info += smtpSocket.error;
//		m_listInfo.AddString(smtpSocket.error);
		s = "�����Ѿ��ر�\r\n";
		m_Info += s;
		//m_listInfo.AddString(s); 
		break;
	}
	UpdateData(FALSE);   //�����û�����
}

void SendDlg::GetHeader()  //��ȡ�ż�������
{
	UpdateData(TRUE);
	CString sepa;
	CString sReply;
	sReply = _T("");

	//���� "Date:" ����������
	CTime now(CTime::GetCurrentTime());
	CString sDate(now.Format(_T("%a, %d %b %Y %H:%M:%S ")));
	sDate +="+0800 (CST)";

	CString sBuf(sReply);
	if (m_Attach.GetLength())
	{
		sReply.Format(_T("MIME-Version: 1.0\r\n"));
		sBuf += sReply;
	}

	//��� From �� to �ֶ�
	//From: "=?GB2312?B?XXXXXXXXXX=?=" // �����ˣ������˱���
	//To: Li@163.com 

	coder.Encode(m_Addr);
	sReply.Format(_T("From: %s\r\n"), m_Addr);
//	sReply.Format(_T("From: =?gb2312?B?%s?=\r\n"), coder.EncodedMessage());
	sBuf += sReply;
	
	sReply.Format(_T("To: %s\r\n"),m_Receiver);
	sBuf += sReply;

	//��� Date�ֶ�
	//Date: Wed, 8 Apr 2004 16:16:16 +0800 // ���ŵ�����ʱ��
	sReply.Format(_T("Date: %s\r\n"),sDate);
	sBuf += sReply;

	//��� subject�ֶ�
	//Subject: =?GB2312?B?XXXXXX=?= // ���⣬�����˱���
		if(m_encrypt) {
			CString jiami;
				jiami.Format("(���ڹ������ʼ�)");
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

	//����У���� Cc �ֶ�
	if (m_CC.GetLength())
	{
		sReply.Format(_T("Cc: %s\r\n"), m_CC);
		sBuf += sReply;
	}

	//����У����Bcc �ֶ�
	if (m_BCC.GetLength())
	{
		sReply.Format(_T("Bcc: %s\r\n"), m_BCC);
		sBuf += sReply;
	}

	//�����Ҫ����� Mime �ֶ�
	//MIME-Version: 1.0               // MIME�汾
	//Content-type: multipart/mixed;  // ���������Ƕಿ��/�����
	//boundary = "NextPart_000_00A"  // ָ��һ���߽������ַ���
	
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
		
		//add ������Letter
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
		
		sReply.Format(_T("%c%c.%c%c"),13,10,13,10);  //13�ǻس��� 10�ǻ��м�
		sBuf += sReply;
		smtpSocket.Send((LPCSTR)sBuf,sBuf.GetLength());
		m_Info+=sBuf;
	}
	UpdateData(FALSE);
}

BOOL SendDlg::GetBody(LPSTR& pszBody, int& nBodySize) //��ȡ��������
{
	BOOL bSuccess = FALSE;

	//�򿪸����ļ�
	CFile infile;
	if (infile.Open(m_Attach, CFile::modeRead | CFile::shareDenyWrite))
	{
		DWORD dwSize = infile.GetLength();
		if (dwSize)
		{
			//��������
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
				coder.Encode(pszIn, dwSize); //����
				delete [] pszIn;               //ɾ�������뻺����
				infile.Close();                //�ر������ļ�

				//�γɱ����ķ�������
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
				pszBody[nOutPos] = '\0'; //�Կ��ַ�������
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
	m_Name = _T("Kinson");               //������
	m_Addr = _T("wongkinson@163.com");     //���ŵ�ַ
	m_Server = _T("smtp.163.com");   //smtp������
	m_Port = 25;                     //smtp�ı����˿�
	m_User = _T("wongkinson@163.com");             //�û���
	m_Pass = _T("");           //����

	m_Receiver = _T("18928791752@163.com"); //�����˵�ַ
	m_Title = _T("");            //����
	m_CC = _T("");                   //����
	m_BCC = _T("");                  //����
	m_Letter = _T("");               //�ż�����
	m_Attach = _T("");               //����
	UpdateData(FALSE);               //�����û�����
}

//RC4����
CString SendDlg::RC4_encrypt(CString letter)
{
	string str;
    CString encrypt ;
	str = CA2A(letter.GetString()); //���ܵ�����
	unsigned char Tbox[256] = {0x00}; //ȫ����ʼ��Ϊ0
	unsigned char Sbox[256] = {0x00};//ȫ����ʼ��Ϊ0
	char *k="wjl";//ʹ����Կwjl���м���
	int len=strlen(k); //��ȡ��Կ����
    for(int i=0;i<256;i++) Sbox[i]=i; //��ʼ��S
	if(len>=256) for(int i=0;i<256;i++) Tbox[i]=k[i];   //��Կ��չ������
	if(len<256) for(int i=0;i<256;i++) Tbox[i]=k[i%len];
	int j=0; 
	unsigned char swap;
	int n;
	for(int i=0;i<256;i++)  //S��T�ĳ�ʼ�û�
	{
		n=j+(int)Sbox[i]+(int)Tbox[i]; 
		j=n%256;
		swap=Sbox[i];
		Sbox[i]=Tbox[i];
		Tbox[i]=swap;
	}
	//ʵ�ּ���
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
		   char k1=Sbox[t];          //k1Ϊ�����ಽ�����û��õ�����Կ
		   char cipherchar = ch^k1; //ciphercharΪ�õ�������
		   str.insert(q,1,cipherchar);
		   str.erase(q + 1, 1);
	  }
	  encrypt += CA2A(str.c_str());
	  return encrypt;


}

void SendDlg::OnBtnWriteagain()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	::PostMessage(AfxGetMainWnd()->m_hWnd,WM_SYSCOMMAND,SC_CLOSE,NULL);
    //��ȡexe����ǰ·��
    extern CMailApp theApp;
    TCHAR szAppName[MAX_PATH];
    :: GetModuleFileName(theApp.m_hInstance, szAppName, MAX_PATH);
    CString strAppFullName;
    strAppFullName.Format(_T("%s"),szAppName);
    //��������
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
	// TODO: �ڴ����ר�ô����/����û���
	CString s;
	if(pMsg->message== WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{ 
		case VK_RETURN: //�ػ�س� 
			if(GetDlgItem(IDC_EDIT_LETTER) == GetFocus())
			{ 

		    m_edit_Letter.GetWindowText(m_Letter);
            // ����ʾ�ַ�����ӻس��ͻ���
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
					m_Letter += s;   //����
                 UpdateData(true);  
			return true;
			break;
		default:
			break;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}
