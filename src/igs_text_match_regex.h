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
	1.マッチしたらtrueを返し、マッチしなければfalseを返す
	2.文字列(str)から正規表現(regexp)にマッチする部分を取出す(match)
  */
  IGS_RESOURCE_IRF_EXPORT bool match_regex(
	const std::basic_string<TCHAR>&str
	,const std::basic_string<TCHAR>&regexp
	,std::basic_string<TCHAR>&match
  );
 }
}

#endif /* !igs_text_match_regex_h */
