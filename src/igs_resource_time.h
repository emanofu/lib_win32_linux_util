#ifndef igs_resource_time_h
#define igs_resource_time_h

#include "igs_os_type.h"
#include <sstream>	// std::basic_ostringstream<TCHAR>
#include <string>	// std::string

#ifndef IGS_RESOURCE_LOG_EXPORT
# define IGS_RESOURCE_LOG_EXPORT
#endif

namespace igs {
 namespace resource {
  namespace time {
	class IGS_RESOURCE_LOG_EXPORT parameters {
	public:
		parameters();
		void clear(void);
#if defined _WIN32 //-------------------------------------------------
		FILETIME file_time; /* 1601-1-1から100nsec間隔の数 */
		SYSTEMTIME system_time; /* 年,月,曜日,日,時,分,秒,ミリ秒 */
#else //--------------------------------------------------------------
		time_t epoch_time; /* 1970年1月1日00:00:00 UTC からの秒 */
		struct tm local_time; /* 年,月,曜日,日,時,分,秒,ミリ秒 */
#endif //-------------------------------------------------------------
		int year,month,month_day,hour,minute,second;
		int week_day,year_day,milli_second;
	};
	/* 時間を設定する */
	IGS_RESOURCE_LOG_EXPORT
	void ymdhms_to_param(
		const int year ,const int month ,const int month_day
		,const int hour ,const int minute ,const int second
		,igs::resource::time::parameters&time_pa
	);
	/* 現在ローカル時間を得る */
	IGS_RESOURCE_LOG_EXPORT
	void now_to_param( igs::resource::time::parameters&time_pa );
	/* ファイルの更新時間を得る
		unixファイルの時間は	--> Modify
		Windowsファイルの時間は	--> Last Write Time : 最終更新日時
			Creation Time     : 作成日時
			Last Accesss Time : 最終アクセス日時
			Last Write Time   : 最終更新日時
			の内、Explorerと同じ
				"Last Write Time : 最終更新日時"を得る。
	*/
	IGS_RESOURCE_LOG_EXPORT
	void file_timestamp_to_param(
		LPCTSTR file_path
		,igs::resource::time::parameters&time_pa
	);
	/* 表示のための独自書式を得る */
	IGS_RESOURCE_LOG_EXPORT
	void form(
		const igs::resource::time::parameters&time_pa
		,std::basic_ostringstream<TCHAR>&ost
	);
	/* 表示のためのOS依存書式(WIN32/unix)を得る */
	IGS_RESOURCE_LOG_EXPORT
	void form_os(
		const igs::resource::time::parameters&time_pa
		,std::basic_ostringstream<TCHAR>&ost
	);
	/* 時刻差をsecで得る
		差の最大値は、
		2^31 = 2147483648
		60sec * 60minute * 24hour * 365day * 68year = 2144448000
		より、約68年間差まで */
	IGS_RESOURCE_LOG_EXPORT
	int second_diff(
		const igs::resource::time::parameters&time_pa1
		,const igs::resource::time::parameters&time_pa2
	);
	/*
	時分を指定し、現在時刻より後の時刻を得る
	例えば、10:00現在
	10:01を指定したら、今日の10:01
	09:59を指定したら、次の日の09:59
	を得る
	*/
	IGS_RESOURCE_LOG_EXPORT
	void next_hour_minute(
		const igs::resource::time::parameters&time_now
		,const int next_hour
		,const int next_minute
		,igs::resource::time::parameters&time_next
	);
	/* 現在時刻からの秒単位の変更 */
	IGS_RESOURCE_LOG_EXPORT
	void shift_second(
		const igs::resource::time::parameters&time_now
		,const int second_for_shift
		,igs::resource::time::parameters&time_next
	);
	IGS_RESOURCE_LOG_EXPORT
	void compile_ymdhms(
		int&year ,int&month ,int&day
		,int&hour ,int&minute ,int&second
		,const std::string&complie_date=__DATE__
		,const std::string&complie_time=__TIME__
	);
  }
 }
}

#endif /* !igs_resource_time_h */
