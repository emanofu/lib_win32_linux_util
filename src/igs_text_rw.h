#ifndef igs_text_rw_h
#define igs_text_rw_h

#include <vector>	// std::vector
#include <string>	// std::basic_string<TCHAR>
#include "igs_os_type.h"

#ifndef IGS_RESOURCE_IRF_EXPORT
# define IGS_RESOURCE_IRF_EXPORT
#endif

/*
	irfm�����ƥ��profile
		(RENDERS_DATABASE_TOP)\records\profile\...
	�ؤ�Read/Write�򤹤�
	�ʤ���irfm�����ƥ��log
		(RENDERS_DATABASE_TOP)\records\log\...
	�ˤĤ��Ƥϡ�igs::resource::append_text_by_atom(-)��
*/
namespace igs {
  namespace text {
    IGS_RESOURCE_IRF_EXPORT void read_lines(
	LPCTSTR file_path
	,std::vector< std::basic_string<TCHAR> >&lines
    );
    IGS_RESOURCE_IRF_EXPORT void read_line(
	LPCTSTR file_path
	,std::basic_string<TCHAR>&line
    );
    IGS_RESOURCE_IRF_EXPORT void append_line( /* ���Ԥ�����ɲ���¸ */
	const std::basic_string<TCHAR>&line_with_no_return_code
	,LPCTSTR file_path
    );
    IGS_RESOURCE_IRF_EXPORT void write_line( /* ���Ԥʤ�����¸ */
	const std::basic_string<TCHAR>&line
	,LPCTSTR file_path
    );
    IGS_RESOURCE_IRF_EXPORT void write_lines(
	const std::vector< std::basic_string<TCHAR> >&lines
	,LPCTSTR file_path
    );
  }
}
#endif /* !igs_text_rw_h */
