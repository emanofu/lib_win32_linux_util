#if defined _WIN32
# include <regex> /* vc2008sp1から使用可能??? */
using namespace std::tr1;/* namespace共通なったら関数直記述しusingは削除 */
#else // gcc
# include <boost/regex.hpp>
using namespace boost; /* namespace共通なったら関数直記述しusingは削除 */
#endif
#include "igs_resource_msg_from_err.h"
#include "igs_text_match_regex.h"
//--------------------------------------------------------------------
bool igs::text::match_regex(
	const std::basic_string<TCHAR>&str
	,const std::basic_string<TCHAR>&regexp
	,std::basic_string<TCHAR>&match
) {
	std::string str_(igs::resource::mbs_from_ts(str));
	std::string regexp_(igs::resource::mbs_from_ts(regexp));
#if defined _WIN32
	regex regex(regexp_.c_str());
	smatch result;
	if (regex_match(str_,result,regex) && (2 <= result.size())) {
		match = igs::resource::ts_from_mbs(result.str(1));
		return true;
	}
#else
	regex regex(regexp_);
	smatch result;
	if (regex_match(str_,result,regex) && (2 <= result.size())) {
		match = igs::resource::ts_from_mbs(result.str(1));
		return true;
	}
#endif
	match.clear();
	return false;
}
//--------------------------------------------------------------------
