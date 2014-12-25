#include <stdexcept>
#include "igs_resource_exist.h"
#include "igs_resource_wnet.h"
#include "igs_resource_msg_from_err.h"

namespace {
  LPCTSTR msg_from_add_err_( const int code ) {
	switch (code) {
	case ERROR_ACCESS_DENIED:
	return	TEXT("The caller does not have access to the ")
		TEXT("network resource.")
		;
	case ERROR_ALREADY_ASSIGNED:
	return	TEXT("The local device specified by the lpLocalName ")
		TEXT("member is already connected to a network resource.")
		;
	case ERROR_BAD_DEV_TYPE:
	return	TEXT("The type of local device and the type ")
		TEXT("of network resource do not match.")
		;
	case ERROR_BAD_DEVICE:
	return	TEXT("The value specified by lpLocalName is invalid.")
		;
	case ERROR_BAD_NET_NAME:
	return	TEXT("The value specified by the lpRemoteName member ")
		TEXT("is not acceptable to any network resource ")
		TEXT("provider, either because the resource name ")
		TEXT("is invalid, or because the named resource cannot ")
		TEXT("be located.")
		;
	case ERROR_BAD_PROFILE:
	return	TEXT("The user profile is in an incorrect format.")
		;
	case ERROR_BAD_PROVIDER:
	return	TEXT("The value specified by the lpProvider member ")
		TEXT("does not match any provider.")
		;
	case ERROR_BUSY:
	return	TEXT("The router or provider is busy, possibly ")
		TEXT("initializing. The caller should retry.")
		;
	case ERROR_CANCELLED:
	return	TEXT("The attempt to make the connection was canceled ")
		TEXT("by the user through a dialog box from one of the ")
		TEXT("network resource providers, ")
		TEXT("or by a called resource.")
		;
	case ERROR_CANNOT_OPEN_PROFILE:
	return	TEXT("The system is unable to open the user profile ")
		TEXT("to process persistent connections.")
		;
	case ERROR_DEVICE_ALREADY_REMEMBERED:
	return	TEXT("An entry for the device specified by ")
		TEXT("lpLocalName is already in the user profile.")
		;
	case ERROR_EXTENDED_ERROR:
	return	TEXT("A network-specific error occurred. ")
		TEXT("Call the WNetGetLastError function to ")
		TEXT("obtain a description of the error.")
		;
	case ERROR_INVALID_PASSWORD:
	return	TEXT("The specified password is invalid and the ")
		TEXT("CONNECT_INTERACTIVE flag is not set.")
		;
	case ERROR_NO_NET_OR_BAD_PATH:
	return	TEXT("The operation cannot be performed because ")
		TEXT("a network component is not started or ")
		TEXT("because a specified name cannot be used.")
		;
	case ERROR_NO_NETWORK:
	return	TEXT("The network is unavailable.");
	}
	return	TEXT("");
  }
  LPCTSTR msg_from_cancel_err_( const int code ) {
	switch (code) {
	case ERROR_BAD_PROFILE:
	return	TEXT("The user profile is in an incorrect format.");
 
	case ERROR_CANNOT_OPEN_PROFILE:
	return	TEXT("The system is unable to open the user profile ")
		TEXT("to process persistent connections.")
		;
	case ERROR_DEVICE_IN_USE:
	return	TEXT("The device is in use by an active process and ")
		TEXT("cannot be disconnected.")
		;
	case ERROR_EXTENDED_ERROR:
	return	TEXT("A network-specific error occurred. ")
		TEXT("To obtain a description of the error, ")
		TEXT("call the WNetGetLastError function.")
		;
	case ERROR_NOT_CONNECTED:
	return	TEXT("The name specified by the lpName parameter ")
		TEXT("is not a redirected device, ")
		TEXT("or the system is not currently connected ")
		TEXT("to the device specified by the parameter.")
		;
	case ERROR_OPEN_FILES:
	return	TEXT("There are open files, ")
		TEXT("and the fForce parameter is FALSE.")
		;
	}
	return	TEXT("");
  }
}
void igs::resource::wnet::connect(
	LPTSTR local_name	// "G:"
	,LPTSTR remote_name	// "\\host1\Users\Public"
) {
	NETRESOURCE net_resource;
	::ZeroMemory(&net_resource,sizeof(NETRESOURCE));
	net_resource.dwType = RESOURCETYPE_ANY;
	//net_resource.dwType = RESOURCETYPE_DISK; // ????????????????????
	net_resource.lpLocalName = local_name;   // LPTSTR
	net_resource.lpRemoteName = remote_name; // LPTSTR

	const DWORD result = ::WNetAddConnection2( &net_resource,0,0,0 );
	if (NO_ERROR != result) {
		throw std::domain_error( igs_resource_msg_from_er(
			msg_from_add_err_(result)
		));
	}
}
void igs::resource::wnet::connect_if_not_exist(
	LPTSTR local_name	// "G:"
	,LPTSTR remote_name	// "\\host1\Users\Public"
	,LPTSTR target_path	// "G:\irfm"
) {
	if (false == igs::resource::exist::file_or_directory(target_path)) {
		igs::resource::wnet::connect( local_name ,remote_name );
	}
}
void igs::resource::wnet::disconnect(
	LPTSTR local_name	// "N:"
) {
	const DWORD result = ::WNetCancelConnection2( local_name,0,FALSE );
	if (NO_ERROR != result) {
		throw std::domain_error( igs_resource_msg_from_er(
			msg_from_cancel_err_(result)
		));
	}
}

#if defined DEBUG_IGS_RESOURCE_WNET
# include <iostream>
# include "igs_resource_exist.cxx"
# include "igs_resource_msg_from_err.cxx"
int main(int argc, char *argv[]) {
  try {
	if (5 == argc && !strcmp( argv[1] ,"connect_if_not_exist" )) {
		std::cout << "connect_if_not_exist local=" << argv[2]
			<< " remote=" << argv[3]
			<< " path=" << argv[4]
			<< std::endl;
		igs::resource::wnet::connect_if_not_exist(
	const_cast<LPTSTR>(igs::resource::ts_from_mbs(argv[2]).c_str())
	,const_cast<LPTSTR>(igs::resource::ts_from_mbs(argv[3]).c_str())
	,const_cast<LPTSTR>(igs::resource::ts_from_mbs(argv[4]).c_str())
		);
	} else
	if (3 == argc && !strcmp( argv[1] ,"disconnect" )) {
		std::cout << "disconnect local=" << argv[2]
			<< std::endl;
		igs::resource::wnet::disconnect(
	const_cast<LPTSTR>(igs::resource::ts_from_mbs(argv[2]).c_str())
		);
	} else
	if (4 == argc && !strcmp( argv[1] ,"connect" )) {
		std::cout << "connect local=" << argv[2]
			<< " remote=" << argv[3] << std::endl;
		igs::resource::wnet::connect(
	const_cast<LPTSTR>(igs::resource::ts_from_mbs(argv[2]).c_str())
	,const_cast<LPTSTR>(igs::resource::ts_from_mbs(argv[3]).c_str())
		);
	} else {
		std::cout
		<< "Usage : " << argv[0] << " connect_if_not_exist local remote path\n"
		<< "Usage : " << argv[0] << " disconnect local\n"
		<< "Usage : " << argv[0] << " connect local remote\n"
		;
		return 1;
	}
  }
  catch (std::exception& e) {std::cerr<<"exception="<<e.what()<<std::endl;}
  catch (...) { std::cerr << "other exception" << std::endl; }
	return 0;
}
#endif  /* !DEBUG_IGS_RESOURCE_WNET */
/*
rem vc2008sp1(32/64bits) :191,208 w! tes_msc_igs_rsc_wnet.bat
set _DEB=DEBUG_IGS_RESOURCE_WNET
set _SRC=igs_resource_wnet
set _TGT=tes_msc_%PROCESSOR_ARCHITECTURE%_igs_rsc_wnet
set _INC=.
set _LIB=mpr.lib shlwapi.lib
cl /W4 /WX /MD /EHa /O2 /wd4819 /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _TGT=tes_msc_%PROCESSOR_ARCHITECTURE%_igs_rsc_wnet_uc
cl /W4 /WX /MD /EHa /O2 /wd4819 /DUNICODE /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _LIB=
set _INC=
set _TGT=
set _SRC=
set _DEB=
*/
