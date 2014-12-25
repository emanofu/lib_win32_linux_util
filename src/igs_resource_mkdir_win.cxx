#include <iostream>	// std::cout
#include <vector>	// std::vector
#include <string>	// std::basic_string<TCHAR>
#include <sstream>	// std::basic_ostringstream<TCHAR>
#include <stdexcept>	// std::domain_error
#include "igs_resource_msg_from_err.h"
#include "igs_resource_exist.h"
#include "igs_resource_mkdir.h"
#include "igs_resource_sleep.h"

/* �ǥ��쥯�ȥ�1�ĺ�����¸�ߤ��ǧ�����֤롢¹��ľ�ܤϤĤ���ʤ� */
void igs::resource::mkdir_certain(
	LPCTSTR dir_path
	,DWORD wait_milli_seconds	/* =  10(0.01��) */
	,int wait_times			/* =6000(60��) */
) {
	if (0 == ::CreateDirectory(dir_path ,NULL)) {
		throw std::domain_error(igs_resource_msg_from_err(
			dir_path ,::GetLastError()
		));
	}
/*
	CreateDirectory(-)�Τ��ȼºݤ�¸�ߤ�������ǧ��Ȥ��Ȥ���ȡ�
	���δ֤νִ֤ˡ�¾�Υץ�����RemoveDirectoy(-)��¹Ԥ��Ƥ��ޤ��ȡ�
	¸�߳�ǧLoop����λ���ʤ��ʤꡢ���顼���ꤲ�Ƥ��ޤ������ΤǤϡ���
	���ΤȤ����顼�ϵ����ʤ��Τ���α
	2012-10-19
*/
	/* ��������directory��¸�ߤ��ǧ���� */
	for (int retry_exist=0 ;retry_exist<wait_times ;++retry_exist) {
		if (igs::resource::exist::directory(dir_path)) {
			return;
		}
		igs::resource::sleep_m(wait_milli_seconds);
	}
	/* ���(wait_times)�����֤��ơ�����Ǥ����ʤ�TimeOut
	���顼�Ȥ����㳰���ꤲ�� */
	std::basic_ostringstream<TCHAR> ost;
	ost	<< TEXT("mkdir_certain() but not mkdir")
		<< TEXT(" \"") << dir_path << TEXT('\"')
		<< TEXT(' ') << wait_times << TEXT("times/loop")
		<< TEXT(' ') << wait_milli_seconds << TEXT("msec/time")
		;
	throw std::domain_error(igs_resource_msg_from_er(ost.str()));
}

/* �ǥ��쥯�ȥ�򷡤�ʤࡢ¹�ʲ������ˤ�ꤹ�٤ƺ�� */
void igs::resource::mkdir_descendant(LPCTSTR dir_path)
{
	std::basic_string<TCHAR> dirpath(dir_path);
	std::vector<std::basic_string<TCHAR>> dir_part;

	std::basic_string<TCHAR>::size_type before_index = 0;
	std::basic_string<TCHAR>::size_type index = 0;
	for (;std::basic_string<TCHAR>::npos!=index ;before_index=++index) {
		index = dirpath.find_first_of(TEXT("/\\"),before_index);
		if (index == before_index) { /* ���ڤ�ʸ����Ϣ³���� */
			dir_part.push_back(TEXT(""));
		} else if (std::basic_string<TCHAR>::npos == index) {
			dir_part.push_back(dirpath.substr(before_index));
			break; /* ���ڤ�ʸ�����⤦�ʤ��ʤ�Ǹ����ʬ */
		} else {
			dir_part.push_back(dirpath.substr(
				before_index,index-before_index
			));
		}
	}

	std::basic_string<TCHAR> dpath;
	for (unsigned ii = 0; ii < dir_part.size(); ++ii) {
		if (0 < ii) { dpath += TEXT("\\"); }
		dpath += dir_part[ii];
		if (dpath.size() <= 0) { continue; }

		/* "\\host1"��¸�ߥ����å����ʤ��褦��... */
		if ((2 == ii) &&
		(0 == dir_part[0].size()) &&
		(0 == dir_part[1].size())) { continue; }

		if (igs::resource::exist::file_or_directory(dpath.c_str())){
#if defined DEBUG_IGS_RESOURCE_MKDIR
std::TCOUT << TEXT("exist=\"") << dpath << TEXT("\"") << std::endl;
#endif  /* !DEBUG_IGS_RESOURCE_MKDIR */
			continue;
		}
#if defined DEBUG_IGS_RESOURCE_MKDIR
std::TCOUT << TEXT("mkdir=\"") << dpath << TEXT("\"") << std::endl;
#endif  /* !DEBUG_IGS_RESOURCE_MKDIR */
		igs::resource::mkdir_certain(dpath.c_str());
	}
}
/* lock���뤿��˥ǥ��쥯�ȥ�1�ĺ���(¹��ľ�ܤϤĤ���ʤ�)
	����ͤ�
		ERROR_SUCCESS		=0   ��������
		ERROR_ALREADY_EXISTS	=183 ����¸�ߤ���
		ERROR_ACCESS_DENIED	=5   ������������
		ERROR_NETWORK_ACCESS_DENIED = 64
			���ꤵ�줿�ͥåȥ��̾�����ѤǤ��ޤ���
	�λ��ġ�
	����ʳ��Υ��顼���㳰��throw���� */
DWORD igs::resource::mkdir_for_lock(LPCTSTR dir_path)
{
	/*
		MSDN���
		�ѥ�̾��ʸ����κ����Ĺ���ϡ�����Ǥ� 248 ʸ���Ǥ���
		�������¤ϡ��ؿ����ѥ���ʬ�Ϥ�����ˡ�ˤ�ط����Ƥ��ޤ���
		�������¤�ۤ� 32,000 �磻��ʸ���س�ĥ����ˤϡ�
		���δؿ��� Unicode �Ǥ�ƤӽФ���
		�ѥ������� "\\?\" �Ȥ�����Ƭ�����ɲä��Ƥ���������
		shlwapi.h
		shlwapi.lib
	*/
	if (0 != ::CreateDirectory(dir_path ,NULL)) {
		return ERROR_SUCCESS;		/* =0   ��������	*/
	}
	const DWORD err = ::GetLastError();
	if ((ERROR_ALREADY_EXISTS == err)	/* =183 ����¸�ߤ���	*/
	||  (ERROR_ACCESS_DENIED  == err)	/* =5   ������������	*/
	||  (ERROR_NETWORK_ACCESS_DENIED == err)
					/* =64  �ͥåȥ��̾�����Բ�	*/
	) {
		return err;
	}
	throw std::domain_error(igs_resource_msg_from_err(
		dir_path ,err
	));
}

#if defined DEBUG_IGS_RESOURCE_MKDIR
# include <iostream>
# include "igs_resource_msg_from_err.cxx"
# include "igs_resource_exist.cxx"
# include "igs_resource_sleep.cxx"
int main( int argc, char *argv[] )
{
	if (2 != argc) {
		std::cout << "Usage " << argv[0] << " newdirpath\n";
		std::cout << "Exsample\n";
		std::cout << "  C:\\Users\\user1\\tmp\n";
		std::cout << "  C:/Users/user1/tmp\n";
		std::cout << "  \\\\host1\\Users\\user1\\tmp\n";
		std::cout << "  //host1/Users/user1/tmp\n";
		return 1;
	}
  try {
	igs::resource::mkdir_descendant(igs::resource::ts_from_mbs(argv[1]).c_str());
  }
  catch (std::exception& e) {
	std::cerr << "exception=\"" << e.what() << "\"\n";
  }
  catch (...) {
	std::cerr << "other exception\n";
  }
	return 0;
}
#endif  /* !DEBUG_IGS_RESOURCE_MKDIR */
/*
rem vc2010(32/64bits) :163,178 w! tes_msc16_igs_rsc_mkdir.bat
set _DEB=DEBUG_IGS_RESOURCE_MKDIR
set _SRC=igs_resource_mkdir
set _TGT=tes_msc16_%PROCESSOR_ARCHITECTURE%_igs_rsc_mkdir_mb
set _INC=.
set _LIB=shlwapi.lib
cl /W4 /WX /MD /EHa /O2 /wd4819 /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
del %_SRC%.obj
set _TGT=tes_msc16_%PROCESSOR_ARCHITECTURE%_igs_rsc_mkdir_wc
cl /W4 /WX /MD /EHa /O2 /wd4819 /DUNICODE /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
del %_SRC%.obj
set _LIB=
set _INC=
set _TGT=
set _SRC=
set _DEB=
rem vc2008sp1(32/64bits) :148,165 w! tes_msc15_igs_rsc_mkdir.bat
set _DEB=DEBUG_IGS_RESOURCE_MKDIR
set _SRC=igs_resource_mkdir
set _TGT=tes_msc15_%PROCESSOR_ARCHITECTURE%_igs_rsc_mkdir_mb
set _INC=.
set _LIB=shlwapi.lib
cl /W4 /WX /MD /EHa /O2 /wd4819 /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _TGT=tes_msc15_%PROCESSOR_ARCHITECTURE%_igs_rsc_mkdir_wc
cl /W4 /WX /MD /EHa /O2 /wd4819 /DUNICODE /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _LIB=
set _INC=
set _TGT=
set _SRC=
set _DEB=
*/
