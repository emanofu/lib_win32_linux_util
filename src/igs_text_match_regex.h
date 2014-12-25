#ifndef igs_text_match_regex_h
#define igs_text_match_regex_h

#include <string>
#include "igs_os_type.h"

#ifndef IGS_RESOURCE_IRF_EXPORT
# define IGS_RESOURCE_IRF_EXPORT
#endif
namespace igs {
 namespace text {
  /*
	1.�ޥå�������true���֤����ޥå����ʤ����false���֤�
	2.ʸ����(str)��������ɽ��(regexp)�˥ޥå�������ʬ���Ф�(match)
  */
  IGS_RESOURCE_IRF_EXPORT bool match_regex(
	const std::basic_string<TCHAR>&str
	,const std::basic_string<TCHAR>&regexp
	,std::basic_string<TCHAR>&match
  );
 }
}

#endif /* !igs_text_match_regex_h */
