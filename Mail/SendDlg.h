#pragma once


// SendDlg 对话框
#include "afxwin.h"
#include "smtpSock.h"

class SendDlg : public CDialog
{
	DECLARE_DYNAMIC(SendDlg)

public:
	SendDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~SendDlg();
	void Disp(LONG flag);
	void GetHeader();
	smtpSock smtpSocket;    //套接字类对象

// 对话框数据
	enum { IDD = IDD_SENDDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton2();
	CString m_Name;
	CString m_Addr;
	CString m_Server;
	UINT m_Port;
	CString m_User;
	CString m_Pass;
	CString m_Receiver;
	CString m_Title;
	CString m_CC;
	CString m_BCC;
	CString m_Attach;
	CString m_Letter;
	CString m_Info;
	afx_msg void OnBtnView();
	afx_msg void OnOK();
	 void InitSendDialog(void); //初始化
	 CString RC4_encrypt(CString letter); //信件加密的主要操作
private:
	CBase64 coder;
	BOOL GetBody(LPSTR& pszBody, int& nBodySize);
public:
	afx_msg void OnBtnWriteagain();
	//afx_msg void OnOK2();
//	afx_msg void OnBnClickedCheck1();
	BOOL m_encrypt;
	CButton m_ctrEncrypt;

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CEdit m_edit_Letter;
};
