#include <sys/types.h>	// struct stat
#include <sys/stat.h>	// struct stat
#include <unistd.h>	// struct stat
#include <time.h>	// ::time(-)
#include <vector>	// std::vector
#include <cerrno>	// errno

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
	this->epoch_time = static_cast<time_t>(-1);
	::memset( &this->local_time ,0 ,sizeof(struct tm) );
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

	this->epoch_time = static_cast<time_t>(-1);
	::memset( &this->local_time ,0 ,sizeof(struct tm) );
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
	time_pa.week_day	= -1; /* 未使用 */
	time_pa.year_day	= -1; /* 未使用 */
	time_pa.milli_second	= -1; /* 未使用 */

	time_pa.local_time.tm_year	= time_pa.year - 1900;
	time_pa.local_time.tm_mon	= time_pa.month - 1;
	time_pa.local_time.tm_mday	= time_pa.month_day;
	time_pa.local_time.tm_hour	= time_pa.hour;
	time_pa.local_time.tm_min	= time_pa.minute;
	time_pa.local_time.tm_sec	= time_pa.second;
	time_pa.local_time.tm_wday	= 0; /* 仮に0(日曜日)を入れておく */
	time_pa.local_time.tm_yday	= 0; /* 仮に0を入れておく */

	time_pa.epoch_time = ::mktime(&time_pa.local_time);
}
void igs::resource::time::now_to_param(
	igs::resource::time::parameters&time_pa
) {
	time_pa.epoch_time = ::time(NULL);
	/* time(-)の引数がアクセス可能なアドレス空間の外を指している
	とエラーだが、NULLを渡しているのでエラーにならないはず */
	if (((time_t)-1) == time_pa.epoch_time) {
		throw std::domain_error(igs_resource_msg_from_err(
			TEXT("::time(NULL)"),errno
		));
	}
	/* エラーの場合は NULLを返す */
	if (NULL==::localtime_r(&time_pa.epoch_time,&time_pa.local_time)) {
		throw std::domain_error(igs_resource_msg_from_err(
			TEXT("::localtime_r(-)"),errno
		));
	}
	time_pa.year		= time_pa.local_time.tm_year + 1900;
	time_pa.month		= time_pa.local_time.tm_mon + 1;
	time_pa.month_day	= time_pa.local_time.tm_mday;
	time_pa.hour		= time_pa.local_time.tm_hour;
	time_pa.minute		= time_pa.local_time.tm_min;
	time_pa.second		= time_pa.local_time.tm_sec;
	time_pa.week_day	= time_pa.local_time.tm_wday;
	time_pa.year_day	= time_pa.local_time.tm_yday;
	time_pa.milli_second	= -1; /* 未使用 */
}
void igs::resource::time::file_timestamp_to_param(
	LPCTSTR file_path
	,igs::resource::time::parameters&time_pa
) {
	struct stat t_stat;
	if (0 != ::stat(
		igs::resource::mbs_from_ts(file_path).c_str() ,&t_stat
	)) { /* not exists */
		throw std::domain_error(igs_resource_msg_from_err(
			file_path,errno
		));
	}
	time_pa.epoch_time = t_stat.st_mtime; // modify time
	if (NULL == ::localtime_r(&t_stat.st_mtime,&time_pa.local_time)) {
		throw std::domain_error(igs_resource_msg_from_err(
			file_path,errno
		));
	}
	time_pa.year		= time_pa.local_time.tm_year + 1900;
	time_pa.month		= time_pa.local_time.tm_mon + 1;
	time_pa.month_day	= time_pa.local_time.tm_mday;
	time_pa.hour		= time_pa.local_time.tm_hour;
	time_pa.minute		= time_pa.local_time.tm_min;
	time_pa.second		= time_pa.local_time.tm_sec;
	time_pa.week_day	= time_pa.local_time.tm_wday;
	time_pa.year_day	= time_pa.local_time.tm_yday;
	time_pa.milli_second	= -1; /* 未使用 */
}
void igs::resource::time::form_os(
	const igs::resource::time::parameters&time_pa
	,std::basic_ostringstream<TCHAR>&ost
) {
	/* 時刻書式をunix apiで得る */
	char local_date_time[255];
	::ctime_r( &time_pa.epoch_time, local_date_time );
	/* 改行を削除 */
	std::basic_string<TCHAR> str(
		igs::resource::ts_from_mbs(local_date_time)
	);
	std::basic_string<TCHAR>::size_type index=str.find_first_of(
		TEXT("\n\r")
	);
	if (std::basic_string<TCHAR>::npos != index) { str.erase(index); }
	/* 保存 */
	ost << str;
}
namespace {
  /* 時刻差をsecで得る */
  void diff_( const time_t sec1 ,const time_t sec2 ,double&sec) {
	sec = ::difftime(sec2,sec1);
  }
}
int igs::resource::time::second_diff(
	const igs::resource::time::parameters&time_pa1
	,const igs::resource::time::parameters&time_pa2
) {
	double sec=0;
	diff_(
		time_pa1.epoch_time ,time_pa2.epoch_time ,sec
	);
	return static_cast<int>(sec);
}
/*
時分を指定し、現在時刻より後の時刻を得る
例えば、10:00現在
10:01を指定したら、今日の10:01
09:59を指定したら、次の日の09:59
を得る
*/
void igs::resource::time::next_hour_minute(
	const igs::resource::time::parameters&time_now
	,const int next_hour
	,const int next_minute
	,igs::resource::time::parameters&time_next
) {
	/* 現在との時間差を分で得る */
	int minute_for_shift =
		(next_hour - time_now.hour) * 60
		+ (next_minute - time_now.minute)
		;

	/* 時間差がマイナスの場合、現在時刻より後の時刻に変換する */
	if (minute_for_shift < 0) {
		/* 10:00現在09:59を指定したら、次の日の09:59 */
		minute_for_shift += 24 * 60;
	}

	igs::resource::time::shift_second(
		time_now ,minute_for_shift*60/* 分を秒に */ ,time_next
	);
}
/* 現在時刻からの秒単位の変更 */
void igs::resource::time::shift_second(
	const igs::resource::time::parameters&time_now
	,const int second_for_shift
	,igs::resource::time::parameters&time_next
) {
	/* 時間差を加え、構造体の年月日時分秒の値をセットする */
	time_next.epoch_time = time_now.epoch_time + second_for_shift;
	if (NULL==::localtime_r(
		&time_next.epoch_time
		,&time_next.local_time
	)) {
		throw std::domain_error(igs_resource_msg_from_err(
			TEXT("localtime_r(-)"),errno
		));
	}
	time_next.year		= time_next.local_time.tm_year + 1900;
	time_next.month		= time_next.local_time.tm_mon + 1;
	time_next.month_day	= time_next.local_time.tm_mday;
	time_next.hour		= time_next.local_time.tm_hour;
	time_next.minute	= time_next.local_time.tm_min;
	time_next.second	= time_next.local_time.tm_sec;
	time_next.week_day	= time_next.local_time.tm_wday;
	time_next.year_day	= time_next.local_time.tm_yday;
	time_next.milli_second	= -1; /* 未使用 */
}

/* 表示のための独自書式を得る */
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
}
void igs::resource::time::compile_ymdhms(
	int&year ,int&month ,int&day
	,int&hour ,int&minute ,int&second
	,const std::string&complie_date
	,const std::string&complie_time
) {
 { /*------ 年月日 ------*/
	std::istringstream ist(complie_date);
	std::string month_str;
	ist >> month_str >> day >> year;

	/* __DATE__が表現する月の名前のリスト */
	const std::string month_names(
		"JanFebMarAprMayJunJulAugSepOctNovDec"
	);
	/* 開始位置 */
	std::string::size_type index1 = month_names.find( month_names );
	/* 現在月 */
	std::string::size_type index2 = month_names.find( month_str );
	/* 現在月が確定できないエラー */
	if (std::string::npos == index2) {
		// std::basic_string<TCHAR> msg(igs::resource::ts_from_mbs( ist.str()));
		// throw std::domain_error(igs_resource_msg_from_er(msg));
		return;
	}
	month = static_cast<int>(index2-index1)/3+1;
 }

 { /*------ 時分秒 ------*/
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
	/* localeを日本に設定し日本語を扱うことを指示(しないと日本語文
	字部分のみ処理しない)。ただし数値カテゴリはC localeのままに(し
	ないと3桁ごとにカンマが付く(1000-->1,000)) */
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
# gcc(32/64bits) :408,410 w! tes_gcc_igs_rsc_time.csh
g++ -Wall -O2 -g -DDEBUG_IGS_RESOURCE_TIME -I. igs_resource_time.cxx -o tes_gcc_${HOSTTYPE}_igs_rsc_time_mb
g++ -Wall -O2 -g -DDEBUG_IGS_RESOURCE_TIME -DUNICODE -I. igs_resource_time.cxx -o tes_gcc_${HOSTTYPE}_igs_rsc_time_wc
*/
