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
	Win32 : 現在のスレッドのユーザー名を取得します
	unix : 現在のプロセスの実ユーザーIDからとる
  */
  IGS_RESOURCE_LOG_EXPORT void user_name(std::basic_string<TCHAR>&name);
 }
}

#endif /* !igs_resource_user_name_h */
