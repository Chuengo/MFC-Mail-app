#if !defined(AFX_pop3Sock_H__0444E4E0_0C28_11DA_BB87_AF2B44E1925E__INCLUDED_)
#define AFX_pop3Sock_H__0444E4E0_0C28_11DA_BB87_AF2B44E1925E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// pop3Sock.h : header file
//
#include <vector>
#include <strstream>
#include <string>

using namespace std;

class  ReceiveDlg;

/////////////////////////////////////////////////////////////////////////////
// pop3Sock command target

//显示不同信息的标志
#define S_CLOSE   1  //当关闭连接时，显示关闭信息
#define S_CONNECT 2  //当连接成功时，显示连接信息
#define S_RECEIVE 3  //当收到服务器发来的信息时，显示该信息
#define S_GETNUM  4 //当所有信件下载完毕后，显示信箱中的信件总数
#define S_GETSIZE 5 //当所有信件下载完毕后，显示信箱中的信件总大小
#define S_ENDRETR 6 //当所有信件下载完毕后，显示相关信息

//描述pop3会话状态的枚举类型
typedef enum {pop3FIRST=0,pop3USER,pop3PASS,pop3STAT,pop3LIST,pop3RETR,pop3ENDRETR,pop3DELE,pop3GOON} pop3STATE;

//用来缓存一封信件信息的结构
typedef struct 
{
	CString text;  //存储信件的文本
	int msgSize;   //信件的大小
	int	retrSize;  //信件实际下载的大小，在下载过程中动态变化
} MESSAGEPROP;

//自己的套接字类
class pop3Sock : public CAsyncSocket
{
// Attributes
public:
	CString lastMsg;
	CString outputMsg;
	CString error;
	int numMsg, sizeMsg, retrMsg;
	vector<MESSAGEPROP> msgs; //盛放信件信息的向量
	vector<MESSAGEPROP> rbmsgs; //盛放垃圾信件信息的向量

// Operations
public:
	//获得基本的邮件信头字段: From, To, Date, Subject, Body
	CString GetMsgStuff(int i); 
	CString GetMsgBody(int i);     //获得信件体
	CString GetMsgSubject(int i);  //获得信件标题
	CString GetMsg(int i);         //获得整封信的内容
	CString GetMsg_button(int i);
	CString GetRbMsg_button(int i);//获得垃圾邮件
	CString GetRbMsgBody(int i);     //获得信件体
	CString GetRbMsgSubject(int i);  //获得信件标题
	CString ReadLn(int index,CString src);
	bool isrubbish(CString lastMsg);//判断是否是垃圾邮件
	int rubbish_number;

	void Close();            //退出服务器
	void SetParent(ReceiveDlg *  pDlg);
	pop3Sock();
	virtual ~pop3Sock();
   CString RC4_decrypt(CString letter); //信件解密的主要操作

// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(pop3Sock)
	public:
	virtual void OnConnect(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(pop3Sock)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
private:
	void AnalyzeMsg();
	ReceiveDlg* m_pDlg;
	pop3STATE state;
	int delMsg;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_pop3Sock_H__0444E4E0_0C28_11DA_BB87_AF2B44E1925E__INCLUDED_)
