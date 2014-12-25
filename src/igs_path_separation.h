#ifndef igs_path_separation_h
#define igs_path_separation_h

#include "igs_os_type.h"

namespace igs {
 namespace path {
	/* パス文字列のセパレータ文字の定義(固定) */
	class separation {
	public:
	separation();
		LPCTSTR path;	/* "/" */
		LPCTSTR file;	/* "." */
		LPCTSTR encl;	/* "\"" =enclosed */ 
		//LPCTSTR body;	/* "_" */
	};
 }
}

#endif /* !igs_path_separation_h */
