#ifndef igs_path_separation_h
#define igs_path_separation_h

#include "igs_os_type.h"

namespace igs {
 namespace path {
	/* �ѥ�ʸ����Υ��ѥ졼��ʸ�������(����) */
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
