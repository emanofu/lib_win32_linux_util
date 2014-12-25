#ifndef igs_text_scntoc_h
#define igs_text_scntoc_h

#include "igs_os_type.h"

/*
	Softimageの".scntoc"ファイルから
	開始フレームと終了フレーム番号を得る
	redners.exeのみで使用する
*/
namespace igs {
 namespace text {
  void scntoc_start_end(
	const std::basic_string<TCHAR>&file_path // Extention is .scntoc
	,int&start_frame
	,int&end_frame
  );
 }
}
#endif /* !igs_text_scntoc_h */
