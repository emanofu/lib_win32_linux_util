#include <fstream>	// std::basic_ofstream<TCHAR>
#include <sstream>	// std::ostringstream
#include <iostream>	// std::cout
#include <stdexcept>	// std::domain_error()
#include "igs_resource_exist.h"
#include "igs_resource_msg_from_err.h"
#include "igs_resource_mkdir.h"
#include "igs_resource_rmdir.h"
#include "igs_resource_sleep.h"
#include "igs_resource_lock.h"

igs::resource::lock::parameters::parameters(
	const std::basic_string<TCHAR>&dir_path_
	,DWORD wait_milli_seconds_
	,int wait_times_
	,const std::basic_string<TCHAR>&lock_by_file_path_
	,const std::basic_string<TCHAR>&host_name_
	,const std::basic_string<TCHAR>&user_name_
)
:dir_path(dir_path_)
,wait_milli_seconds(wait_milli_seconds_)
,wait_times(wait_times_)
,lock_by_file_path(lock_by_file_path_)
,host_name(host_name_)
,user_name(user_name_)
{}

namespace {
 void write_lock_by_(
	const std::basic_string<TCHAR>&path
	,const std::basic_string<TCHAR>&host_name
	,const std::basic_string<TCHAR>&user_name
 ) {
	std::basic_ofstream<TCHAR> ofs(
		//igs::resource::mbs_from_ts( path ).c_str()
		path.c_str()
	);
	if (!ofs) {
		std::basic_string<TCHAR> str;
		str += TEXT("\"");
		str += path;
		str += TEXT("\":can not ofs.open()");
		throw std::domain_error(igs_resource_msg_from_er(str));
	}
	ofs << TEXT("host\t") << host_name << TEXT('\n')
	    << TEXT("user\t") << user_name << TEXT('\n')
		;
	if (ofs.bad()) { /* 致命的なエラー */
		std::basic_string<TCHAR> str;
		str += path;
		str += TEXT(":ofs.bad() is not zero");
		throw std::domain_error(igs_resource_msg_from_er(str));
	}
	ofs.close();
 }
}
/* ディレクトリ作成によりlockを実行、lock出来ない場合は例外を投げる */
void igs::resource::lock::on(const igs::resource::lock::parameters&lock_pa) {
	bool not_mkdir_sw = true;
	int retry_mkdir = 0;
	int retry_exist = 0;
	DWORD mkdir_error_code = 0;
	for (int ii=0 ;ii<lock_pa.wait_times ;++ii) {
		if (not_mkdir_sw) {
			mkdir_error_code = igs::resource::mkdir_for_lock(
				lock_pa.dir_path.c_str()
			);
			if (0 == mkdir_error_code) { not_mkdir_sw = false; }
			else { ++retry_mkdir; }
		} else {
			if (igs::resource::exist::directory(
				lock_pa.dir_path.c_str()
			)) {
				/* lock情報保存 */
				if (!lock_pa.lock_by_file_path.empty()) {
					write_lock_by_(
						lock_pa.lock_by_file_path
						,lock_pa.host_name
						,lock_pa.user_name
					);
				}
#if defined DEBUG_IGS_RESOURCE_LOCK
	if (0 < retry_mkdir || 0 < retry_exist) {
	std::TCOUT << std::endl << TEXT("onononon");
	}
	std::TCOUT << TEXT(" on")
		<< TEXT('(') << retry_mkdir << TEXT("md")
		<< TEXT(' ') << retry_exist << TEXT("et")
		<< TEXT(' ') << mkdir_error_code << TEXT("mdec")
		<< TEXT(')')
		;
	if (0 < retry_mkdir || 0 < retry_exist) {
	std::TCOUT << TEXT("onononon") << std::endl;
	}
	std::TCOUT << std::flush;
#endif
				return;
			} else { ++retry_exist; }
		}
		igs::resource::sleep_m(lock_pa.wait_milli_seconds);
	}
	/* リカバリーとしてlockを外しておかないと、
	連鎖反応として他のserviceも落ちてしまう...のでやる!!! */
	igs::resource::rmdir( lock_pa.dir_path.c_str() );

	/* 回数(wait_times)繰り返して、それでもだめならTimeOut
	エラーとして例外を投げる */
	std::basic_ostringstream<TCHAR> ost;
	ost	<< TEXT("lock::on(-) mkdir_for_lock() but not mkdir")
		<< TEXT(" \"") << lock_pa.dir_path << TEXT('\"')
		<< TEXT(' ') << lock_pa.wait_times << TEXT("times/loop")
		<< TEXT(' ')<<lock_pa.wait_milli_seconds<<TEXT("msec/time")
		<< TEXT(' ') << retry_mkdir << TEXT("retry_mkdir (")
		<< TEXT(' ')
			<< TEXT("mkdir_last_error_code ")
			<< mkdir_error_code
			<< TEXT(")")
		<< TEXT(' ') << retry_exist << TEXT("retry_exist")
		;
	throw std::domain_error(igs_resource_msg_from_er(ost.str()));
}
void igs::resource::lock::off(
	const igs::resource::lock::parameters&lock_pa
) {
	if (false == igs::resource::exist::directory(
		lock_pa.dir_path.c_str()
	)) {
		/*
		ロックのリカバリをした場合、ロック(lock.dir)がない場合あり
		この関数を呼んでおきながら、外すべきロック(lock.dir)がない
		そのとき、
		連鎖反応で落ちてしまうのでこれはエラーとせず許容する
		*/
		return;
	}

	igs::resource::rmdir( lock_pa.dir_path.c_str() );

#if 0//-------------------------------------------------------------
	/*
	 * ここがあると、mkdirと存在チェックの間に他のlockがかかるかも
	 * しれないのでだめ
	 */
	int retry_exist = 0;
	for (;retry_exist<lock_pa.wait_times ;++retry_exist) {
		if (false == igs::resource::exist::directory(
			lock_pa.dir_path.c_str()
		)) {
#if defined DEBUG_IGS_RESOURCE_LOCK
if (0 < retry_exist) { std::TCOUT << std::endl << TEXT("offoffoffoff");}
	std::TCOUT << TEXT(" off")
		<< TEXT('(') << retry_exist << TEXT("et") << TEXT(')')
		;
if (0 < retry_exist) { std::TCOUT << TEXT("offoffoffoff") << std::endl;}
	std::TCOUT << std::flush;
#endif
			return; /* 正常終了 */
		}
		igs::resource::sleep_m(lock_pa.wait_milli_seconds);
	}
	/*
	lock外した(rmdir)後、存在しつづけたなら
	エラーとして例外を投げる
	*/
	std::basic_ostringstream<TCHAR> ost;
	ost	<< TEXT("lock::off(-) rmdir() but exist ")
		<< TEXT(" \"") << lock_pa.dir_path << TEXT('\"')
		<< TEXT(' ') << lock_pa.wait_times << TEXT("times/loop")
		<< TEXT(' ')<<lock_pa.wait_milli_seconds<<TEXT("msec/time")
		<< TEXT(' ') << retry_exist << TEXT("retry_exist")
		;
	throw std::domain_error(igs_resource_msg_from_er(ost.str()));
#endif//--------------------------------------------------------------
}
//--------------------------------------------------------------------
#if defined DEBUG_IGS_RESOURCE_LOCK
# include "igs_resource_exist.cxx" // shlwapi.lib
# include "igs_resource_msg_from_err.cxx"
# include "igs_resource_sleep.cxx"
# include "igs_resource_mkdir.cxx"
# include "igs_resource_rmdir.cxx"
# include "igs_resource_rmfile.cxx"
namespace {
 void pr_(const igs::resource::lock::parameters&lock_pa) {
	std::TCOUT
	<< TEXT("dir_path  =\"") << lock_pa.dir_path << TEXT("\"\n")
	<< TEXT("wait_msec =")   << lock_pa.wait_milli_seconds << std::endl
	<< TEXT("wait_times=")   << lock_pa.wait_times << std::endl
	<< TEXT("lock_by   =\"") << lock_pa.lock_by_file_path<< TEXT("\"\n")
	<< TEXT("host_name =\"") << lock_pa.host_name << TEXT("\"\n")
	<< TEXT("user_name =\"") << lock_pa.user_name << TEXT("\"\n")
	;
 }
 LPCTSTR msg =
	TEXT("cpu_rate                  0.0145823839593776\n")
	TEXT("is_login                  true\n")
	TEXT("commit_charge_total_bytes 3637747712\n")
	TEXT("commit_charge_limit_bytes 68563898368\n")
	TEXT("total_physical_bytes      34282897408\n")
	TEXT("number_of_processors      24\n")
	TEXT("compile_date_time         2012 8 16 11 2 54\n")
	TEXT("start_date_time           2012 9 1 14 54 19\n")
	;
 //const std::basic_string<TCHAR> 
 LPCTSTR file_path( TEXT("tmp.txt") );
 void wr_check_( const igs::resource::lock::parameters&lock_pa ) {
	int retry_exist = 0;
	for (;retry_exist < lock_pa.wait_times ;++retry_exist) {
		//if (igs::resource::exist::file_or_directory(
		if (igs::resource::exist::file(
			lock_pa.dir_path.c_str()
		)) {
#if defined DEBUG_IGS_RESOURCE_LOCK
if (0 < retry_exist) { std::TCOUT << std::endl << TEXT("wrwrwrwr");}
	std::TCOUT << TEXT(" wr")
		<< TEXT('(') << retry_exist << TEXT("et") << TEXT(')')
		;
if (0 < retry_exist) { std::TCOUT << TEXT("wrwrwrwr") << std::endl;}
	std::TCOUT << std::flush;
#endif
			return; /* 正常終了 */
		}
		igs::resource::sleep_m(lock_pa.wait_milli_seconds);
	}
	/*
	外して(rmdir)存在消えた確認回数(wait_times)繰り返して、
	それでも存在しないなら
	エラーとして例外を投げる */
	std::basic_ostringstream<TCHAR> ost;
	ost	<< TEXT("fd")
		<< TEXT(" \"") << lock_pa.dir_path << TEXT('\"')
		<< TEXT(' ') << lock_pa.wait_times << TEXT("times/loop")
		<< TEXT(' ')<<lock_pa.wait_milli_seconds<<TEXT("msec/time")
		<< TEXT(' ') << retry_exist << TEXT("retry_exist")
		;
	throw std::domain_error(igs_resource_msg_from_er(ost.str()));
 }
 void rmfile_check_(
	const igs::resource::lock::parameters&lock_pa
 ) {
	int retry_exist = 0;
	for (;retry_exist < lock_pa.wait_times ;++retry_exist) {
		//if (false == igs::resource::exist::file_or_directory(
		if (false == igs::resource::exist::file(
			lock_pa.dir_path.c_str()
		)) {
#if defined DEBUG_IGS_RESOURCE_LOCK
if (0 < retry_exist) { std::TCOUT << std::endl << TEXT("rmrmrmrm");}
	std::TCOUT << TEXT(" rm")
		<< TEXT('(') << retry_exist << TEXT("et") << TEXT(')')
		;
if (0 < retry_exist) { std::TCOUT << TEXT("rmrmrmrm") << std::endl;}
	std::TCOUT << std::flush;
#endif
			return; /* 正常終了 */
		}
		igs::resource::sleep_m(lock_pa.wait_milli_seconds);
	}
	/*
	外して(rmdir)存在消えた確認回数(wait_times)繰り返して、
	それでも存在しないなら
	エラーとして例外を投げる */
	std::basic_ostringstream<TCHAR> ost;
	ost	<< TEXT("nfd")
		<< TEXT(" \"") << lock_pa.dir_path << TEXT('\"')
		<< TEXT(' ') << lock_pa.wait_times << TEXT("times/loop")
		<< TEXT(' ')<<lock_pa.wait_milli_seconds<<TEXT("msec/time")
		<< TEXT(' ') << retry_exist << TEXT("retry_exist")
		;
	throw std::domain_error(igs_resource_msg_from_er(ost.str()));
 }
 void wr_( void ) {
	std::basic_ofstream<TCHAR> ofs(
		//igs::resource::mbs_from_ts(file_path).c_str()
		file_path
	);
	ofs << msg;
	ofs.close();
 }
 void re_(std::vector< std::basic_string<TCHAR> >&lines) {
	std::basic_ifstream<TCHAR> ifs(
		//igs::resource::mbs_from_ts(file_path).c_str()
		file_path
	);
	std::basic_string<TCHAR> line;
	while (std::getline(ifs,line)) { lines.push_back(line); }
	ifs.close();
 }
}
int main(int argc,char *argv[]) {
 try {
	igs::resource::locale_to_jp();
	std::basic_string<TCHAR> lock_dir(TEXT("lock.dir"));
	std::basic_string<TCHAR> lock_by( TEXT("lock_by.txt"));
	std::basic_string<TCHAR> hostname(TEXT("host1"));
	std::basic_string<TCHAR> username(TEXT("user1"));

	std::basic_string<TCHAR> lock_file_dir(TEXT("lock_file.dir"));
	std::basic_string<TCHAR> lock_file_by( TEXT("lock_file_by.txt"));

	igs::resource::lock::parameters lock_pa(
		lock_dir
		,10
		//,100
		/*,10	sleep()ゼロのとき、以下のエラーが起こる
2450 tmp.txt wexception=""igs_resource_lock.cxx:117:_MSC_VER:150030729:2012:Sep:15:11:54:42" "void __cdecl igs::resource::lock::on(const struct igs::resource::lock::parameters &)" "lock::on(-) lock "lock.dir" 1200times/loop 10msec/time 1200retry_mkdir 0retry_exist""
21147 tmp.txt r on(0md 0et)exception=""igs_resource_lock.cxx:159:_MSC_VER:150030729:2012:Sep:15:11:54:42" "void __cdecl igs::resource::lock::off(const struct igs::resource::lock::parameters &)" "lock::off(-) Not Exist  "lock.dir" 1200times/loop 10msec/time -1retry_exist""
		*/

		,1000
		,lock_by
		,hostname
		,username
	);
	igs::resource::lock::parameters lock_pa_file(
		lock_file_dir
		,10
		,1000
		,lock_file_by
		,hostname
		,username
	);
	if (2==argc && !strcmp("on",argv[1])) {
		pr_(lock_pa);
		igs::resource::lock::on(lock_pa);
	} else
	if (2==argc && !strcmp("off",argv[1])) {
		pr_(lock_pa);
		igs::resource::lock::off(lock_pa);
	} else
	if (2==argc && !strcmp("w",argv[1])) {
		pr_(lock_pa);
		for (unsigned ii=0 ;ii<0xffff; ++ii) {
			std::TCOUT << TEXT('\r') << ii
				<< TEXT(' ') << file_path;
			std::TCOUT << TEXT(" w");

			igs::resource::lock::on(lock_pa);
			wr_();
			//wr_check_( lock_pa_file );
			igs::resource::lock::off(lock_pa);

			std::TCOUT << TEXT(" rm");
			igs::resource::lock::on(lock_pa);
			if (igs::resource::exist::file( file_path )) {
				igs::resource::rmfile(file_path);
				//rmfile_check_( lock_pa_file );
/*
2012-09-13
rhel5とWindows7の両方でここを実行すると、Windows7側のみで、
ファイルは存在するのに、消そうとすると
"指定されたファイルが見つかりません。"
となる。原因不明

	なお、rhel5とWindows7と両方で実行すると、lockではなく
	ファイル削除で、Windows7側のみで、問題が起きる
	lockして、ファイル存在確認してから削除しているのに、
	rmfile()の例外エラー"ファイルがみつかりません"となる。
	lockのタイミングがrhel5とWindows7間でとれないのか?
	Windows7側のファイル存在確認してもrmfileできない状態なのか?
*/
			}
			igs::resource::lock::off(lock_pa);

			igs::resource::sleep_m(11); /* 必須 */
		}
	} else
	if (2==argc && !strcmp("r",argv[1])) {
		pr_(lock_pa);
		size_t sz2=0;
		for (unsigned ii=0 ;ii<0xffff; ++ii) {
			std::TCOUT << TEXT('\r') << ii
				<< TEXT(' ') << file_path;
			std::TCOUT << TEXT(" r");

			igs::resource::lock::on(lock_pa);
			if (igs::resource::exist::file(file_path)) {
		 		std::vector< std::basic_string<TCHAR> >
								lines;
				re_(lines);
				size_t sz1 = lines.size();
				if (0 == sz1 || sz1 != sz2) {
					std::TCOUT << TEXT("\n")
						<< ii << TEXT("time ")
						<< sz1 << TEXT("lines\n");
					sz2 = sz1;
				}
			}
			igs::resource::lock::off(lock_pa);

			igs::resource::sleep_m(11); /* 必須 */
		}
	} else {
		std::cout << "Usage : " << argv[0] << " on\n";
		std::cout << "Usage : " << argv[0] << " off\n";
		std::cout << "Usage : " << argv[0] << " w\n";
		std::cout << "Usage : " << argv[0] << " r\n";
		return 1;
	}
 }
 catch (std::exception& e) {
	std::cerr << "exception=\"" << e.what() << "\"\n";
 }
 catch (...) {
	std::cerr << "other exception\n";
 }
	return 0;
}
#endif  /* !DEBUG_IGS_RESOURCE_LOCK */
/*
rem vc2008sp1(32/64bits) :387,404 w! tes_msc_igs_resource_lock.bat
set _DEB=DEBUG_IGS_RESOURCE_LOCK
set _SRC=igs_resource_lock
set _TGT=tes_msc_%PROCESSOR_ARCHITECTURE%_igs_resource_lock_mb
set _INC=../src
set _LIB=wsock32.lib advapi32.lib shlwapi.lib
cl /W4 /WX /MD /EHa /O2 /wd4819 /D%_DEB% /DDEBUG_IRFMS /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _TGT=tes_msc_%PROCESSOR_ARCHITECTURE%_igs_resource_lock_wc
cl /W4 /WX /MD /EHa /O2 /wd4819 /DUNICODE /D%_DEB% /DDEBUG_IRFMS /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _LIB=
set _INC=
set _TGT=
set _SRC=
set _DEB=
# gcc(32/64bits) :259,261 w! tes_gcc_igs_resource_lock.csh
g++ -Wall -O2 -g -DDEBUG_IGS_RESOURCE_LOCK -I../src igs_resource_lock.cxx -o tes_gcc_${HOSTTYPE}_igs_resource_lock_mb
g++ -Wall -O2 -g -DUNICODE -DDEBUG_IGS_RESOURCE_LOCK -I../src igs_resource_lock.cxx -o tes_gcc_${HOSTTYPE}_igs_resource_lock_wc
*/
