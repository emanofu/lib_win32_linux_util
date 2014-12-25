#ifndef igs_resource_cpu_rate_h
#define igs_resource_cpu_rate_h

/* CPU�λ�����Ψ��0...1���ͤ��֤� */

#if defined _WIN32 //-------------------------------------------------
# include <windows.h>	// DWORD
# ifndef IGS_RESOURCE_IRF_EXPORT
#  define IGS_RESOURCE_IRF_EXPORT
# endif
namespace igs {
 namespace resource {
  /* 1second = 1,000milli_seconds(�ߥ���) */
  IGS_RESOURCE_IRF_EXPORT double cpu_rate( const DWORD milli_seconds=100 );
 }
}
#else //--------------------------------------------------------------
# include <time.h>	// time_t, ::nanosleep()
namespace igs {
 namespace resource {
  /*
	seconds(tv_sec)		--> ����ʾ����
	nano_seconds(tv_nsec)	--> 0�ʾ塢999,999,999�ʲ�
	����ʳ����ͤϥ��顼�Ȥʤ롣

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
