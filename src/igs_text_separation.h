#ifndef igs_text_separation_h
#define igs_text_separation_h

#include "igs_os_type.h"

namespace igs {
 namespace text {
	/* ʸ���Ԥ�ʬΥʸ�������(����) */
	class separation {
	public:
	separation();
		LPCTSTR date;	/* "-" */
		LPCTSTR time;	/* ":" */
		LPCTSTR word;	/* " " */
		LPCTSTR line;	/* "\r\n"(Windows),"\n"(linux)
				=binary��¸�Ѥβ��� */
	};
 }
}

#endif /* !igs_text_separation_h */
