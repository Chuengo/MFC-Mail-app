#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "pop3Sock.h"

// ReceiveDlg 对话框
class ReceiveDlg : public CDialog
{
	DECLARE_DYNAMIC(ReceiveDlg)

public:
	void Disp(LONG flag);  //在不同的会话阶段显示不同的信息
	void InitReceiveDialog(void);//初始化函数
	ReceiveDlg(CWnd* pParent = NULL);   // 标准构造函数
	pop3Sock pop3Socket;     //套接字类对象实例
	virtual ~ReceiveDlg();

// 对话框数据
	enum { IDD = IDD_RECEIVEDLG };

// 实现
protected:
	HICON m_hIcon;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton4();
	CString m_strUser;
	CString m_strServer;
	CString m_strPass;
//	BOOL m_boolDel;
	CComboBox m_ctrList;
	CString m_Info;
	CRichEditCtrl m_ctrInfo;
	afx_msg void OnBtnConn();
	afx_msg void OnBtnDisc();
	afx_msg void OnBtnView();
	afx_msg void OnBtnSave();
	BOOL m_bolDel;
	bool isEncrypt(CString Msgbody);
	afx_msg void OnBtnRb();
};
