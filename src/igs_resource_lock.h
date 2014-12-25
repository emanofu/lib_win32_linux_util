#ifndef igs_resource_lock_h
#define igs_resource_lock_h

#include <string>
#include "igs_os_type.h"

namespace igs {
 namespace resource {
  namespace lock {
   struct parameters {
   public:
	/* instance生成は引数必須
		引数は実態を持ったものを渡す、ゼロは渡せない
		引数の説明は以下各パラメータの説明を見よ
	*/
	parameters(
		const std::basic_string<TCHAR>&dir_path_
		,DWORD wait_milli_seconds_
		,int wait_times_
		,const std::basic_string<TCHAR>&lock_by_file_path_
		,const std::basic_string<TCHAR>&host_name_
		,const std::basic_string<TCHAR>&user_name_
	);

	/* ロック(中であることを表す)directoryへのパス
	*/
	const std::basic_string<TCHAR>&dir_path;

	/* ロックを掛けようとして待つ秒数と回数
		Timeout時は緊急措置としてロックを外し、エラー例外を投げる

		サービスの停止要求は、
			stop_msec_ = 15
		なので、
	2012-06-26
		15秒以内にする必要あり。
			10msec(0.01秒) x 1000times = 10秒
		でやっていた
	2012-09-12
		しかし、処理が重くなったとき
		TimeOutエラーが増えてしまうので、
			10msec(0.1秒) x 6000times = 60秒
		と長くして、サービスの停止要求時は強制終了にたよることにする
	*/
	DWORD wait_milli_seconds;	/* =  10(0.01秒) */
	int wait_times;			/* =6000(60秒) */

	/* ロックdirectoryの作成ホスト名とユーザ名を記憶するFileパス
	*/
	const std::basic_string<TCHAR>&lock_by_file_path;

	/* 記憶しておくホスト名とユーザ名
		エラーでデッドロックしたとき、
		どこで起きたかが分かる
	*/
	const std::basic_string<TCHAR>&host_name;
	const std::basic_string<TCHAR>&user_name;
#if defined _WIN32
	/* VC2008sp1 C++では代入演算子をオーバーロードする
		参照メンバあるいはconstメンバがあるので
		"代入演算子を生成できません"というwarningが出る
		これを打ち消すため
	*/
		//parameters& operator=(parameters& obj){return obj;}
		void operator=(const parameters& obj) {obj;}
#endif
   };
   /* ネットワークホスト間で情報共有するための共有アクセスロック機構
	ディレクトリ作成によりlockを実行、
	lockできない場合、指定時間sleepで指定回数retryし、
	それでもだめならエラーとして例外を投げる --> 致命的エラー
	注意：	rhel5とwin7間で実行すると、rhel5側で
"../src/igs_resource_exist_unix.cxx:14:__GNUC__:4.1.2-48:2012:Sep:15:10:00:34" "const bool<unnamed>::exist_file_or_directory_(const TCHAR*, stat&)" "lock.dir:Input/output error"
		のエラーが起きることあり。よって将来はこの方法は止めて、
		ネットワーク通信による共有の方法を確立すること。
   */
	void on( const igs::resource::lock::parameters&lock_pa );
	void off( const igs::resource::lock::parameters&lock_pa );
  }
 }
}

#endif /* !igs_resource_lock_h */
