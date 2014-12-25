#ifndef igs_resource_host_name_h
#define igs_resource_host_name_h

#include <string>
#include "igs_os_type.h" // TCHAR

#ifndef IGS_RESOURCE_LOG_EXPORT
# define IGS_RESOURCE_LOG_EXPORT
#endif

namespace igs {
 namespace resource {
  /*
	win32 : WinSock���鼫�ۥ���̾��Ĵ�٤�
	unix : ���ۥ���̾��Ĵ�٤�
  */
  IGS_RESOURCE_LOG_EXPORT void host_name(std::basic_string<TCHAR>&name);
 }
}

#endif /* !igs_resource_host_name_h */
