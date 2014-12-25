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
		FILETIME file_time; /* 1601-1-1����100nsec�ֳ֤ο� */
		SYSTEMTIME system_time; /* ǯ,��,����,��,��,ʬ,��,�ߥ��� */
#else //--------------------------------------------------------------
		time_t epoch_time; /* 1970ǯ1��1��00:00:00 UTC ������� */
		struct tm local_time; /* ǯ,��,����,��,��,ʬ,��,�ߥ��� */
#endif //-------------------------------------------------------------
		int year,month,month_day,hour,minute,second;
		int week_day,year_day,milli_second;
	};
	/* ���֤����ꤹ�� */
	IGS_RESOURCE_LOG_EXPORT
	void ymdhms_to_param(
		const int year ,const int month ,const int month_day
		,const int hour ,const int minute ,const int second
		,igs::resource::time::parameters&time_pa
	);
	/* ���ߥ�������֤����� */
	IGS_RESOURCE_LOG_EXPORT
	void now_to_param( igs::resource::time::parameters&time_pa );
	/* �ե�����ι������֤�����
		unix�ե�����λ��֤�	--> Modify
		Windows�ե�����λ��֤�	--> Last Write Time : �ǽ���������
			Creation Time     : ��������
			Last Accesss Time : �ǽ�������������
			Last Write Time   : �ǽ���������
			���⡢Explorer��Ʊ��
				"Last Write Time : �ǽ���������"�����롣
	*/
	IGS_RESOURCE_LOG_EXPORT
	void file_timestamp_to_param(
		LPCTSTR file_path
		,igs::resource::time::parameters&time_pa
	);
	/* ɽ���Τ�����ȼ��񼰤����� */
	IGS_RESOURCE_LOG_EXPORT
	void form(
		const igs::resource::time::parameters&time_pa
		,std::basic_ostringstream<TCHAR>&ost
	);
	/* ɽ���Τ����OS��¸��(WIN32/unix)������ */
	IGS_RESOURCE_LOG_EXPORT
	void form_os(
		const igs::resource::time::parameters&time_pa
		,std::basic_ostringstream<TCHAR>&ost
	);
	/* ���ﺹ��sec������
		���κ����ͤϡ�
		2^31 = 2147483648
		60sec * 60minute * 24hour * 365day * 68year = 2144448000
		��ꡢ��68ǯ�ֺ��ޤ� */
	IGS_RESOURCE_LOG_EXPORT
	int second_diff(
		const igs::resource::time::parameters&time_pa1
		,const igs::resource::time::parameters&time_pa2
	);
	/*
	��ʬ����ꤷ�����߻������λ��������
	�㤨�С�10:00����
	10:01����ꤷ���顢������10:01
	09:59����ꤷ���顢��������09:59
	������
	*/
	IGS_RESOURCE_LOG_EXPORT
	void next_hour_minute(
		const igs::resource::time::parameters&time_now
		,const int next_hour
		,const int next_minute
		,igs::resource::time::parameters&time_next
	);
	/* ���߻��狼�����ñ�̤��ѹ� */
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
