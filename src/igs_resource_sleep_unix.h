#ifndef igs_resource_sleep_h
#define igs_resource_sleep_h

#include "igs_os_type.h" // DWORD

#ifndef IGS_RESOURCE_IFX_EXPORT
# define IGS_RESOURCE_IFX_EXPORT
#endif

namespace igs {
 namespace resource {
	/* 1second = 1,000milli_seconds(�ߥ���) */
	IGS_RESOURCE_IFX_EXPORT void sleep_m(const DWORD milli_seconds);
 }
}

#if !defined _WIN32
#include <ctime> // time_t, ::nanosleep()
namespace igs {
 namespace resource {
	/* unix�ǤϤ����餬�ᥤ��,sleep_m()��������sleep_sn()��Ƥ�Ǥ���
1second=1,000milli_seconds=1,000,000micro_seconds=1,000,000,000nano_seconds
tv_sec(seconds)���餫��tv_nsec(nano_seconds)��0��999,999,999�ϰϳ����ϥ��顼
	*/
	void sleep_sn(const time_t seconds, const long nano_seconds);
 }
}
#endif   /* !_WIN32 */

#endif  /* !igs_resource_sleep_h */
