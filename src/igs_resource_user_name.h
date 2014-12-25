#ifndef igs_resource_user_name_h
#define igs_resource_user_name_h

#include <string>
#include "igs_os_type.h"

# ifndef IGS_RESOURCE_LOG_EXPORT
#  define IGS_RESOURCE_LOG_EXPORT
# endif

namespace igs {
 namespace resource {
  /*
	Win32 : ���ߤΥ���åɤΥ桼����̾��������ޤ�
	unix : ���ߤΥץ����μ¥桼����ID����Ȥ�
  */
  IGS_RESOURCE_LOG_EXPORT void user_name(std::basic_string<TCHAR>&name);
 }
}

#endif /* !igs_resource_user_name_h */
