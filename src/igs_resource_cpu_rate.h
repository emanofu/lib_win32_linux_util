#ifndef igs_resource_cpu_rate_h
#define igs_resource_cpu_rate_h

/* CPUの使用比率を0...1の値で返す */

#if defined _WIN32 //-------------------------------------------------
# include <windows.h>	// DWORD
# ifndef IGS_RESOURCE_IRF_EXPORT
#  define IGS_RESOURCE_IRF_EXPORT
# endif
namespace igs {
 namespace resource {
  /* 1second = 1,000milli_seconds(ミリ秒) */
  IGS_RESOURCE_IRF_EXPORT double cpu_rate( const DWORD milli_seconds=100 );
 }
}
#else //--------------------------------------------------------------
# include <time.h>	// time_t, ::nanosleep()
namespace igs {
 namespace resource {
  /*
	seconds(tv_sec)		--> ゼロ以上の値
	nano_seconds(tv_nsec)	--> 0以上、999,999,999以下
	それ以外の値はエラーとなる。

			1 second
	=           1,000 milli seconds
	=       1,000,000 micro seconds
	=   1,000,000,000 nano seconds
  */
  double cpu_rate( const time_t seconds ,const long nano_seconds );
 }
}
#endif //-------------------------------------------------------------

#endif /* !igs_resource_cpu_rate_h */
