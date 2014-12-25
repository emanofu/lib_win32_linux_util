#ifndef igs_resource_rmfile_h
#define igs_resource_rmfile_h

#include "igs_os_type.h"

#ifndef IGS_RESOURCE_IRF_EXPORT
# define IGS_RESOURCE_IRF_EXPORT
#endif

namespace igs {
 namespace resource {
	/*
		ファイル1個削除
		ファイルがない、削除できない、とき、エラー例外を投げる
	*/

	IGS_RESOURCE_IRF_EXPORT void rmfile(LPCTSTR file_path);
 }
}

#endif /* !igs_resource_rmfile_h */
