#ifndef igs_text_log_h
#define igs_text_log_h

#include <string> // std::basic_string<TCHAR>
#include "igs_os_type.h"

namespace igs {
 namespace text {
  namespace log {
   class parameters {
   public:
	parameters();
	LPCTSTR err_key;	// stderr
	LPCTSTR out_key;	// stdout
   };
   /*
	log_sw     == trueなら、log記録、
	dbgview_sw == trueなら、debug表示(Windowsのみ)、
	をする。
	両方falseの場合はなにもしない。
	--- Windows debug表示とは ---
	igs::text::log::append_to_file(-)内で
	::OutputDebugString(-)を実行している。
	--- Windows Service の debug の方法 ---
	microsoftのサイトからdownloadした"dbgview.exe"を、
	右クリックメニューの"管理者として実行..."を、
	実行して、情報を見る
   */
   /* logの書式を無視して1行記録する */
   void append_to_file(
	 const std::basic_string<TCHAR>&message_with_no_return/* 改行含ず */
	,const std::basic_string<TCHAR>&file_path
	,const bool&dbgview_sw = true	/* debug表示指示 */
	,const bool&log_sw = true	/* log記録実行指示 */
   );
   /* logの書式で1行記録する */
   void append_hms_key_msg_ret(
	 const std::basic_string<TCHAR>&category_word
	,const std::basic_string<TCHAR>&message_with_no_return/* 改行含ず */
	,const std::basic_string<TCHAR>&file_path
	,const bool&dbgview_sw = true	/* debug表示指示 */
	,const bool&log_sw = true	/* log記録実行指示 */
	,const bool&ymdhms_sw = false /* hmsでなくymdhmsを各行に記録指示
				critical error時"C:/temp/..."保存用 */
   );
  }
 }
}

#endif /* !igs_text_log_h */
