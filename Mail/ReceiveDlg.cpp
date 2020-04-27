// ReceiveDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Mail.h"
#include "ReceiveDlg.h"
#include "afxdialogex.h"


// ReceiveDlg 对话框

IMPLEMENT_DYNAMIC(ReceiveDlg, CDialog)

ReceiveDlg::ReceiveDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ReceiveDlg::IDD, pParent)
	, m_strUser(_T(""))
	, m_strServer(_T(""))
	, m_strPass(_T(""))
	, m_bolDel(FALSE)
	, m_Info(_T(""))
{

}

ReceiveDlg::~ReceiveDlg()
{
}

void ReceiveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_USER, m_strUser);
	DDX_Text(pDX, IDC_EDIT_SERVER, m_strServer);
	DDX_Text(pDX, IDC_EDIT_PASS, m_strPass);
	DDX_Check(pDX, IDC_CHECK_DEL, m_bolDel);
	DDX_Control(pDX, IDC_COMB_LIST, m_ctrList);
	DDX_Text(pDX, IDC_RICH_INFO, m_Info);
	DDX_Control(pDX, IDC_RICH_INFO, m_ctrInfo);
}


BEGIN_MESSAGE_MAP(ReceiveDlg, CDialog)
	//ON_BN_CLICKED(IDC_BUTTON4, &ReceiveDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BTN_CONN, &ReceiveDlg::OnBtnConn)
//	ON_BN_CLICKED(IDC_BTN_DISC, &ReceiveDlg::OnBtNdisc)
ON_BN_CLICKED(IDC_BTN_DISC, &ReceiveDlg::OnBtnDisc)
ON_BN_CLICKED(IDC_BTN_VIEW, &ReceiveDlg::OnBtnView)
ON_BN_CLICKED(IDC_BTN_SAVE, &ReceiveDlg::OnBtnSave)
ON_BN_CLICKED(IDC_BTN_RB, &ReceiveDlg::OnBtnRb)
END_MESSAGE_MAP()


// ReceiveDlg 消息处理程序


/*void ReceiveDlg::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
}*/

//以下是事件处理函数和其他成员函数的代码
//当用户点击"连接"按钮时，执行此函数
void ReceiveDlg::OnBtnConn()
{
	// TODO: 在此添加控件通知处理程序代码
		//设定pop3类的对话框指针变量，使之指向本对话框，以便传递信息
	pop3Socket.SetParent(this); 
	UpdateData(TRUE);           //取来用户在对话框中输入的数据
	InitReceiveDialog();
	pop3Socket.Create();        //创建套接字对象的底层套接字
	pop3Socket.Connect((LPCSTR)m_strServer,110); //连接pop3服务器
	m_Info = "";                //多文本列表框清空
	GetDlgItem(IDC_RICH_INFO)->SetWindowText(_T("")); 
	((CComboBox*)GetDlgItem(IDC_COMB_LIST))->ResetContent();
	GetDlgItem(IDC_BTN_VIEW)->EnableWindow(TRUE);   //查看邮件按钮
	GetDlgItem(IDC_BTN_SAVE)->EnableWindow(TRUE);   //存储按钮
	GetDlgItem(IDC_BTN_DISC)->EnableWindow(TRUE);   //断开按钮
	GetDlgItem(IDC_BTN_RB)->EnableWindow(TRUE);   //垃圾箱按钮
	UpdateData(FALSE);          //更新用户界面
}



//当用户点击"断开"按钮时，执行此函数
void ReceiveDlg::OnBtnDisc()
{
	// TODO: 在此添加控件通知处理程序代码
		    pop3Socket.Close();
	     //启用连接按钮
		GetDlgItem(IDC_BTN_CONN)->EnableWindow(TRUE);
		//启用断开按钮
		GetDlgItem(IDC_BTN_DISC)->EnableWindow(TRUE); 
		//启用查看邮件按钮
		GetDlgItem(IDC_BTN_VIEW)->EnableWindow(TRUE);
			GetDlgItem(IDC_BTN_RB)->EnableWindow(TRUE);   //垃圾箱按钮
		((CComboBox*)GetDlgItem(IDC_COMB_LIST))->ResetContent();
		GetDlgItem(IDC_RICH_INFO)->SetWindowText(_T("")); 
	   UpdateData(FALSE);
}

//当用户点击"查看邮件"按钮时，执行此函数
void ReceiveDlg::OnBtnView()
{
	// TODO: 在此添加控件通知处理程序代码
		int i;
	i = m_ctrList.GetCurSel();  //获得用户在组合框中的选择
	//获得并显示指定信件的基本属性及信件内容
      m_Info = "";
	 UpdateData(FALSE);
	CString rb=pop3Socket.GetMsgSubject(i);
	CString s;
	s.Format("Content:");
	m_Info += pop3Socket.GetMsg_button(i);
	m_Info +=pop3Socket.GetMsgSubject(i);   //加上标题函数
	m_Info +="\n"; //加上换行
	m_Info += s;
	if(isEncrypt(pop3Socket.GetMsgSubject(i))) {
	s=pop3Socket.RC4_decrypt(pop3Socket.GetMsgBody(i));
	//  int index_first;
	  //int index_last;
	
	  s=s.Left(s.GetLength()-2);
	m_Info +=s;
	}
	else {
		m_Info += pop3Socket.GetMsgBody(i);
	}

	/*s.Format("第 %d 封信 %c%c",i,13,10);
	  m_Info +=s;
	  m_Info += pop3Socket.msgs[i].text;*/
	UpdateData(FALSE);
}

//点击存储时
void ReceiveDlg::OnBtnSave()
{
	// TODO: 在此添加控件通知处理程序代码
	int i;
	CString s;
	i = m_ctrList.GetCurSel();
	//获得指定信件的基本属性及信件内容
	s = pop3Socket.GetMsgStuff(i);
	s += pop3Socket.GetMsgBody(i);
	
	CFileDialog dlg(FALSE);
	if(dlg.DoModal()==IDOK)
	{
		CFile file(dlg.GetPathName(),
			CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
		file.Write((LPCSTR)s,s.GetLength());
		file.Close();
	}
}

//在不同的情况下，向用户显示不同的信息
void ReceiveDlg::Disp(LONG flag)
{
	CString s;
	switch(flag)
	{
	case S_CONNECT:
		//已连接到服务器，显示信息
		s = "已连接到"+m_strServer+"...\r\n";
		m_Info+=s;
		//禁用连接按钮
		GetDlgItem(IDC_BTN_CONN)->EnableWindow(FALSE);
		//启用断开按钮
		GetDlgItem(IDC_BTN_DISC)->EnableWindow(TRUE); 
		//禁用查看邮件按钮
		GetDlgItem(IDC_BTN_VIEW)->EnableWindow(FALSE);
		break;
    case S_RECEIVE: //收到服务器发来的响应，显示它的内容
		//m_Info+=pop3Socket.lastMsg;
		  
		break; 
	case S_CLOSE: //显示关闭连接的信息
		m_Info+=pop3Socket.error;
		s = "连接已经关闭\r\n";
		m_Info+=s;
		//启用连接按钮
		GetDlgItem(IDC_BTN_CONN)->EnableWindow(TRUE);
		//禁用断开按钮
	  GetDlgItem(IDC_BTN_DISC)->EnableWindow(FALSE);
		break;
	case S_GETNUM: //已经知道了邮箱中信件的总数
		s.Format("邮箱中共有 %d 封信\r\n",pop3Socket.numMsg);
		m_Info+=s;
		break;
	case S_GETSIZE: //显示信件的大小
		s.Format("信箱中信件大小为：%d字节\r\n",pop3Socket.sizeMsg);
		m_Info+=s;
		break;
	case S_ENDRETR: //所有信件已下载完毕，显示信息
		s.Format("共获取了 %d 封信件\r\n",pop3Socket.msgs.size()-pop3Socket.rubbish_number);
		s.Format("其中拦截垃圾邮件 %d 封\r\n",pop3Socket.rubbish_number-1);
		//pop3Socket.rubbish_number=0; //重新归0 下次连接可用
		m_Info+=s;
		//如果有信件下载，启用查看邮件按钮
		if((pop3Socket.msgs.size())>0)
			GetDlgItem(IDC_BTN_VIEW)->EnableWindow(TRUE);
		break;
	}
	UpdateData(FALSE);   //更新用户界面
}
void ReceiveDlg::InitReceiveDialog(void)
{
	m_strServer = _T("pop.163.com");   //POP3服务器地址
	m_strUser = _T("wongkinson@163.com");               //邮箱的用户名
	m_strPass = _T("");              //口令
	m_Info = _T("");                       //多文本框清空
	m_ctrList.ResetContent();
	if(pop3Socket.msgs.empty())
	{
		pop3Socket.msgs.clear();
	}
	GetDlgItem(IDC_BTN_RB)->EnableWindow(FALSE);   //禁用垃圾箱按钮
	GetDlgItem(IDC_BTN_VIEW)->EnableWindow(FALSE);   //禁用查看邮件按钮
	GetDlgItem(IDC_BTN_SAVE)->EnableWindow(FALSE);   //禁用存储按钮
	GetDlgItem(IDC_BTN_DISC)->EnableWindow(FALSE);   //禁用断开按钮
	UpdateData(FALSE);
}
bool ReceiveDlg::isEncrypt(CString Msgbody)
{
	int index;

	index = Msgbody.Find("加密");
	if(index != -1) {
		return true;
	}
	else return false;

}
//按下垃圾箱
void ReceiveDlg::OnBtnRb()
{
	// TODO: 在此添加控件通知处理程序代码
	m_Info = "";                //多文本列表框清空
	 UpdateData(FALSE);
	int i=0;
	CString num;
	num.Format("共有垃圾邮件 %d 封 %c%c",pop3Socket.rubbish_number-1,13,10);
	 m_Info +=num;
	for(i=0;i<pop3Socket.rubbish_number-1;i++)
	{
	CString rb;
	  rb.Format("%c%c第 %d 封信 %c%c",13,10,i+1,13,10);
	  m_Info +=rb;
	CString s;
	m_Info += pop3Socket.GetRbMsg_button(i);
	m_Info +=pop3Socket.GetRbMsgSubject(i);   //加上标题函数
	m_Info +="\n"; //加上换行
    	s.Format("Content:");
	m_Info += s;
		m_Info += pop3Socket.GetRbMsgBody(i);
	}
	UpdateData(FALSE);
}
