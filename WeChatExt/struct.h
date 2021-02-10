#pragma once
#include <minwindef.h>
#include <stdafx.h>

//�û�������Ϣ
struct Information
{
	wchar_t wxid[0x200] = L"NULL";	//΢��ID
	wchar_t account[0x200] = L"NULL";	//�Զ����˺�
	wchar_t nickname[0x500] = L"NULL";	//�ǳ�
	wchar_t device[0x100] = L"NULL";	//��½�豸
	wchar_t phone[0x100] = L"NULL";	//���ֻ���
	wchar_t email[0x200] = L"NULL";	//������
	int		sex = 0;		//�Ա�
	wchar_t nation[0x100] = L"NULL";	//����
	wchar_t province[0x100] = L"NULL";	//ʡ��
	wchar_t city[0x100] = L"NULL";	//����
	wchar_t signName[0x500] = L"NULL";	//ǩ��
	wchar_t bigHeader[0x1000] = L"NULL";	//��ͷ��
};

//�û���Ϣ
struct recieveMsgStruct
{
	wchar_t type[20];		//��Ϣ����
	wchar_t source[20];		//��Ϣ��Դ
	wchar_t wxid[40];		//΢��ID/ȺID
	wchar_t msgSender[40];	//��Ϣ������
	wchar_t content[200];	//��Ϣ����
	BOOL isMoney;	//�Ƿ����տ���Ϣ
};

//�����б�
struct friendStruct
{
	wchar_t wxid[0x100] = L"NULL";
	wchar_t account[0x200] = L"NULL";
	wchar_t nickname[0x200] = L"NULL";
	wchar_t head[0x1000] = L"NULL";
	wchar_t v1[0x200] = L"NULL";
	wchar_t reMark[0x200] = L"NULL";
	wchar_t bigHeadImgUrl[0x1000] = L"NULL";
};

//���ݿ���
struct DbHandle
{
	int handler;
	char path[0x1000];
};

typedef int (__cdecl*sqlite3_callback)(void*, int, char**, char**);

typedef int(__cdecl* Sqlite3_exec)(
	DWORD,
	const char*,
	sqlite3_callback,
	void*,
	char**
	);