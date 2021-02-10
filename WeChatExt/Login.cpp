#include <Windows.h>
#include "offset.h"
#include "Util.h"

/**
 * ���΢�ŵ�¼״̬
 * @param
 * @return int
 */
bool CheckLogin()
{
	int* islogin=(int*)(GetWeChatWinBase() + ISLOGIN);
	return *islogin == 1;
}


/**
 * �˳�΢��
 * @param
 * @return void
 */
void LogoutWeChat()
{
	DWORD dwCallAddress = GetWeChatWinBase() + LOGOUT;
	HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)dwCallAddress, 0, NULL, 0);
	if (hThread != 0)
	{
		CloseHandle(hThread);
	}
}


/**
 * ��ת����ά��
 * @param
 * @return void
 */
VOID GotoQrCode()
{
	DWORD dwCallAddr1 = GetWeChatWinBase() + GOTOQRCODE1;
	DWORD dwCallAddr2 = GetWeChatWinBase() + GOTOQRCODE2;

	__asm {
		call dwCallAddr1;
		mov ecx, eax;
		call dwCallAddr2;
	}

}
