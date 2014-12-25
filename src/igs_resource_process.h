#ifndef igs_resource_process_h
#define igs_resource_process_h

#if defined _WIN32 //-------------------------------------------------
# include <windows.h>
# include <vector>
# include <string>
# ifndef IGS_RESOURCE_IRF_EXPORT
#  define IGS_RESOURCE_IRF_EXPORT
# endif
namespace igs {
  namespace resource {
    namespace process {
      struct parameters {
		parameters(); /* ����˽���� */
		~parameters(); /* ���ϥ�ɥ���Ĥ��� */
		const std::string close( void );
		HANDLE hStdRead, hStdWrite;
		HANDLE hErrRead, hErrWrite;
		PROCESS_INFORMATION pi;
      };

      /* �ѥ��פ��äƥץ�����¹Ԥ��롣
	 ���顼��λ���Ƥ�ץ����Ϥۤä��餫�� */
      void execute(
		igs::resource::process::parameters&pa
		,LPTSTR command_line
		,DWORD add_priority_flag=
			//NORMAL_PRIORITY_CLASS
			BELOW_NORMAL_PRIORITY_CLASS
			//0
			//IDLE_PRIORITY_CLASS
      );

      void form(
		const igs::resource::process::parameters&pa
		,std::vector< std::basic_string<TCHAR> >&lines
      );

      /* �����ʥ���֤Ǥʤ�(��λ���Ƥʤ�)�����뤤�ϡ��ɤ߹��ߤ���ʤ�true */
	/*
	���ץꥱ������󤬽Ф���å������ϡ�
	MBCS�Ǥ���餷����
	���Τ���UNICODE����compile���Ƥ⡢
	�ѥ��פ���������stdout,stderr��å������ϡ�
	MBCS�Ǽ������(std::string)�褦�ˤ��Ƥ��롣
	�ʤΤǡ����ץꥱ�������UNICODE16��å�������Ф��Ƥ���ȡ�
	����˼������ʤ����⤷��ʤ���
	������б�����ˤϼ�ư��MBCS/UNICODE16���Τ���ɬ�פ����뤬��
	������ˡ�Ϥ狼��ʤ��Τ���α����Ȥ��ޤ���
	*/
      bool wait( const igs::resource::process::parameters&pa
		, const int time_out_msec
		, std::basic_string<TCHAR>&std_str
		, std::basic_string<TCHAR>&err_str
      );

      /* �ץ�����λ������Ȥ��Τ���Υץ���ID */
      DWORD id( const igs::resource::process::parameters&pa );

      /* �ץ�������λ�����Ȥ�������� */
      DWORD exit_code( const igs::resource::process::parameters&pa );

      /* ���ϥ�ɥ���Ĥ��� */
      void close( igs::resource::process::parameters&pa );
    }
  }
}
#else //--------------------------------------------------------------
# include <sys/types.h>
# include <string>
# include <vector>
namespace igs {
  namespace resource {
    namespace process {
      struct parameters {
		parameters(); /* ����˽���� */
		~parameters(); /* ���ϥ�ɥ���Ĥ��� */
		const std::string close( void );
		int fdout[2];
		int fderr[2];
		int fdend[2];
		pid_t cpid;
		int exit_code;
		char end_char; /* ��λ���Τ餻�򡢵����Τ��ᤳ�����֤� */
      };
      void execute(
		igs::resource::process::parameters&pa
		,int argc
		,char *argv[]
      );

      void form(
		const igs::resource::process::parameters&pa
		,std::vector< std::string >&lines
      );

      bool wait( igs::resource::process::parameters&pa
		,const int sleep_sec
		,const int sleep_nsec /* 999,999,999�ʲ� */
		,std::string&out_str
		,std::string&err_str
      );
      pid_t id( const igs::resource::process::parameters&pa );
      int exit_code( const igs::resource::process::parameters&pa );
      void close( igs::resource::process::parameters&pa );
    }
  }
}
#endif //-------------------------------------------------------------

#endif /* !igs_resource_process_h */
