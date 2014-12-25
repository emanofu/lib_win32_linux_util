#ifndef igs_resource_append_text_by_atom_h
#define igs_resource_append_text_by_atom_h

#include "igs_os_type.h"
#include <string>

#ifndef IGS_RESOURCE_LOG_EXPORT
# define IGS_RESOURCE_LOG_EXPORT
#endif

namespace igs {
 namespace resource {
	/*
		�ե�����˥ƥ����Ȥ��Բ�ʬ���ɲä���
		�Բ�ʬ =atomically =ʬ��Ǥ��ʤ�

		��������unix��win�κ��߼¹ԤϤ��ᡣ
		win���ɲäǤ��ʤ����Ȥ����롢����������
	*/
	IGS_RESOURCE_LOG_EXPORT void append_text_by_atom(
		const std::basic_string<TCHAR>&file_path
		,const std::string&str
#if defined _WIN32
		,const DWORD sleep_milliseconds=3//=3...7 about log at once
		,const DWORD time_out_milli_seconds=1000//=1sec
#endif
	);
 }
}

#endif /* !igs_resource_append_text_by_atom_h */
