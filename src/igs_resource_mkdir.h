#ifndef igs_resource_mkdir_h
#define igs_resource_mkdir_h

#include "igs_os_type.h"

#ifndef IGS_RESOURCE_LOG_EXPORT
# define IGS_RESOURCE_LOG_EXPORT
#endif

namespace igs {
 namespace resource {
	/* ディレクトリ1個作成し存在も確認して返る、孫は直接はつくれない */
	IGS_RESOURCE_LOG_EXPORT void mkdir_certain(
		LPCTSTR dir_path
		,DWORD wait_milli_seconds=10	/* 0.01秒 */
		,int wait_times=1000		/* 10秒 */
	);

	/* ディレクトリを掘り進む、孫以下も指定によりすべて作る */
	IGS_RESOURCE_LOG_EXPORT void mkdir_descendant(LPCTSTR dir_path);

	/* lockするためにディレクトリ1個作成(孫は直接はつくれない)
		Windowsでの戻り値は
			ERROR_SUCCESS		=0   作成した
			ERROR_ALREADY_EXISTS	=183 既に存在する
			ERROR_ACCESS_DENIED	=5   アクセス拒否
		unixでの戻り値は
			0	作成した
			EEXIST	既に存在している
		これ以外はエラーをthrowする
	*/
	IGS_RESOURCE_LOG_EXPORT DWORD mkdir_for_lock(LPCTSTR dir_path);
 }
}

#endif /* !igs_resource_mkdir_h */
