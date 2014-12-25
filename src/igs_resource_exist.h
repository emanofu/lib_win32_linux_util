#ifndef igs_resource_exist_h
#define igs_resource_exist_h

#include "igs_os_type.h"

# ifndef IGS_RESOURCE_LOG_EXPORT
#  define IGS_RESOURCE_LOG_EXPORT
# endif

namespace igs {
 namespace resource {
  namespace exist {
/*
	dirあるいはfileの存在を確認する。
		戻り値は共通で、
			true	存在する
			false	存在しない
		エラーの場合は例外を投げる
*/
	/* fileあるいはdirの存在 */
	IGS_RESOURCE_LOG_EXPORT const bool file_or_directory(LPCTSTR path);

	/* fileの存在(dirに対してはfalseを返す) */
	IGS_RESOURCE_LOG_EXPORT const bool file(LPCTSTR path);

	/* dirの存在(fileに対してはfalseを返す) */
	IGS_RESOURCE_LOG_EXPORT const bool directory(LPCTSTR path);
  }
 }
}

#endif /* !igs_resource_exist_h */
