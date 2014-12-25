#include <shlwapi.h>	// ::PathIsDirectory(-)
#include <iostream>	// std::cout
#include <iomanip>	// std::setw(-),std::setfill(-)
#include <stdexcept>	// std::domain_error
#include <iomanip>	// std::setw(2),std::setfill('0')
#include "igs_resource_time.h"
#include "igs_resource_msg_from_err.h"

igs::resource::time::parameters::parameters()
	:year(-1) ,month(-1) ,month_day(-1)
	,hour(-1) ,minute(-1) ,second(-1)
	,week_day(-1) ,year_day(-1) ,milli_second(-1)
{
	::ZeroMemory( &this->file_time ,sizeof(FILETIME) );
	::ZeroMemory( &this->system_time ,sizeof(SYSTEMTIME) );
}
void igs::resource::time::parameters::clear(void) {
	this->year = -1;
	this->month = -1;
	this->month_day = -1;

	this->hour = -1;
	this->minute = -1;
	this->second = -1;

	this->week_day = -1;
	this->year_day = -1;
	this->milli_second = -1;

	::ZeroMemory( &this->file_time ,sizeof(FILETIME) );
	::ZeroMemory( &this->system_time ,sizeof(SYSTEMTIME) );
}

void igs::resource::time::ymdhms_to_param(
	const int year ,const int month ,const int month_day
	,const int hour ,const int minute ,const int second
	,igs::resource::time::parameters&time_pa
) {
	time_pa.year		= year;
	time_pa.month		= month;
	time_pa.month_day	= month_day;
	time_pa.hour		= hour;
	time_pa.minute		= minute;
	time_pa.second		= second;
	time_pa.week_day	= -1; /* ̤���� */
	time_pa.year_day	= -1; /* ̤���� */
	time_pa.milli_second	= -1; /* ̤���� */

	time_pa.system_time.wYear   = static_cast<WORD>(time_pa.year);
	time_pa.system_time.wMonth  = static_cast<WORD>(time_pa.month);
	time_pa.system_time.wDay    = static_cast<WORD>(time_pa.month_day);
	time_pa.system_time.wHour   = static_cast<WORD>(time_pa.hour);
	time_pa.system_time.wMinute = static_cast<WORD>(time_pa.minute);
	time_pa.system_time.wSecond = static_cast<WORD>(time_pa.second);
	time_pa.system_time.wDayOfWeek = 0; /* ����0(������)������Ƥ��� */
	time_pa.system_time.wMilliseconds = 0; /* ����0������Ƥ��� */

	if (0 == ::SystemTimeToFileTime(
		&time_pa.system_time ,&time_pa.file_time
	)) {
		throw std::domain_error(igs_resource_msg_from_err(
			TEXT("set(-):SystemTimeToFileTime(-)")
			,::GetLastError()
		));
	}
}
void igs::resource::time::now_to_param(
	igs::resource::time::parameters&time_pa
) {
	::GetLocalTime( &time_pa.system_time );
	if (0 == ::SystemTimeToFileTime(
		&time_pa.system_time ,&time_pa.file_time
	)) {
		throw std::domain_error(igs_resource_msg_from_err(
			TEXT("get(-):SystemTimeToFileTime(-)")
			,::GetLastError()
		));
	}
	time_pa.year		= time_pa.system_time.wYear;
	time_pa.month		= time_pa.system_time.wMonth;
	time_pa.month_day	= time_pa.system_time.wDay;
	time_pa.hour		= time_pa.system_time.wHour;
	time_pa.minute		= time_pa.system_time.wMinute;
	time_pa.second		= time_pa.system_time.wSecond;
	time_pa.week_day	= time_pa.system_time.wDayOfWeek;
	time_pa.year_day	= -1; /* ̤���� */
	time_pa.milli_second	= time_pa.system_time.wMilliseconds;
}
void igs::resource::time::file_timestamp_to_param(
	LPCTSTR file_path
	,igs::resource::time::parameters&time_pa
) {
	HANDLE handle = 0;
	if (::PathIsDirectory(file_path)) {
		handle = ::CreateFile(
			file_path ,GENERIC_READ ,FILE_SHARE_READ ,NULL
			,OPEN_EXISTING ,FILE_FLAG_BACKUP_SEMANTICS ,NULL
		);
	} else {
		handle = ::CreateFile(
			file_path ,GENERIC_READ ,FILE_SHARE_READ ,NULL
			,OPEN_EXISTING ,FILE_ATTRIBUTE_NORMAL ,NULL
		);
	}
	if (INVALID_HANDLE_VALUE == handle) {
		throw std::domain_error(igs_resource_msg_from_err(
			file_path,::GetLastError()
		));
	}
	FILETIME last_write_time;
	::ZeroMemory( &last_write_time ,sizeof(FILETIME) );
	if (0 == ::GetFileTime(
		handle,NULL,NULL,&last_write_time
	)) {
		const DWORD err = ::GetLastError();
		::CloseHandle(handle);
		throw std::domain_error(igs_resource_msg_from_err(
			file_path,err
		));
	}
	if (0 == ::CloseHandle(handle)) {
		throw std::domain_error(igs_resource_msg_from_err(
			file_path,::GetLastError()
		));
	}
	if (0 == ::FileTimeToLocalFileTime(
		&last_write_time ,&time_pa.file_time
	)) {
		throw std::domain_error(igs_resource_msg_from_err(
			file_path,::GetLastError()
		));
	}
	if (0 == ::FileTimeToSystemTime(
		&time_pa.file_time, &time_pa.system_time
	)) {
		throw std::domain_error(igs_resource_msg_from_err(
			file_path,::GetLastError()
		));
	}
	time_pa.year		= time_pa.system_time.wYear;
	time_pa.month		= time_pa.system_time.wMonth;
	time_pa.month_day	= time_pa.system_time.wDay;
	time_pa.hour		= time_pa.system_time.wHour;
	time_pa.minute		= time_pa.system_time.wMinute;
	time_pa.second		= time_pa.system_time.wSecond;
	time_pa.week_day	= time_pa.system_time.wDayOfWeek;
	time_pa.year_day	= -1; /* ̤���� */
	time_pa.milli_second	= time_pa.system_time.wMilliseconds;
}
/* ɽ���Τ����WIN32�񼰤����� */
void igs::resource::time::form_os(
	const igs::resource::time::parameters&time_pa
	,std::basic_ostringstream<TCHAR>&ost
) {
	{
		TCHAR local_date[255];
		if (0 == ::GetDateFormat(
			LOCALE_USER_DEFAULT,DATE_LONGDATE
			,&time_pa.system_time,NULL,local_date,255
		)) {
			throw std::domain_error(igs_resource_msg_from_err(
				TEXT("::GetDateFormat(-)"),::GetLastError()
			));
		}
		ost << local_date;
	}
	{
		TCHAR local_time[255];
		if (0 == ::GetTimeFormat(
			LOCALE_USER_DEFAULT,0
			,&time_pa.system_time,NULL,local_time,255
		)) {
			throw std::domain_error(igs_resource_msg_from_err(
				TEXT("::GetDateFormat(-)"),::GetLastError()
			));
		}
		ost << TEXT(' ');
		ost << local_time;
	}
}
namespace {
  /* ���ﺹ��100nsecñ�̤����� */
  void diff_(
	const FILETIME file_time1
	,const FILETIME file_time2
	,ULONGLONG&per100nsec
	,bool&minus_sw	/* "2 < 1"�ʤ�true */
  ) {
	ULONGLONG nsec1 = (static_cast<ULONGLONG>(
		file_time1.dwHighDateTime) << 32) +
		file_time1.dwLowDateTime;
	ULONGLONG nsec2 = (static_cast<ULONGLONG>(
		file_time2.dwHighDateTime) << 32) +
		file_time2.dwLowDateTime;

	if (nsec2 < nsec1) {
		minus_sw = true;
		per100nsec = nsec1 - nsec2;
		return;
	}
	minus_sw = false;
	per100nsec = nsec2 - nsec1;
  }
}
int igs::resource::time::second_diff(
	const igs::resource::time::parameters&time_pa1
	,const igs::resource::time::parameters&time_pa2
) {
	ULONGLONG per100nsec=0;
	bool minus_sw=false;
	diff_(time_pa1.file_time ,time_pa2.file_time ,per100nsec ,minus_sw);
/******	std::cout
		<< "diff minus_sw=" << minus_sw
		<< " per100nsec=" << per100nsec
		<< " microsec=" << per100nsec / 10
		<< " millisec=" << per100nsec / 10 / 1000
		<< " sec=" << per100nsec / 10 / 1000 / 1000
		<< "\n";
******/
	const int val = static_cast<int>(per100nsec / 10 / 1000 / 1000);
	return minus_sw ?-val :val;
}
/*
��ʬ����ꤷ�����߻������λ��������
�㤨�С�10:00����
10:01����ꤷ���顢������10:01
09:59����ꤷ���顢��������09:59
������
*/
void igs::resource::time::next_hour_minute(
	const igs::resource::time::parameters&time_now
	,const int next_hour
	,const int next_minute
	,igs::resource::time::parameters&time_next
) {
	/* ���ߤȤλ��ֺ���ʬ������ */
	int minute_for_shift =
		(next_hour - time_now.hour) * 60
		+ (next_minute - time_now.minute)
		;

	/* ���ֺ����ޥ��ʥ��ξ�硢���߻������λ�����Ѵ����� */
	if (minute_for_shift < 0) {
		/* 10:00����09:59����ꤷ���顢��������09:59 */
		minute_for_shift += 24 * 60;
	}

	igs::resource::time::shift_second(
		time_now ,minute_for_shift*60/* ʬ���ä� */ ,time_next
	);
}
/* ���߻��狼�����ñ�̤��ѹ� */
void igs::resource::time::shift_second(
	const igs::resource::time::parameters&time_now
	,const int second_for_shift
	,igs::resource::time::parameters&time_next
) {
	/* �ä�100nsecñ�̤��Ѵ� */
	const ULONGLONG h100nsec_shift =
		static_cast<ULONGLONG>(second_for_shift) * 10 * 1000 * 1000;

	/* 100nsecñ�̤Ǹ��߻��֤����� */
	const ULONGLONG h100nsec_now = (static_cast<ULONGLONG>(
		time_now.file_time.dwHighDateTime) << 32) +
		time_now.file_time.dwLowDateTime;

	/* ���ֺ���ä��� */
	const ULONGLONG h100nsec = h100nsec_now + h100nsec_shift;

	/* 100nsecñ�̤�file_time��¤�Τˤ�ɤ� */
	time_next.file_time.dwHighDateTime =
			static_cast<DWORD>((h100nsec >> 32) & 0xffffffff);
	time_next.file_time.dwLowDateTime =
			static_cast<DWORD>(h100nsec & 0xffffffff);

	/* file_time��¤���ͤ�ǯ������ʬ�ä��ͤˤ��� */
	if (0 == ::FileTimeToSystemTime(
		&time_next.file_time, &time_next.system_time
	)) {
		throw std::domain_error(igs_resource_msg_from_err(
			TEXT("FileTimeToSystemTime(-)"),::GetLastError()
		));
	}
	time_next.year		= time_next.system_time.wYear;
	time_next.month		= time_next.system_time.wMonth;
	time_next.month_day	= time_next.system_time.wDay;
	time_next.hour		= time_next.system_time.wHour;
	time_next.minute	= time_next.system_time.wMinute;
	time_next.second	= time_next.system_time.wSecond;
	time_next.week_day	= time_next.system_time.wDayOfWeek;
	time_next.year_day	= -1; /* ̤���� */
	time_next.milli_second	= time_next.system_time.wMilliseconds;
}

/* ɽ���Τ�����ȼ��񼰤����� */
void igs::resource::time::form(
	const igs::resource::time::parameters&time_pa
	,std::basic_ostringstream<TCHAR>&ost
) {
	LPCTSTR weeks[] = {
		TEXT("Sun")
		,TEXT("Mon"),TEXT("Tue"),TEXT("Wed")
		,TEXT("Thr"),TEXT("Fri"),TEXT("Sat")
		,TEXT("...")
	};
	const int week = (time_pa.week_day < 0)? 7: time_pa.week_day;
	ost
	 << time_pa.year
	 << TEXT('-')
	 << std::setw(2) << std::setfill(TEXT('0')) << time_pa.month
	 << TEXT('-')
	 << std::setw(2) << std::setfill(TEXT('0')) << time_pa.month_day
	 << TEXT('-') << weeks[week]
	 ;
	if (0 <= time_pa.year_day) {
	ost
	 << TEXT(' ')
	 << std::setw(3) << std::setfill(TEXT('0')) << time_pa.year_day
		<< TEXT("/year")
	 ;
	}
	ost
	 << TEXT(' ')
	 << std::setw(2) << std::setfill(TEXT('0')) << time_pa.hour
	 << TEXT(':')
	 << std::setw(2) << std::setfill(TEXT('0')) << time_pa.minute
	 << TEXT(':')
	 << std::setw(2) << std::setfill(TEXT('0')) << time_pa.second
	 ;
	if (0 <= time_pa.milli_second) {
	ost
	 << TEXT(' ')
	 << std::setw(3) << std::setfill(TEXT('0')) << time_pa.milli_second
		<< TEXT("msec")
	 ;
	}
	ost
	 << TEXT(" H=")
	 << time_pa.file_time.dwHighDateTime
	 << TEXT(" L=")
	 << time_pa.file_time.dwLowDateTime
	 ;
}
void igs::resource::time::compile_ymdhms(
	int&year ,int&month ,int&day
	,int&hour ,int&minute ,int&second
	,const std::string&complie_date
	,const std::string&complie_time
) {
 { /*------ ǯ���� ------*/
	std::istringstream ist(complie_date);
	std::string month_str;
	ist >> month_str >> day >> year;

	/* __DATE__��ɽ��������̾���Υꥹ�� */
	const std::string month_names(
		"JanFebMarAprMayJunJulAugSepOctNovDec"
	);
	/* ���ϰ��� */
	std::string::size_type index1 = month_names.find( month_names );
	/* ���߷� */
	std::string::size_type index2 = month_names.find( month_str );
	/* ���߷����Ǥ��ʤ����顼 */
	if (std::string::npos == index2) {
		// std::basic_string<TCHAR> msg(igs::resource::ts_from_mbs( ist.str()));
		// throw std::domain_error(igs_resource_msg_from_er(msg));
		return;
	}
	month = static_cast<int>(index2-index1)/3+1;
 }

 { /*------ ��ʬ�� ------*/
	std::istringstream ist(complie_time);
	char cc=0;
	ist >> hour >> cc >> minute >> cc >> second;
 }
}
#if defined DEBUG_IGS_RESOURCE_TIME
#include "igs_resource_msg_from_err.cxx"
namespace {
  void pr_time_pa_(
	const std::basic_string<TCHAR>&name
	,const igs::resource::time::parameters&time_pa
  ) {
	std::basic_string<TCHAR> so(name);
	so += TEXT("(org) \"");
	{	std::basic_ostringstream<TCHAR> ost;
		igs::resource::time::form_os(time_pa,ost);
		so+= ost.str();
	}
	so += TEXT("\"\n");

	std::basic_string<TCHAR> sc(name);
	sc += TEXT("(cus) \"");
	{	std::basic_ostringstream<TCHAR> ost;
		igs::resource::time::form(time_pa,ost);
		sc+= ost.str();
	}
	sc += TEXT("\"\n");

	std::TCOUT << so << sc ;
  }
}
int main( int argc, char *argv[] )
{
	/* locale�����ܤ����ꤷ���ܸ�򰷤����Ȥ�ؼ�(���ʤ������ܸ�ʸ
	����ʬ�Τ߽������ʤ�)�����������ͥ��ƥ����C locale�Τޤޤ�(��
	�ʤ���3�头�Ȥ˥���ޤ��դ�(1000-->1,000)) */
	/***std::locale::global(
	 std::locale(std::locale(),"japanese",std::locale::ctype)
	);***/
	igs::resource::locale_to_jp();
  try {
	if (2 == argc && !strcmp("now",argv[1])) {
		igs::resource::time::parameters tpa_now;
		igs::resource::time::now_to_param(tpa_now);
		pr_time_pa_(TEXT("now "),tpa_now);
	} else
	if (3 == argc && !strcmp("file",argv[1])) {
		igs::resource::time::parameters tpa_now,tpa_fil;
		igs::resource::time::now_to_param(tpa_now);
		pr_time_pa_(TEXT("now "),tpa_now);

		igs::resource::time::file_timestamp_to_param(
			igs::resource::ts_from_mbs(argv[2]).c_str()
			,tpa_fil
		);
		pr_time_pa_(TEXT("file"),tpa_fil);

		std::TCOUT
		<< TEXT("diff=") << igs::resource::time::second_diff(
			tpa_fil,tpa_now)
		<< TEXT("sec\n")
		;
	} else
	if (8 == argc && !strcmp("set",argv[1])) {
		igs::resource::time::parameters tpa_now,tpa_set;
		igs::resource::time::now_to_param(tpa_now);
		pr_time_pa_(TEXT("now "),tpa_now);

		igs::resource::time::ymdhms_to_param(
			 ::atoi(argv[2])
			,::atoi(argv[3])
			,::atoi(argv[4])
			,::atoi(argv[5])
			,::atoi(argv[6])
			,::atoi(argv[7])
			,tpa_set
		);
		pr_time_pa_(TEXT("set "),tpa_set);

		std::cout
		<< TEXT("diff=") << igs::resource::time::second_diff(
			tpa_set,tpa_now)
		<< TEXT("sec\n")
		;
	} else
	if (4 == argc && !strcmp("next",argv[1])) {
		igs::resource::time::parameters tpa_now,tpa_next;
		igs::resource::time::now_to_param(tpa_now);
		pr_time_pa_(TEXT("now "),tpa_now);

		igs::resource::time::next_hour_minute(
			tpa_now
			,::atoi(argv[2])
			,::atoi(argv[3])
			,tpa_next
		);
		pr_time_pa_(TEXT("next"),tpa_next);
	} else
	if (2 == argc && !strcmp("compile",argv[1])) {
		igs::resource::time::parameters time_pa;
		igs::resource::time::compile_ymdhms(
			 time_pa.year ,time_pa.month  ,time_pa.month_day
			,time_pa.hour ,time_pa.minute ,time_pa.second
		);
		igs::resource::time::ymdhms_to_param(
			 time_pa.year ,time_pa.month  ,time_pa.month_day
			,time_pa.hour ,time_pa.minute ,time_pa.second
			,time_pa
		);
		pr_time_pa_(TEXT("compile "),time_pa);
	} else {
		std::cout << "Usage " << argv[0] << " now\n";
		std::cout << "Usage " << argv[0] << " file file_path\n";
		std::cout << "Usage " << argv[0] << " set y m d h m s\n";
		std::cout << "Usage " << argv[0] << " next h m\n";
		std::cout << "Usage " << argv[0] << " compile\n";
		return 1;
	}
  }
  catch (std::exception& e) {std::cerr<<"exception="<<e.what()<<std::endl;}
  catch (...) { std::cerr << "other exception" << std::endl; }
	return 0;
}
#endif  /* !DEBUG_IGS_RESOURCE_TIME */
/*
rem vc2010(32/64bits) :498,513 w! tes_msc16_igs_rsc_time.bat
set _DEB=DEBUG_IGS_RESOURCE_TIME
set _SRC=igs_resource_time
set _TGT=tes_msc16_%PROCESSOR_ARCHITECTURE%_igs_rsc_time_mb
set _INC=.
set _LIB=shlwapi.lib
cl /W4 /WX /MD /EHa /O2 /wd4819 /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
del %_SRC%.obj
set _TGT=tes_msc16_%PROCESSOR_ARCHITECTURE%_igs_rsc_time_wc
cl /W4 /WX /MD /EHa /O2 /wd4819 /DUNICODE /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
del %_SRC%.obj
set _LIB=
set _INC=
set _TGT=
set _SRC=
set _DEB=
rem vc2008sp1(32/64bits) :514,531 w! tes_msc15_igs_rsc_time.bat
set _DEB=DEBUG_IGS_RESOURCE_TIME
set _SRC=igs_resource_time
set _TGT=tes_msc15_%PROCESSOR_ARCHITECTURE%_igs_rsc_time_mb
set _INC=.
set _LIB=shlwapi.lib
cl /W4 /WX /MD /EHa /O2 /wd4819 /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _TGT=tes_msc15_%PROCESSOR_ARCHITECTURE%_igs_rsc_time_wc
cl /W4 /WX /MD /EHa /O2 /wd4819 /DUNICODE /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _LIB=
set _INC=
set _TGT=
set _SRC=
set _DEB=
*/
