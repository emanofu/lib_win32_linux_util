#ifndef igs_resource_rmdir_h
#define igs_resource_rmdir_h

#include "igs_os_type.h"

#ifndef IGS_RESOURCE_IRF_EXPORT
# define IGS_RESOURCE_IRF_EXPORT
#endif

namespace igs {
 namespace resource {
	/*
		ディレクトリ1個削除
			directoryがない
			removeできない
			空でないディレクトリが消せない
		とき、エラー例外を投げる
	*/
	IGS_RESOURCE_IRF_EXPORT void rmdir(LPCTSTR dir_path);
 }
}

#endif /* !igs_resource_rmdir_h */
