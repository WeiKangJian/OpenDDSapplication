#ifndef _H_GETSYSINFO  
#define _H_GETSYSINFO  


#pragma once  


#include <afxcoll.h>
#include <afxtempl.h>
#include <atltypes.h>
#include <string>

struct interfaces {
	char name[10][100];
	int band[10];
};

class GetSysInfo
{
public:
	GetSysInfo(void);
	~GetSysInfo(void);

public:
				   /***********获取网卡数目和名字***********/
	//int  GetInterFaceCount(struct interfaces *strs);
	int  GetInterFaceCount(long *band);
	void GetInterFaceName(CString &InterfaceName, int pNum);
	
private:
	CStringList Interfaces;                       //保存所有网卡的名字  
	CList < DWORD, DWORD &>       Bandwidths;   //各网卡的带宽  
	CList < DWORD, DWORD &>       TotalTraffics;    //各网卡的总流量  
};

#endif