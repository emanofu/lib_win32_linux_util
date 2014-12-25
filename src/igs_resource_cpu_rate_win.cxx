#if 0//---------------------------------------------------------------
/*
	= 0; ::Sleep(milli_seconds);
	以下のエラーに対する回避処理、、、2012-02-28
	 host1   Windows7 Ultimate SP1 64bits
	 host3   Windows Server 2008 R2 Datacenter SP1 64bits
	 host4  Windows7 Professional SP1 64bits
	の中でhost1でのみ、
	"igs::resource::cpu_rate_of_use(-)"内の、
	PdhCollectQueryData(-)にて、
	"The Query does not currently have any counter"
	というエラーが起きる。
	単体では起きず。
	windowsのserviceプログラムに入れることで起きる。
	組み合わせは関係ない。
*/
/*
PDH(Performance Data Helper)によるCPU使用率の調査方法について(2012-04-13)
現状
	コメントアウトしてある
理由
	host1(Windows7ultimate)
	host2(Windows7Server2008R2Datecenter)
	host4(Windows7Professional)
	にて、serviceプログラムとして実行したとき、
	2012-04-12 16:08:55 errs "igs_resource_cpu_rate_win.cxx:58:_MSC_VER:150030729:2012:Apr:12:12:15:16" "void __cdecl `anonymous-namespace'::error_throw_(const wchar_t *,long,void *)" "PdhCollectQueryData(-)1st:The query does not currently have any counters."
	というようなエラーとなる。
	昨年末のテスト時は正常動作していた。
	OSに何か変化あったのか?
	原因不明。
	::GetSystemTimes(-)を使う方法だと正常動作するので、
	そちらに切り替えてこちらは使用しない。
今後
	エラーの原因がわかり、
	別途必要なら別関数に再設計し作成する。
	使わないことが明確になったら破棄する
*/
#include <pdh.h>	// PdhOpenQuery(-)...
#include <pdhmsg.h>	// PDH_INVALID_ARGUMENT...
#include <stdexcept>	// std::domain_error()
#include "igs_resource_msg_from_err.h"
#include "igs_resource_sleep.h"
#include "igs_resource_cpu_rate_of_use.h"
namespace {
 void error_throw_( LPCTSTR tit, PDH_STATUS val, HQUERY hQuery ) {
	LPCTSTR msg = 0;
	switch (val) {
	case PDH_CSTATUS_BAD_COUNTERNAME:
		msg =	TEXT("The counter name path string could not be ")
			TEXT("parsed or interpreted.");
		break;
	case PDH_CSTATUS_NO_COUNTER:
		msg =	TEXT("The specified counter was not found.");
		break;
	case PDH_CSTATUS_NO_COUNTERNAME:
		msg =	TEXT("An empty counter name path string was ")
			TEXT("passed in.");
		break;
	case PDH_CSTATUS_NO_MACHINE:
		msg =	TEXT("A computer entry could not be created.");
		break;
	case PDH_CSTATUS_NO_OBJECT:
		msg =	TEXT("The specified object could not be found.");
		break;
	case PDH_FUNCTION_NOT_FOUND:
		msg =	TEXT("The calculation function for this counter ")
			TEXT("could not be determined.");
		break;
	case PDH_INVALID_ARGUMENT:
		msg =	TEXT("One or more arguments are invalid.");
		break;
	case PDH_INVALID_HANDLE:
		msg =	TEXT("The query handle is not valid.");
		break;
	case PDH_MEMORY_ALLOCATION_FAILURE:
		msg =	TEXT("A memory buffer could not be allocated.");
		break;
	case PDH_NO_DATA:
		msg =	TEXT("The query does not currently have ")
			TEXT("any counters.");
		break;
	case PDH_INVALID_DATA:
/* 指定したカウンタには、有効なデータも成功を示す状態コードもありません */
		msg =	TEXT("invalid data");
		break;
	default:
		if (ERROR_SUCCESS != val) {
			msg = TEXT("any error");
		}
		break;
	}
	if (0 != msg) {
		if (0 != hQuery) { PdhCloseQuery(hQuery); }
		std::basic_string<TCHAR> str(tit); str+=TEXT(':'); str+=msg;
		throw std::domain_error( igs_resource_msg_from_er( str ) );
	}
 }
}
/* 1second = 1,000milli_seconds(ミリ秒) */
double igs::resource::cpu_rate(const DWORD milli_seconds) {
	/* 01 構造体を作成し、初期化 */
	HQUERY hQuery=0;
	PDH_STATUS ret = ::PdhOpenQuery(NULL, 0, &hQuery);
	error_throw_( TEXT("PdhOpenQuery(-)"), ret, 0 );

	/* 02 CPU使用率のカウンタをクエリに追加 */
	HCOUNTER hCounter=0;
	ret = ::PdhAddCounter(
		hQuery
		, TEXT("\\Processor(_Total)\\% Processor Time")
		//, TEXT("\\Processor(Idle#0)\\% Processor Time")
		, 0
		, &hCounter
	);
	error_throw_( TEXT("PdhAddCounter(-)"), ret, hQuery );

	/* 03 計測 */
	ret = ::PdhCollectQueryData(hQuery);
	error_throw_( TEXT("PdhCollectQueryData(-)1st"), ret, hQuery );
	/* 指定のミリ秒待つ */
	igs::resource::sleep_m(milli_seconds);
	/* 04 計測 */
	ret = ::PdhCollectQueryData(hQuery);
	error_throw_( TEXT("PdhCollectQueryData(-)2nd"), ret, hQuery );
	/* 05 カウンタ現在値取得 */
	PDH_FMT_COUNTERVALUE FmtValue;
	/***::memset(static_cast<void *>(&FmtValue),0,sizeof(FmtValue));***/
	//::ZeroMemory(static_cast<PVOID>(FmtValue),sizeof(FmtValue));
	::ZeroMemory(&FmtValue,sizeof(FmtValue));
	ret = ::PdhGetFormattedCounterValue(
		hCounter
		, PDH_FMT_DOUBLE | PDH_FMT_NOSCALE
		, NULL
		, &FmtValue
	);
	error_throw_( TEXT("PdhGetFormattedCounterValue(-)"), ret, hQuery );

	/* 06 終了 */
	ret = ::PdhCloseQuery(hQuery);
	error_throw_( TEXT("PdhCloseQuery(-)"), ret, 0 );

	/* percentをrateに変換する */
	return FmtValue.doubleValue / 100.;
	//return 1 - (FmtValue.doubleValue / 100.);
}
#endif//--------------------------------------------------------------

#include "igs_resource_msg_from_err.h"
#include "igs_resource_sleep.h"
#include "igs_resource_cpu_rate.h"
namespace {
  struct filetime { FILETIME  idle,kernel,user; };
  struct calctime { ULONGLONG idle,kernel,user; };
  ULONGLONG ulonglong_from_filetime_(const FILETIME&ft) {
    return (static_cast<ULONGLONG>(ft.dwHighDateTime)<<32)+ft.dwLowDateTime;
  }
  void filetime_and_ulonglong_( filetime&file ,calctime&calc ) {
	if (0 == ::GetSystemTimes(&file.idle,&file.kernel,&file.user)) {
		throw std::domain_error(igs_resource_msg_from_err(
			TEXT("GetSystemTimes(-)"),::GetLastError()
		));
	}
	/* kernel時間はidle時間を含んでいる */
	calc.idle   = ulonglong_from_filetime_( file.idle );
	calc.kernel = ulonglong_from_filetime_( file.kernel );
	calc.user   = ulonglong_from_filetime_( file.user );
  }
}
double igs::resource::cpu_rate( const DWORD msec ) {
	filetime file1,file2;
	calctime calc1,calc2;
	filetime_and_ulonglong_( file1 ,calc1 );
	igs::resource::sleep_m(msec);
	filetime_and_ulonglong_( file2 ,calc2 );

	const ULONGLONG idle = calc2.idle   - calc1.idle;
	const ULONGLONG kern = calc2.kernel - calc1.kernel;
	const ULONGLONG user = calc2.user   - calc1.user;
	const ULONGLONG syst = kern + user;
	return (static_cast<double>(syst) - idle) / syst;
}
//-----------------------------------------------
#if defined DEBUG_IGS_RESOURCE_CPU_RATE
# include <iostream>
# include "igs_resource_msg_from_err.cxx"
# include "igs_resource_sleep.cxx"
int main(int argc ,char *argv[]) {
  try {
	if (2 != argc) {
		std::cout << "Usage : " << argv[0] << " msec" << std::endl;
		return 1;
	}
	std::cout << "cpu_rate(win)="
		<< igs::resource::cpu_rate(atoi(argv[1]))
		<< std::endl;
  }
  catch (std::exception&e) {std::cerr<<"exception=\""<<e.what()<<"\"\n";}
  catch (...)              {std::cerr<<"other exception" << std::endl; }
	return 0;
}
#endif  /* !DEBUG_IGS_RESOURCE_CPU_RATE */
/*
rem vc2008sp1(32/64bits) :188,205 w! tes_msc_igs_rsc_cpu_rate.bat
set _DEB=DEBUG_IGS_RESOURCE_CPU_RATE
set _SRC=igs_resource_cpu_rate
set _TGT=tes_msc_%PROCESSOR_ARCHITECTURE%_igs_rsc_cpu_rate
set _INC=.
rem set _LIB=Pdh.lib
cl /W4 /WX /MD /EHa /O2 /wd4819 /D%_DEB% /I%_INC% %_SRC%.cxx /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _TGT=tes_msc_%PROCESSOR_ARCHITECTURE%_igs_rsc_cpu_ra_uc
cl /W4 /WX /MD /EHa /O2 /wd4819 /DUNICODE /D%_DEB% /I%_INC% %_SRC%.cxx /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
rem set _LIB=
set _INC=
set _TGT=
set _SRC=
set _DEB=
*/
