#include <stdexcept> // std::domain_error()
#include <iomanip>

#include <windows.h>
#include <wtsapi32.h>

#include "igs_resource_msg_from_err.h"

#if defined DEBUG_IGS_RESOURCE_IS_LOGIN
# include <iostream> // std::cout
# if defined UNICODE
#  if !defined cout // define guard
#   define cout wcout /* "cout"ʸ���������"wcout"���Ѵ����������� */
#  endif
# endif
namespace {
 /******LPCTSTR info_from_session_state_(WTS_CONNECTSTATE_CLASS state) {
	switch (state) {
	case WTSActive:
		return	TEXT("A user is logged on to the WinStation.");
	case WTSConnected:
		return	TEXT("The WinStation is connected to the client.");
	case WTSConnectQuery:
		return	TEXT("The WinStation is in the process of")
			TEXT(" connecting to the client.");
	case WTSShadow:
		return	TEXT("The WinStation is shadowing another")
			TEXT(" WinStation.");
	case WTSDisconnected:
		return	TEXT("The WinStation is active but")
			TEXT(" the client is disconnected.");
	case WTSIdle:
		return	TEXT("The WinStation is waiting for ")
			TEXT("a client to connect.");
	case WTSListen:
		return	TEXT("The WinStation is listening for ")
			TEXT("a connection.")
			TEXT(" A listener session waits for")
			TEXT(" requests for new client connections.")
			TEXT(" No user is logged on a listener session.")
			TEXT(" A listener session cannot be reset,")
			TEXT(" shadowed, or changed to")
			TEXT(" a regular client session.");
	case WTSReset:
		return	TEXT("The WinStation is being reset.");
	case WTSDown:
		return	TEXT("The WinStation is down due to an error.");
	case WTSInit:
		return	TEXT("The WinStation is initializing.");
	}
	return 0;
 }******/
 LPCTSTR info_from_session_state_(WTS_CONNECTSTATE_CLASS state) {
	switch (state) {
	case WTSActive:		return	TEXT("WTSActive");
	case WTSConnected:	return	TEXT("WTSConnected");
	case WTSConnectQuery:	return	TEXT("WTSConnectQuery");
	case WTSShadow:		return	TEXT("WTSShadow");
	case WTSDisconnected:	return	TEXT("WTSDisconnected");
	case WTSIdle:		return	TEXT("WTSIdle");
	case WTSListen:		return	TEXT("WTSListen");
	case WTSReset:		return	TEXT("WTSReset");
	case WTSDown:		return	TEXT("WTSDown");
	case WTSInit:		return	TEXT("WTSInit");
	}
	return 0;
 }
 void wts_user_name_(
	HANDLE hServer
	,DWORD SessionId
	,std::basic_string<TCHAR>&str
 ) {
	LPTSTR lpBuffer = 0;
	DWORD dwBytesReturned = 0;
	BOOL bReturn = ::WTSQuerySessionInformation(
		hServer
		, SessionId
		, WTSUserName
		, &lpBuffer
		, &dwBytesReturned
	);
	str = lpBuffer;
	::WTSFreeMemory(lpBuffer);
	if (!bReturn) {
		throw std::domain_error(igs_resource_msg_from_err(
		 TEXT("WTSQuerySessionInformation(-)"),::GetLastError()
		));
	}
 }
 void wts_session_print_(
	HANDLE hWTServer
	,PWTS_SESSION_INFO pSessionInfo
 ) {
	DWORD sid = pSessionInfo->SessionId;
	LPCTSTR station_name = pSessionInfo->pWinStationName;
	std::basic_string<TCHAR> user_name;
	wts_user_name_( hWTServer,pSessionInfo->SessionId,user_name );
	LPCTSTR info = info_from_session_state_(pSessionInfo->State);

	std::cout
		<< std::left
		<< TEXT("StationName=") << std::setw(9) << station_name
		<< TEXT(" UserName=")      << std::setw(7) << user_name
		<< TEXT(" SessionId=")     << std::setw(5) << sid
		//<< std::endl << TEXT(" State=") << info << std::endl
		<< TEXT(" State=") << info << std::endl
		;
 }
}
#endif  /* !DEBUG_IGS_RESOURCE_IS_LOGIN */

#include "igs_resource_is_login.h"

bool igs::resource::is_login(void) {
	HANDLE hWTServer = ::WTSOpenServer(NULL);
	if (NULL == hWTServer) {
		throw std::domain_error(igs_resource_msg_from_err(
			TEXT("WTSOpenServer(-)"),::GetLastError()
		));
	}

	PWTS_SESSION_INFO pSessionInfo;
	DWORD pCount = 0;
	BOOL bReturn = ::WTSEnumerateSessions(
		hWTServer,0,1,&pSessionInfo,&pCount
	);
	if (!bReturn) {
		const DWORD dw = ::GetLastError();
		::WTSFreeMemory(pSessionInfo); // Return Nothing
		::WTSCloseServer(hWTServer);   // Return Nothing
		throw std::domain_error(igs_resource_msg_from_err(
			TEXT("WTSEnumerateSessions(-)"),dw
		));
	}

	/* Windows�Υ����ߥʥ륵���ӥ��ˤĤ��ƥ�� 2012-10-17
		Windows�Υ����ߥʥ륵���ӥ��ϥ�⡼�ȥǥ����ȥå���³�Ȥ���
		��³�������֤Τ��Ȥ�Session�ȸ�����
		ʪ�����󥽡���ϥޥ���˰�Ĥ���¸�ߤ��롣
		ʪ�����󥽡������³����Session�򥳥󥽡��륻�å����ȸ���
		�Ĥޤꥳ�󥽡��륻�å����ϥޥ���˰�Ĥ���¸�ߤ���
		��⡼�ȥǥ����ȥåפˤ����³����Session��
						��⡼�ȥ��å����ȸ���
		���󥽡��륻�å����ȥ�⡼�ȥ��å������Ф��ơ�
		ʪ�����󥽡���ȥ�⡼�ȥǥ����ȥåפ���³���ڤ��ؤ�����
		�Ĥޤꡢʪ�����󥽡������³����Ƥ��륳�󥽡��륻�å����
		���⡼�ȥǥ����ȥåפ��ڤ��ؤ��ƥ�⡼�ȥ��å����ˤǤ���
		"query session"���ޥ�ɤˤ�� --- �������� ---
 ���å����̾      �桼����̾               ID  ����    ����        �ǥХ���
 services                                    0  Disc
>console           user1                     2  Active
 rdp-tcp                                 65536  Listen
		"query session"���ޥ�ɤˤ�� --- �����ޤ� ---
		���å����̾��console�ǡ�
			Active�Ǥ����ʪ�����󥽡���˥����󤷤Ƥ���
			Connected�Ǥ���Ф��������󤷤Ƥ��ʤ�
		�Ĥޤꡢ���֤�Active�Ǥ����Το�����³���Ȥʤꡢ
		Active���⡢���å����̾��console��ʪ�������ʪ��ü��������
	*/
	bool logon_sw = false;
	for (unsigned ii=0;ii<pCount;++ii) {
#if defined DEBUG_IGS_RESOURCE_IS_LOGIN
		wts_session_print_(hWTServer,&pSessionInfo[ii]);
#endif  /* !DEBUG_IGS_RESOURCE_IS_LOGIN */
		if (WTSActive == pSessionInfo[ii].State) {
			logon_sw = true; /* logon���Ƥ��Ĥ�����! */
		}
	}
/* Source of Next Version 2012-10-15
	bool session_of_console_sw = false;
	int session_count = 0;
	const std::basic_string<TCHAR> cons(TEXT("Console"));
	for (unsigned ii=0;ii<pCount;++ii) {
		if (WTSActive == pSessionInfo[ii].State) {
			++session_count;
			if (cons == pSessionInfo[ii].pWinStationName) {
				session_of_console_sw = true;
			}
		}
	}
	return session_of_console_sw? session_count: -session_count;//??????
*/
	::WTSFreeMemory(pSessionInfo); // Return Nothing
	::WTSCloseServer(hWTServer);   // Return Nothing
	return logon_sw;
}

#if defined DEBUG_IGS_RESOURCE_IS_LOGIN
# include "igs_resource_msg_from_err_win.cxx"
int main(void)
{
  try {
		std::cout << std::endl;
	if ( igs::resource::is_login() ) {
		std::cout << TEXT("log on");
	} else {
		std::cout << TEXT("log off");
	}
		std::cout << std::endl;
  }
  catch (std::exception& e) {std::cerr<<"exception="<<e.what()<<std::endl;}
  catch (...) { std::cerr << "other exception" << std::endl; }
	return 0;
}
#endif  /* !DEBUG_IGS_RESOURCE_IS_LOGIN */
/*
rem vc2008sp1(32/64bits) :154,171 w! tes_msc_igs_rsc_is_login.bat
set _DEB=DEBUG_IGS_RESOURCE_IS_LOGIN
set _SRC=igs_resource_is_login
set _TGT=tes_msc_%PROCESSOR_ARCHITECTURE%_igs_rsc_is_login_mb
set _INC=.
set _LIB=wtsapi32.lib
cl /W4 /WX /MD /EHa /O2 /wd4819 /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _TGT=tes_msc_%PROCESSOR_ARCHITECTURE%_igs_rsc_is_login_wc
cl /W4 /WX /MD /EHa /O2 /wd4819 /DUNICODE /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _LIB=
set _INC=
set _TGT=
set _SRC=
set _DEB=
*/
/*
host1��user1��logon�������֤ǡ�host1�ˤ�
StationName=Services  UserName=        SessionId=0     State=WTSDisconnected
StationName=Console   UserName=user1   SessionId=3     State=WTSActive
StationName=RDP-Tcp   UserName=        SessionId=65536 State=WTSListen
host1����user1��host2��remote logon���ơ�host2�ˤ�
StationName=Services  UserName=        SessionId=0     State=WTSDisconnected
StationName=Console   UserName=        SessionId=1     State=WTSConnected
StationName=RDP-Tcp#0 UserName=user1   SessionId=2     State=WTSActive
StationName=RDP-Tcp   UserName=        SessionId=65536 State=WTSListen
host1����user1��kiki��remote logon���ơ�kiki�ˤ�
StationName=Services  UserName=        SessionId=0     State=WTSDisconnected
StationName=Console   UserName=        SessionId=2     State=WTSConnected
StationName=RDP-Tcp#0 UserName=user1   SessionId=3     State=WTSActive
StationName=RDP-Tcp   UserName=        SessionId=65536 State=WTSListen
host1����user1�ǡ�host3����user2�ǡ�host2��remote login����
StationName=Services  UserName=        SessionId=0     State=WTSDisconnected
StationName=Console   UserName=        SessionId=1     State=WTSConnected
StationName=RDP-Tcp#0 UserName=user1   SessionId=2     State=WTSActive
StationName=RDP-Tcp#1 UserName=user2   SessionId=3     State=WTSActive
StationName=RDP-Tcp   UserName=        SessionId=65536 State=WTSListen
*/
