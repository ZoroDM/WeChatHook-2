#include "ChatRecord.h"
#include <Windows.h>
#include <string>
#include "Util.h"
#include "offset.h"
#include "struct.h"
#include "WSClient.h"
#include "EVString.h"
#include "Command.h"
#include "Loger.h"
#include <Shlwapi.h>

DWORD recieveMsgCall, recieveMsgJmpAddr;
BOOL g_AutoChat = FALSE;	//�Ƿ��Զ�����
BOOL isSendTuLing = FALSE;	//�Ƿ��Ѿ�������ͼ�������
BOOL isText = TRUE;			//�Ƿ���������Ϣ
DWORD r_esp = 0;
DWORD r_eax = 0;
std::wstring GetMsgByAddress(DWORD memAddress);
string Wstr2Str(wstring wstr)
{
	if (wstr.length() == 0)
		return "";

	std::string str;
	str.assign(wstr.begin(), wstr.end());
	return str;
}

void ReceiveMsgProc(LPVOID Context)
{
	recieveMsgStruct* msg = (recieveMsgStruct*)Context;

	WriteInfo("�յ���Ϣ1");
	//todo:�����Զ�����(�Զ��տ�Զ�����Ƭ��)
	neb::CJsonObject data;
	//todo:fromWxid��senderWxidĳЩ������Ϣ���쳣

	WriteInfo("�յ���Ϣ2");
	data.Add("Type", EVString::w2a(msg->type));
	data.Add("Source", EVString::w2a(msg->source));
	data.Add("Wxid", EVString::w2a(msg->wxid));
	data.Add("MsgSender", EVString::w2a(msg->msgSender));
	data.Add("Content", EVString::w2a(msg->content));
	data.Add("IsMoney",msg->isMoney);
	delete msg;
	WriteInfo("�յ���Ϣ3");
	Send(Cmd_ReceiveMessage, data);
}

/**
 * �������ص�����Ϣ����
 */
void RecieveMessageJump()
{
	recieveMsgStruct *msg = new recieveMsgStruct;
	msg->isMoney = FALSE;
	//��Ϣ���λ��
	DWORD** msgAddress = (DWORD**)r_esp;
	//��Ϣ����
	DWORD msgType = *((DWORD*)(**msgAddress + 0x30));

	BOOL isFriendMsg = FALSE;		//�Ƿ��Ǻ�����Ϣ
	BOOL isImageMessage = FALSE;	//�Ƿ���ͼƬ��Ϣ
	BOOL isRadioMessage = FALSE;	//�Ƿ�����Ƶ��Ϣ
	BOOL isVoiceMessage = FALSE;	//�Ƿ���������Ϣ
	BOOL isBusinessCardMessage = FALSE;	//�Ƿ�����Ƭ��Ϣ
	BOOL isExpressionMessage = FALSE;	//�Ƿ�����Ƭ��Ϣ
	BOOL isLocationMessage = FALSE;	//�Ƿ���λ����Ϣ
	BOOL isSystemMessage = FALSE;	//�Ƿ���ϵͳ������Ϣ
	BOOL isFriendRequestMessage = FALSE;	//�Ƿ��Ǻ���������Ϣ
	BOOL isOther = FALSE;	//�Ƿ���������Ϣ


	switch (msgType)
	{
	case 0x01:
		memcpy(msg->type, L"����", sizeof(L"����"));
		break;
	case 0x03:
		memcpy(msg->type, L"ͼƬ", sizeof(L"ͼƬ"));
		isImageMessage = TRUE;
		break;
	case 0x22:
		memcpy(msg->type, L"����", sizeof(L"����"));
		isVoiceMessage = TRUE;
		break;
	case 0x25:
		memcpy(msg->type, L"����ȷ��", sizeof(L"����ȷ��"));
		isFriendRequestMessage = TRUE;
		break;
	case 0x28:
		memcpy(msg->type, L"POSSIBLEFRIEND_MSG", sizeof(L"POSSIBLEFRIEND_MSG"));
		isOther = TRUE;
		break;
	case 0x2A:
		memcpy(msg->type, L"��Ƭ", sizeof(L"��Ƭ"));
		isBusinessCardMessage = TRUE;
		break;
	case 0x2B:
		memcpy(msg->type, L"��Ƶ", sizeof(L"��Ƶ"));
		isRadioMessage = TRUE;
		break;
	case 0x2F:
		//ʯͷ������
		memcpy(msg->type, L"����", sizeof(L"����"));
		isExpressionMessage = TRUE;
		break;
	case 0x30:
		memcpy(msg->type, L"λ��", sizeof(L"λ��"));
		isLocationMessage = TRUE;
		break;
	case 0x31:
		//����ʵʱλ��
		//�ļ�
		//ת��
		//����
		//�տ�
		memcpy(msg->type, L"����ʵʱλ�á��ļ���ת�ˡ�����", sizeof(L"����ʵʱλ�á��ļ���ת�ˡ�����"));
		isOther = TRUE;
		break;
	case 0x32:
		memcpy(msg->type, L"VOIPMSG", sizeof(L"VOIPMSG"));
		isOther = TRUE;
		break;
	case 0x33:
		memcpy(msg->type, L"΢�ų�ʼ��", sizeof(L"΢�ų�ʼ��"));
		isOther = TRUE;
		break;
	case 0x34:
		memcpy(msg->type, L"VOIPNOTIFY", sizeof(L"VOIPNOTIFY"));
		isOther = TRUE;
		break;
	case 0x35:
		memcpy(msg->type, L"VOIPINVITE", sizeof(L"VOIPINVITE"));
		isOther = TRUE;
		break;
	case 0x3E:
		memcpy(msg->type, L"С��Ƶ", sizeof(L"С��Ƶ"));
		isRadioMessage = TRUE;
		break;
	case 0x270F:
		memcpy(msg->type, L"SYSNOTICE", sizeof(L"SYSNOTICE"));
		isOther = TRUE;
		break;
	case 0x2710:
		//ϵͳ��Ϣ
		//���
		memcpy(msg->type, L"�����ϵͳ��Ϣ", sizeof(L"�����ϵͳ��Ϣ"));
		isSystemMessage = TRUE;
		break;
	default:
		isOther = TRUE;
		break;
	}
	//��Ϣ����
	wstring fullmessgaedata = GetMsgByAddress(**msgAddress + 0x68);	//��������Ϣ����
	//�ж���Ϣ��Դ��Ⱥ��Ϣ���Ǻ�����Ϣ
	wstring msgSource2 = L"<msgsource />\n";
	wstring msgSource = L"";
	msgSource.append(GetMsgByAddress(**msgAddress + 0x198));
	//������Ϣ
	if (msgSource.length() <= msgSource2.length())
	{
		memcpy(msg->source, L"������Ϣ", sizeof(L"������Ϣ"));
		isFriendMsg = TRUE;
	}
	else
	{
		//Ⱥ��Ϣ
		memcpy(msg->source, L"Ⱥ��Ϣ", sizeof(L"Ⱥ��Ϣ"));
	}
	//������Ϣ
	if ((int)*((DWORD*)(**msgAddress + 0x34))) {
		//memcpy(msg->type, L"������Ϣ", sizeof(L"������Ϣ"));
		memcpy(msg->source, L"�Լ�", sizeof(L"�Լ�"));
	}
	//��ʾ΢��ID/ȺID
	LPVOID pWxid = *((LPVOID*)(**msgAddress + 0x40));
	swprintf_s(msg->wxid, L"%s", (wchar_t*)pWxid);

	//�����Ⱥ��Ϣ
	if (isFriendMsg == FALSE)
	{
		//��ʾ��Ϣ������
		LPVOID pSender = *((LPVOID*)(**msgAddress + 0x144));
		swprintf_s(msg->msgSender, L"%s", (wchar_t*)pSender);
	}
	else
	{
		memcpy(msg->msgSender, L"NULL", sizeof(L"NULL"));
	}


	//��ʾ��Ϣ����  �����޷���ʾ����Ϣ ��ֹ����
	if (StrStrW(msg->wxid, L"gh"))
	{
		//�����ͼ������˷�������Ϣ ������Ϣ�Ѿ����͸�ͼ�������
		if ((StrCmpW(msg->wxid, L"gh_ab370b2e4b62") == 0) && isSendTuLing == TRUE)
		{
			wchar_t tempcontent[0x200] = { 0 };
			//�����жϻ����˻ظ�����Ϣ���� ����������� ֱ�ӻظ�
			if (msgType != 0x01)
			{
				//SendTextMessage(tempwxid, (wchar_t*)L"������");
				isSendTuLing = FALSE;
			}
			//�ٴ��жϷ��͸������˵���Ϣ����
			else if (isText == FALSE)
			{
				//SendTextMessage(tempwxid, (wchar_t*)L"�� ��֧�ִ�����ϢŶ �뷢���� ôô��");
				isSendTuLing = FALSE;
				isText = TRUE;
			}
			else   //��������� �ٴ��жϳ���
			{
				//�����õ���Ϣ���� ���͸�����
				LPVOID pContent = *((LPVOID*)(**msgAddress + 0x68));
				swprintf_s(tempcontent, L"%s", (wchar_t*)pContent);
				//�жϷ��ص���Ϣ�Ƿ����
				if (wcslen(tempcontent) > 0x100)
				{
					wcscpy_s(tempcontent, wcslen(L"������"), L"������");
				}

				//SendTextMessage(tempwxid, tempcontent);
				isSendTuLing = FALSE;
			}
		}
		//���΢��IDΪgh_3dfda90e39d6 ˵�����տ���Ϣ
		else if ((StrCmpW(msg->wxid, L"gh_3dfda90e39d6") == 0))
		{
			swprintf_s(msg->content, L"%s", L"΢���տ��");
			msg->isMoney = TRUE;
		}
		else
		{
			//���΢��ID�д���gh ˵���ǹ��ں�
			swprintf_s(msg->content, L"%s", L"���ںŷ�������,�����ֻ��ϲ鿴");
		}
	}
	//����ͼƬ��Ϣ 
	else if (isImageMessage == TRUE)
	{
		swprintf_s(msg->content, L"%s", L"�յ�ͼƬ��Ϣ,�����ֻ��ϲ鿴");
	}
	else if (isRadioMessage == TRUE)
	{
		swprintf_s(msg->content, L"%s", L"�յ���Ƶ��Ϣ,�����ֻ��ϲ鿴");
	}
	else if (isVoiceMessage == TRUE)
	{
		swprintf_s(msg->content, L"%s", L"�յ�������Ϣ,�����ֻ��ϲ鿴");
	}
	else if (isBusinessCardMessage == TRUE)
	{
		swprintf_s(msg->content, L"%s", L"�յ���Ƭ��Ϣ,���Զ���Ӻ���");
		//�Զ���Ӻ���
		//AutoAddCardUser(fullmessgaedata);
	}
	else if (isExpressionMessage == TRUE)
	{
		swprintf_s(msg->content, L"%s", L"�յ�������Ϣ,�����ֻ��ϲ鿴");
	}
	//�Զ�ͨ����������
	else if (isFriendRequestMessage == TRUE)
	{
		swprintf_s(msg->content, L"%s", L"�յ���������,���Զ�ͨ��");
		//�Զ�ͨ����������
		//AutoAgreeUserRequest(fullmessgaedata);
	}
	else if (isOther == TRUE)
	{
		//ȡ����Ϣ����
		wchar_t tempcontent[0x10000] = { 0 };
		LPVOID pContent = *((LPVOID*)(**msgAddress + 0x68));
		swprintf_s(tempcontent, L"%s", (wchar_t*)pContent);
		//�ж��Ƿ���ת����Ϣ
		if (StrStrW(tempcontent, L"΢��ת��"))
		{
			swprintf_s(msg->content, L"%s", L"�յ�ת����Ϣ,���Զ��տ�");

			//�Զ��տ�
			//AutoCllectMoney(fullmessgaedata, msg->wxid);	
		}
		else
		{
			//�ж���Ϣ���� ������ȳ����Ͳ���ʾ
			if (wcslen(tempcontent) > 200)
			{
				swprintf_s(msg->content, L"%s", L"��Ϣ���ݹ��� �Ѿ�����");
			}
			else
			{
				//�ж��Ƿ���ת����Ϣ
				swprintf_s(msg->content, L"%s", L"�յ�����ʵʱλ�á��ļ������ӵ�������Ϣ,�����ֻ��ϲ鿴");
			}

		}
	}
	else if (isLocationMessage == TRUE)
	{
		swprintf_s(msg->content, L"%s", L"�յ�λ����Ϣ,�����ֻ��ϲ鿴");
	}
	else if (isSystemMessage == TRUE)
	{
		wchar_t tempbuff[0x1000];
		LPVOID pContent = *((LPVOID*)(**msgAddress + 0x68));
		swprintf_s(tempbuff, L"%s", (wchar_t*)pContent);

		//�����ﴦ�����Ⱥ����Ϣ
		if ((StrStrW(tempbuff, L"�Ƴ���Ⱥ��") || StrStrW(tempbuff, L"������Ⱥ��")))
		{
			wcscpy_s(msg->content, wcslen(tempbuff) + 1, tempbuff);
		}
		else
		{
			swprintf_s(msg->content, L"%s", L"�յ������ϵͳ��Ϣ,�����ֻ��ϲ鿴");
		}

	}
	//������������Ϣ֮��
	else
	{
		wchar_t tempbuff[0x1000];
		LPVOID pContent = *((LPVOID*)(**msgAddress + 0x68));
		swprintf_s(tempbuff, L"%s", (wchar_t*)pContent);
		//�ж���Ϣ���� ������ȳ����Ͳ���ʾ
		if (wcslen(tempbuff) > 200)
		{
			swprintf_s(msg->content, L"%s", L"��Ϣ���ݹ��� �Ѿ�����");
		}
		else
		{
			swprintf_s(msg->content, L"%s", (wchar_t*)pContent);
		}

	}
	//MessageBox(NULL, msg->content, L"����", MB_OK);
	//wchar_t msgOut[0x500];
	//swprintf_s(msgOut, L"%s,%s,%s,%s,%s,%d\r\n", msg->type, msg->source, msg->wxid, msg->msgSender, msg->content, msg->isMoney);
	HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReceiveMsgProc, msg, 0, NULL);
}


/**
 * ��д�뵽hook��Ľ�����Ϣ�㺯��
 */
__declspec(naked) void RecieveMsgDeclspec()
{
	//�����ֳ�
	__asm
	{
		//���䱻���ǵĴ���
		mov ecx, recieveMsgCall

		//��ȡesp�Ĵ������ݣ�����һ��������
		mov r_esp, esp
		mov r_eax, eax

		pushad
		pushfd
	}
	RecieveMessageJump();
	//�ָ��ֳ�
	__asm
	{
		popfd
		popad
		//���ر�HOOKָ�����һ��ָ��
		jmp recieveMsgJmpAddr
	}
}

void HookChatRecord() {
	//HOOK������Ϣ
	DWORD recieveMsgHookAddr = GetWeChatWinBase() + RECIEVEHOOKADDR-5;
	recieveMsgCall = GetWeChatWinBase() + RECIEVEHOOKCALL;
	recieveMsgJmpAddr = recieveMsgHookAddr + 5;
	BYTE msgJmpCode[5] = { 0xE9 };
	*(DWORD*)&msgJmpCode[1] = (DWORD)RecieveMsgDeclspec - recieveMsgHookAddr - 5;
	WriteProcessMemory(GetCurrentProcess(), (LPVOID)recieveMsgHookAddr, msgJmpCode, 5, NULL);
}
//************************************************************
// ��������: GetMsgByAddress
// ����˵��: �ӵ�ַ�л�ȡ��Ϣ����
// ��    ��: GuiShou
// ʱ    ��: 2019/7/6
// ��    ��: DWORD memAddress  Ŀ���ַ
// �� �� ֵ: LPCWSTR	��Ϣ����
//************************************************************
std::wstring GetMsgByAddress(DWORD memAddress)
{
	wstring tmp;
	DWORD msgLength = *(DWORD*)(memAddress + 4);
	if (msgLength > 0) {
		WCHAR* msg = new WCHAR[msgLength + 1];
		wmemcpy_s(msg, msgLength + 1, (WCHAR*)(*(DWORD*)memAddress), msgLength + 1);
		tmp = msg;
		delete[]msg;
	}
	return  tmp;
}