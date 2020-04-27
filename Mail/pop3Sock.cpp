// pop3Sock.cpp : 实现文件
//

#include "stdafx.h"
#include "Mail.h"
#include "pop3Sock.h"
#include "ReceiveDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAX_BUFF 20000
// pop3Sock

pop3Sock::pop3Sock()
{
	m_pDlg = NULL;
	state=pop3FIRST;
	rubbish_number=0;
	error="Not connected to server\r\n";
}

pop3Sock::~pop3Sock()
{
		m_pDlg = NULL;
}


// pop3Sock 成员函数


void pop3Sock::OnClose(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类

	if(nErrorCode==0)  m_pDlg->Disp(S_CLOSE);
}


void pop3Sock::OnConnect(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类
		if(nErrorCode==0)  m_pDlg->Disp(S_CONNECT);
}


void pop3Sock::OnReceive(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(nErrorCode==0) 
	{
		char buff[MAX_BUFF];            //接收缓冲区
		int rec=Receive(buff,MAX_BUFF); //接收服务器发来的数据
		buff[rec]=NULL;                  //结尾置为NULL。
		lastMsg=buff;
		outputMsg=buff;
		AnalyzeMsg();                      //分析收到的数据，作不同的处理
	}  else  {
		error="在接收数据时发送了错误!\r\n";
		m_pDlg->Disp(S_CLOSE);   //显示信息
	}
}
//设置套接字类的对话框指针变量
void pop3Sock::SetParent(ReceiveDlg *pDlg)
{
	m_pDlg = pDlg;
}


//输出在点击查看邮件按钮的内容
CString pop3Sock::GetMsg_button(int i)  //0开始
{
	CString ret;
    CString s;
	int where=msgs[i].text.Find("From:");  //0开始 
	s.Format("From:位置：%d",where);//数字  得到位置
	if (where!=-1)
	{
		ret+=ReadLn(where,msgs[i].text);//从src串中，从index所指的位置开始，取子字串，直到行末符"\r"为止。
		ret+="\n"; //加上换行
		//写入ret 最后返回的东西
	}
	where=msgs[i].text.Find("To:");
	s.Format("To:位置：%d",where);
	if (where!=-1)
	{
		ret+=ReadLn(where,msgs[i].text);
		ret+="\n"; //加上换行
	}
		where=msgs[i].text.Find("Date:");
        s.Format("Date:位置：%d",where);
	if (where!=-1)
	{
		ret+=ReadLn(where,msgs[i].text);	
		ret+="\n";
	}
	return ret;
}
//输出在点击垃圾箱按钮的内容
CString pop3Sock::GetRbMsg_button(int i)  //0开始
{
	CString ret;
    CString s;
	int where=rbmsgs[i].text.Find("From:");  //0开始 
	s.Format("From:位置：%d",where);//数字  得到位置
	if (where!=-1)
	{
		ret+=ReadLn(where,rbmsgs[i].text);//从src串中，从index所指的位置开始，取子字串，直到行末符"\r"为止。
		ret+="\n"; //加上换行
		//写入ret 最后返回的东西
	}
	where=rbmsgs[i].text.Find("To:");
	s.Format("To:位置：%d",where);
	if (where!=-1)
	{
		ret+=ReadLn(where,rbmsgs[i].text);
		ret+="\n"; //加上换行
	}
		where=rbmsgs[i].text.Find("Date:");
        s.Format("Date:位置：%d",where);
	if (where!=-1)
	{
		ret+=ReadLn(where,rbmsgs[i].text);	
		ret+="\n";
	}
	return ret;
}


//获得第 i 封信件的标题信息
CString pop3Sock::GetMsgStuff(int i)
{
	CString ret;
    CString s;
	int where=msgs[i].text.Find("From:");
    s.Format("From:位置：%d\r\n",where);//数字  得到位置
	
	if (where!=-1)
	{
		ret+=ReadLn(where,msgs[i].text);//从src串中，从index所指的位置开始，取子字串，直到行末符"\r"为止。
		ret+="\r\n";
		//写入ret 最后返回的东西
	}

	where=msgs[i].text.Find("To:");

s.Format("To:位置：%d\r\n",where);	
	if (where!=-1)
	{
		ret+=ReadLn(where,msgs[i].text);
		ret+="\r\n";
	}
	
	where=msgs[i].text.Find("Date:");

s.Format("Date:位置：%d\r\n",where);
	
	if (where!=-1)
	{
		ret+=ReadLn(where,msgs[i].text);	
		ret+="\r\n";
	}
	ret+=GetMsgSubject(i);   //加上标题函数
	ret+="\r\n"; //加上换行
	return ret;


}

//获得第 i 封信件的信件体
CString pop3Sock::GetMsgBody(int i)
{
/*a = "123456789";
b =a.Left(4);  //值为:1234
b = a.Right(4);  //值为:6789*/
	CString ret;
	int where=msgs[i].text.Find("\r\n\r\n");  //找到开始的结束符的位置 是一个数字  就是信件内容的开头
	if(where!=-1) where+=7;     //如果找到就+4  加到整个text都覆盖
	else          where=0;      //没有就等于0  什么都没！
	ret=msgs[i].text.Right(msgs[i].text.GetLength()-(where-2));      //text的总长度减去信件开头的位置，就是得到\r\n\r\n后的信件内容。 节约空间
  //如果where=0 就是得到整个text  即没有\r\n\r\n，也实现了where！=-1时要去掉\r\n\r\n的情况(现在没有了，就不用去除)
	ret=ret.Left(ret.GetLength()-3);  //减去2个换行符和一个回车键。 节约空间
	//where!=0的时候，
	return ret;
}
//获得第 i 封垃圾信件的信件体
CString pop3Sock::GetRbMsgBody(int i)
{
/*a = "123456789";
b =a.Left(4);  //值为:1234
b = a.Right(4);  //值为:6789*/
	CString ret;
	int where=rbmsgs[i].text.Find("\r\n\r\n");  //找到开始的结束符的位置 是一个数字  就是信件内容的开头
	if(where!=-1) where+=7;     //如果找到就+  加到整个text都覆盖
	else          where=0;      //没有就等于0  什么都没！
	ret=rbmsgs[i].text.Right(rbmsgs[i].text.GetLength()-(where-2));      //text的总长度减去信件开头的位置，就是得到\r\n\r\n后的信件内容。 节约空间
  //如果where=0 就是得到整个text  即没有\r\n\r\n，也实现了where！=-1时要去掉\r\n\r\n的情况(现在没有了，就不用去除)
	ret=ret.Left(ret.GetLength()-3);  //减去2个换行符和一个回车键。 节约空间
	//where!=0的时候，
	return ret;
}


//获得第 i 封信件的标题
CString pop3Sock::GetMsgSubject(int i)
{
//	CString s;
//	s.Format("%s",msgs[i].text);
//	int where=s.Find("Subject:");
	
	CString ret;
	int where=msgs[i].text.Find("Subject:");
	if (where!=-1) 	ret=ReadLn(where,msgs[i].text);
	else ret.Format("垃圾邮箱，已拦截");
	return ret;
}
//获得第 i 封垃圾信件的标题
CString pop3Sock::GetRbMsgSubject(int i)
{
//	CString s;
//	s.Format("%s",msgs[i].text);
//	int where=s.Find("Subject:");
	
	CString ret;
	int where=rbmsgs[i].text.Find("Subject:");
	if (where!=-1) 	ret=ReadLn(where,rbmsgs[i].text);
	else ret.Format("没有找到");
	return ret;
}

//获得第 i 封信整封信的内容
CString pop3Sock::GetMsg(int i)
{
	return msgs[i].text;
}

//从src串中，从index所指的位置开始，取子字串，直到行末符"\r"为止。
//dst是一个串的地址，取出的行追加在该串的原内容之后
CString pop3Sock::ReadLn(int index,CString src)
{
//	CString comp;
//	CString dst;
//	comp=src[index];
//	while(comp!="\r")
//	{
//		dst+=comp;
//		comp=src[++index];
//	}
//	return dst;

	CString ret;
	CString s;
	
	ret=src.Right(src.GetLength()-index);
	int where=ret.Find("\r");
//	s.Format("行所在的位置：%d",where);
//	m_pDlg->m_Info+=s;
//	m_pDlg->UpdateData(FALSE);
	if(where!=-1) where+=1;  //加上换行符
	else          where=0;
//	s.Format("行所在的位置：%d, Length=%d ",where,ret.GetLength());
//	m_pDlg->m_Info+=s;
//	m_pDlg->UpdateData(FALSE);
	
	ret=ret.Left(where);
	return ret;

}

 
 //退出服务器
void pop3Sock::Close()
{
	CString str;
	str.Format("quit%c%c",13,10);
	Send((LPCSTR)str,str.GetLength());
	state=pop3FIRST;
	CAsyncSocket::Close();
	error="执行pop3Sock::Close()命令，\r\nNot connected to server\r\n";
	m_pDlg->Disp(S_CLOSE);
}

void pop3Sock::AnalyzeMsg()
{
	int  ix;
	CString temp1;
	CString temp2;

	CString s;
	strstream str;
	string check;
	str<<(LPCSTR)lastMsg; //将收到的信息拷贝到字符流变量中  
	str>>check;           //提取所收到信息的第一段代码
	if(check=="-ERR")     //如果有错误
	{
		error="Received -ERR from server :"+lastMsg;
		Close(); //断开连接，然后关闭
		return;
	}
	
	//如果没有错误，则根据不同的会话阶段响应来处理
	switch(state) 
	{
	case pop3FIRST: //如果已经连接成功，类初始化的时候state为FIRST
		msgs.clear();
		//显示连接成功后服务器返回的消息
		m_pDlg->Disp(S_RECEIVE); 
		//发送 user 用户名 命令
		s.Format("user %s%c%c",m_pDlg->m_strUser,13,10);
		Send((LPCSTR)s,s.GetLength()); //发送用户帐号
		state=pop3USER;
		break;

	case pop3USER:
		//显示服务器对于user命令的响应
		m_pDlg->Disp(S_RECEIVE);
		//发送 pass 口令 命令，发送密码
		s.Format("pass %s%c%c",m_pDlg->m_strPass,13,10); 
		Send((LPCSTR)s,s.GetLength()); 
		state=pop3PASS;
		break;

	case pop3PASS:
		//显示服务器对 pass 命令的响应
		m_pDlg->Disp(S_RECEIVE);
		//发送 stat 命令，请求服务器返回信箱中
		//邮件的数量和所占空间的大小
		s.Format("stat%c%c",13,10);
		Send((LPCSTR)s,s.GetLength());
		state=pop3STAT; 
		break;

	case pop3STAT:
	{
		string s1;
		str.seekg(0);  //将字符流的当前指针置为0
		str>>s1>>numMsg>>sizeMsg; //获得数量和大小
		flush(str);
		m_pDlg->Disp(S_GETNUM);
		m_pDlg->Disp(S_GETSIZE);
		if(numMsg>0) //如果有邮件，则发送RETR获得邮件信息
		{
			rubbish_number=1;
			retrMsg=1;        //要取第一封信
			MESSAGEPROP prop; //准备存放信件的地方
			prop.msgSize=0;   //成员置初值
			prop.retrSize=0;
			prop.text="";
			msgs.push_back(prop); //压入向量表
			s.Format("retr 1%c%c",13,10);
			Send((LPCSTR)s,s.GetLength()); 
			state=pop3RETR;
		}
		else //如果没有邮件，则断开
		{
			error="No new messages\r\n";
			Close();
		}
	}
	break;

	case pop3RETR:
	{
		//显示取回来的信件文本

//s.Format("numMsg= %d  retrMsg =%d",numMsg,retrMsg);	
//m_pDlg->m_Info+=s;
		if(isrubbish(lastMsg)) {  //是就直接不接受  不执行操作
				MESSAGEPROP prop;
				prop.msgSize=0;
				prop.retrSize=0;
				prop.text="";
				rbmsgs.push_back(prop);
			rbmsgs[rubbish_number-1].text+=lastMsg; 
		    rbmsgs[rubbish_number-1].retrSize+=lastMsg.GetLength();
							
	//	s.Format("信件号%d: ,信件大小%d, %c%c",
//		 rubbish_number,lastMsg.GetLength(),13,10);
	//	s.Format("垃圾信件内容删输出：%d,%s",rbmsgs[rubbish_number-1].retrSize,rbmsgs[rubbish_number-1].text);
m_pDlg->m_Info+=s;
m_pDlg->UpdateData(FALSE);		
m_pDlg->Disp(S_RECEIVE);
rubbish_number++;
			//s.Format("一共找到%d封垃圾邮件！\n",rubbish_number);
			//m_pDlg->m_Info+=s;
			//state = POP3FILTER;
		}
	  else {
/*s.Format("信件号%d: ,信件大小%d, %c%c",
		 retrMsg-1,lastMsg.GetLength(),13,10);
m_pDlg->m_Info+=s;*/
m_pDlg->UpdateData(FALSE);		
m_pDlg->Disp(S_RECEIVE);
		msgs[retrMsg-1].text+=lastMsg; 
		msgs[retrMsg-1].retrSize+=lastMsg.GetLength();
		}
		ix=lastMsg.Find("\r\n.\r\n");
		if (ix != -1)	//找到了这封信的结尾  这里只是循环找信  并不是输出信件内容
		{	
			//检查是否有其他邮件
			if(retrMsg<numMsg) //如果还有，则继续发送retr命令
			{
				MESSAGEPROP prop;
				prop.msgSize=0;
				prop.retrSize=0;
				prop.text="";
				msgs.push_back(prop);
				retrMsg++;
				//发retr命令取下一封信
				s.Format("retr %d%c%c",retrMsg,13,10);
				Send((LPCSTR)s,s.GetLength());
			}  else	{
				//如果全部信件接收完毕，判断是否要删除
				
				if(m_pDlg->m_bolDel && numMsg>0) 
				{
					state=pop3DELE;
					delMsg=1;
					s.Format("dele %d%c%c",delMsg,13,10);
					Send((LPCSTR)s,s.GetLength());
				}  else {                      //否则退出
					state=pop3ENDRETR;
					m_pDlg->Disp(S_ENDRETR);
					error="信件接收完,不删邮件，会话结束\r\n";
					s.Format("quit%c%c",13,10);
					Send((LPCSTR)s,s.GetLength());
					Close();

					for(int i=0;i<retrMsg;i++)
					{
						s.Format("%d:  %s",i,GetMsgSubject(i));
			//m_pDlg->m_Info+=s;
					
						m_pDlg->m_ctrList.AddString(s);
				//		m_pDlg->m_ctrList.AddString((LPCSTR)GetMsgSubject(i));
					}
			//m_pDlg->UpdateData(FALSE);
					m_pDlg->m_ctrList.SetCurSel(0);
				}
			}
		}  //DEBUG 
		//}
	}
	break;

	case pop3DELE:
	{
		//删除剩余邮件
		if(delMsg<numMsg)
		{
			delMsg++;
			s.Format("dele %d%c%c",delMsg,13,10);
			Send((LPCSTR)s,s.GetLength());
		}
		else //如果已经删除完毕
		{
			m_pDlg->Disp(S_ENDRETR);
			state=pop3GOON;
			error="Deleted all messages\r\n";
			s.Format("quit%c%c",13,10);
			Send((LPCSTR)s,s.GetLength());
			Close();
		}
	}
	break;
	
	case pop3GOON:
	
	default:
		m_pDlg->Disp(S_RECEIVE);
		break;
	}
}
//RC4解密
CString pop3Sock::RC4_decrypt(CString letter)     
{
     CString decrypt;
	 string str = CA2A(letter.GetString());
    unsigned char Tbox[256] = {0x00}; //全部初始化为0
	unsigned char Sbox[256] = {0x00};//全部初始化为0
	char *k="wjl";//使用相同的解密密钥wjl
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
	//加密
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
		   char k1=Sbox[t];    //与加密操作同样的密钥
		   char cipherchar = ch^k1; //密文再异或一次便得到明文 
		   str.insert(q,1,cipherchar);
		   str.erase(q + 1, 1);
	  }
	  decrypt += CA2A(str.c_str());
	  return decrypt;

}
//判断是否是垃圾邮件
bool  pop3Sock::isrubbish(CString lastMsg)
{
	int i,j;
	CString ret = "";
	CStringArray BlackList;
	CString str[][1] = {{_T("下注")},{_T("发票")},{_T("六合彩")},{_T("广告")},{_T("AD")},{_T("Free")}};
	for(i=0;i<sizeof(str)/sizeof(str[0]);i++){
		for(j=0;j<sizeof(str[0])/sizeof(str[0][0]);j++){
			BlackList.Add(str[i][j]);	}      //添加关键字
	}

	int where=lastMsg.Find("From:");   //返回找到匹配内容的索引值
	if (where!=-1)
	{   
		where+=4;                            //索引前移4
	    ret=lastMsg.Right(lastMsg.GetLength()-where);   //从后往前获取内容
	    ret=ret.Left(ret.GetLength()-3);              //从前往后获取内容
		for(int i = 0; i < BlackList.GetCount(); i++)
		{
			if(ret.Find(BlackList[i]) != -1)     //如果匹配到关键字
			{
				return true;            //返回TRUE值，邮件被判断为垃圾邮件
			}
		}
	}
	return false;
}
