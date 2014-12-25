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
		parameters(); /* ゼロに初期化 */
		~parameters(); /* 全ハンドルを閉じる */
		const std::string close( void );
		HANDLE hStdRead, hStdWrite;
		HANDLE hErrRead, hErrWrite;
		PROCESS_INFORMATION pi;
      };

      /* パイプを作ってプロセスを実行する。
	 エラー終了してもプロセスはほったらかし */
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

      /* シグナル状態でなく(終了してない)、あるいは、読み込みありならtrue */
	/*
	アプリケーションが出すメッセージは、
	MBCSであるらしい。
	そのためUNICODE指定compileしても、
	パイプから受け取るstdout,stderrメッセージは、
	MBCSで受け取る(std::string)ようにしてある。
	なので、アプリケーションがUNICODE16メッセージを出していると、
	正常に受け取れないかもしれない。
	これに対応するには自動でMBCS/UNICODE16を感知する必要があるが、
	その方法はわからないので保留問題とします。
	*/
      bool wait( const igs::resource::process::parameters&pa
		, const int time_out_msec
		, std::basic_string<TCHAR>&std_str
		, std::basic_string<TCHAR>&err_str
      );

      /* プロセスを終了させるときのためのプロセスID */
      DWORD id( const igs::resource::process::parameters&pa );

      /* プロセスが終了したときの戻り値 */
      DWORD exit_code( const igs::resource::process::parameters&pa );

      /* 全ハンドルを閉じる */
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
		parameters(); /* ゼロに初期化 */
		~parameters(); /* 全ハンドルを閉じる */
		const std::string close( void );
		int fdout[2];
		int fderr[2];
		int fdend[2];
		pid_t cpid;
		int exit_code;
		char end_char; /* 終了の知らせを、記憶のためここに置く */
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
		,const int sleep_nsec /* 999,999,999以下 */
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
