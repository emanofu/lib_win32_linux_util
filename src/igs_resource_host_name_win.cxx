/*
winsockの、
ver 1.1 or 1.0を使う場合(現状)、
	::WSAStartup(MAKEWORD(1,1) or ::WSAStartup(MAKEWORD(1,0)
	とし、
	USE_WINSOCK2を未定義としwindows.h定義による
ver 2以後を使う場合、
	::WSAStartup(MAKEWORD(2,0)
	とし、
	USE_WINSOCK2を定義しwinsock2.h定義による
	"ws2_32.lib"をリンクする

	注意:	winsock2.hより先にwindows.hをincludeしていると
		コンパイルエラーとなる。
	理由:	windows.h内部でwinsock.hがインクルードされているが、
  		winsock.h と winsock2.h 同時に使用することはできない。
	解決:	windows.hをインクルードする前に _WINSOCKAPI_をdefineする。
		個々に定義するよりプロジェクトオプションで定義したほうがよい

	解決2:	このファイルでwinsock2.hをincludeせず、
		使用するアプリのソースのwindows.h定義の前にwinsock2.hを置く
	考察1:	#pragma comment(lib,"ws2_32.lib")を定義する-->試してない!!!
*/
#if defined USE_WINSOCK2
# include <winsock2.h> // WSAStartup(-),gethostname(-),WSACleanup()
#endif
#include <stdexcept> // std::domain_error
#include "igs_resource_msg_from_err.h" // igs_resource_msg_from_err
namespace {
 void error_check_of_WSAStartup_( const int status ) {
	if (0 == status) { return; } // no error
	LPCTSTR msg=0;
	switch (status) {
	case WSASYSNOTREADY: msg =
		TEXT("The underlying network subsystem is not ")
		TEXT("ready for network communication.");
		break;
	case WSAVERNOTSUPPORTED: msg =
		TEXT("The version of Windows Sockets support ")
		TEXT("requested is not provided by this ")
		TEXT("particular Windows Sockets implementation.");
		break;
	case WSAEINPROGRESS: msg =
		TEXT("A blocking Windows Sockets 1.1 operation is ")
		TEXT("in progress.");
		break;
	case WSAEPROCLIM: msg =
		TEXT("A limit on the number of tasks supported by the ")
		TEXT("Windows Sockets implementation has been reached.");
		break;
	case WSAEFAULT: msg =
		TEXT("The lpWSAData parameter is not a valid pointer.");
		break;
	default: msg =
		TEXT("WSAStartup(-) unknown error");
		break;
	}
	throw std::domain_error(igs_resource_msg_from_er(msg));
 }
 void error_check_of_gethostname_( const int status ) {
	if (0 == status) { return; } // no error
	LPCTSTR msg=0;
	if (SOCKET_ERROR == status) {
		switch (::WSAGetLastError()) {
		case WSAEFAULT: msg =
			TEXT("The name parameter is a NULL pointer or is ")
			TEXT("not a valid part of the user address space. ")
			TEXT("This error is also returned if the buffer ")
			TEXT("size specified by namelen parameter is too ")
			TEXT("small to hold the complete host name.");
			break;
		case WSANOTINITIALISED: msg =
			TEXT("A successful WSAStartup call must occur ")
			TEXT("before using this function.");
			break;
		case WSAENETDOWN: msg =
			TEXT("The network subsystem has failed.");
			break;
		case WSAEINPROGRESS: msg =
			TEXT("A blocking Windows Sockets 1.1 call is in ")
			TEXT("progress, or the service provider is still ")
			TEXT("processing a callback function.");
			break;
		default: msg =
			TEXT("gethostname(-) unknown error");
			break;
		}
	} else {
 		msg = TEXT("gethostname() not SOCKET_ERROR error");
	}
	throw std::domain_error(igs_resource_msg_from_er(msg));
 }
 void error_check_of_WSACleanup_( const int status ) {
	if (0 == status) { return; } // no error
	LPCTSTR msg=0;
	if (SOCKET_ERROR == status) {
		switch (::WSAGetLastError()) {
		case WSANOTINITIALISED: msg =
			TEXT("A successful WSAStartup call must occur ")
			TEXT("before using this function.");
			break; 
		case WSAENETDOWN: msg =
			TEXT("The network subsystem has failed.");
			break; 
		case WSAEINPROGRESS: msg =
			TEXT("A blocking Windows Sockets 1.1 call is in ")
			TEXT("progress, or the service provider is still ")
			TEXT("processing a callback function.");
			break; 
		default: msg =
			TEXT("WSACleanup() unknown error");
			break;
		}
	} else {
 		msg = TEXT("WSACleanup() not SOCKET_ERROR error");
	}
	throw std::domain_error(igs_resource_msg_from_er(msg));
 }
 void host_name_win_(char *hostname, int namelen) {
	/*
		Win Sockからホスト名とる
		環境変数("COMPUTERNAME")からとれるが、こちらが本筋か?
	*/
	WSAData wsaData; ::ZeroMemory(&wsaData,sizeof(WSAData));
#if defined USE_WINSOCK2
	error_check_of_WSAStartup_( ::WSAStartup(MAKEWORD(2,0),&wsaData) );
#else
	error_check_of_WSAStartup_( ::WSAStartup(MAKEWORD(1,1),&wsaData) );
#endif
	hostname[0] = '\0';
  try { error_check_of_gethostname_(::gethostname(hostname,namelen) ); }
  catch(...) { ::WSACleanup(); throw; }
	hostname[namelen-1] = '\0';
	error_check_of_WSACleanup_( ::WSACleanup() );
 }
}
//--------------------------------------------------------------------
#include <cctype>	// tolower()
#include <string>	// std::basic_string<TCHAR>
# ifndef  HOST_NAME_MAX	/* gethostname(-)で使用 */
#  define HOST_NAME_MAX 255	/* unixでは定義されている */
# endif
#include "igs_resource_msg_from_err.h" // ts_from_mbs(-),igs_resource_msg_from_er(-)
#include "igs_resource_host_name.h"

/* WinSockからホスト名とる */
void igs::resource::host_name(std::basic_string<TCHAR>&name)
{
	/* 環境変数からホスト名をとる...のはしない */
	// igs::resource::getenv("COMPUTERNAME", name);

	/*
	文字列の型について、、、
	host_name_win_(-)内で、
	winsock関数の"int ::gethostname(char *name,int namelen)"
	を使うため、char *"にしなければならない
	*/
	char hostname[HOST_NAME_MAX]; hostname[0] = 0;
	host_name_win_(hostname,sizeof(hostname));

	hostname[HOST_NAME_MAX-1] = 0;
	std::string buffer(hostname);
	/* ホスト名がとれないときはエラー */
	if (buffer.empty()) {
		throw std::domain_error(igs_resource_msg_from_er(
			TEXT("Can not get host_name_win_(-)")
		));
	}
	/* ドメイン名からホスト名に当る部分のみにする...のはしない
	rhel4	--> hostrhel4.net1.host1.jp	--> hostrhel4
	darwin8	--> Host-MAC.local		--> Host-MAC
	rhel5	--> hostrhel5.net1.host1.jp	--> hostrhel5
	*/
	/*** std::string::size_type index = buffer.find('.');
	if (std::string::npos != index) { buffer.erase(index);
	}***/

	/* 小文字にする...のはしない
	例1	vc2008md 	--> HOST1	--> host1
	例2	darwin8		--> Host-MAC	--> host-mac
	*/
	/***for (unsigned ii = 0; ii < buffer.length(); ++ii) {
		buffer[ii] = static_cast<char>(tolower(buffer[ii]));
	}***/

	/* MBCSからUNICODE16へ変換 */
	name = igs::resource::ts_from_mbs(buffer);
}

#if defined DEBUG_IGS_RESOURCE_HOST_NAME_WIN
# include <iostream>
# include "igs_resource_msg_from_err.cxx"
int main( int argc, char *argv[] )
{
	if (1!=argc) { std::cout << "Usage:" << argv[0] << "\n"; return 1; }
  try {
    {
	char hostname_mb[HOST_NAME_MAX]; hostname_mb[0] = 0;
	host_name_win_(hostname_mb,sizeof(hostname_mb));
	std::basic_string<TCHAR> hostname_tc;
	igs::resource::host_name(hostname_tc);
	std::cout  <<      "1.host_name_win_(-) \""  << hostname_mb
							<< "\"\n";
	std::TCOUT << TEXT("2.host_name(-)      \"") << hostname_tc
							<< TEXT("\"\n");
    }
  }
  catch (std::exception& e) {std::cerr<<"exception=\""<<e.what()<<"\"\n";}
  catch (...) { std::cerr << "other exception\n"; }
	return 0;
}
#endif  /* !DEBUG_IGS_RESOURCE_HOST_NAME_WIN */
/*
rem vc2010(32/64bits) :236,255 w! tes_msc16_igs_rsc_host_name.bat
set _DEB=DEBUG_IGS_RESOURCE_HOST_NAME_WIN
rem set _DE2=USE_WINSOCK2
set _SRC=igs_resource_host_name_win
set _TGT=tes_msc16_%PROCESSOR_ARCHITECTURE%_igs_rsc_host_name_mb
set _INC=../src
rem --- winsock version 2.0 ---
rem set _LIB=ws2_32.lib
rem --- winsock version 1.1 ,1.0 ---
set _LIB=wsock32.lib
cl /W4 /WX /MD /EHa /O2 /wd4819 /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
del %_SRC%.obj
set _TGT=tes_msc16_%PROCESSOR_ARCHITECTURE%_igs_rsc_host_name_wc
cl /W4 /WX /MD /EHa /O2 /wd4819 /DUNICODE /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
del %_SRC%.obj
set _LIB=
set _INC=
set _TGT=
set _SRC=
set _DEB=
rem vc2008sp1(32/64bits) :236,257 w! tes_msc15_igs_rsc_host_name.bat
set _DEB=DEBUG_IGS_RESOURCE_HOST_NAME_WIN
rem set _DE2=USE_WINSOCK2
set _SRC=igs_resource_host_name_win
set _TGT=tes_msc15_%PROCESSOR_ARCHITECTURE%_igs_rsc_host_name_mb
set _INC=../src
rem --- winsock version 2.0 ---
rem set _LIB=ws2_32.lib
rem --- winsock version 1.1 ,1.0 ---
set _LIB=wsock32.lib
cl /W4 /WX /MD /EHa /O2 /wd4819 /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _TGT=tes_msc15_%PROCESSOR_ARCHITECTURE%_igs_rsc_host_name_wc
cl /W4 /WX /MD /EHa /O2 /wd4819 /DUNICODE /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _LIB=
set _INC=
set _TGT=
set _SRC=
set _DEB=
*/
