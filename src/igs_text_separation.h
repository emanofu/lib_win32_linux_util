#ifndef igs_text_separation_h
#define igs_text_separation_h

#include "igs_os_type.h"

namespace igs {
 namespace text {
	/* 文字行の分離文字の定義(固定) */
	class separation {
	public:
	separation();
		LPCTSTR date;	/* "-" */
		LPCTSTR time;	/* ":" */
		LPCTSTR word;	/* " " */
		LPCTSTR line;	/* "\r\n"(Windows),"\n"(linux)
				=binary保存用の改行 */
	};
 }
}

#endif /* !igs_text_separation_h */
