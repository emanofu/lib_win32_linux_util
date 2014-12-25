#ifndef igs_resource_kill_process_h
#define igs_resource_kill_process_h

#if defined _WIN32 //-------------------------------------------------
# include <windows.h> // DWORD
# ifndef IGS_RESOURCE_IRF_EXPORT
#  define IGS_RESOURCE_IRF_EXPORT
# endif
namespace igs {
 namespace resource {
  /*
	-------------------------------+----------------------------
	post_close_to_process(-)	WM_CLOSE
	post_destroy_to_process(-)	WM_DESTROY(*1)
	post_quit_to_process(-)		WM_QUIT(*2)
	terminate_process(-)		Terminate
	-------------------------------+----------------------------
	(*1) [x]ボタン押しwindow破棄するとき送られてくるメッセージ
	(*2) アプリケーションを終了するメッセージ
		MSDN(vs2008)より
		PostMessage 関数を使って WM_QUIT メッセージを
		ポストすることは避けてください。
		代わりに、PostQuitMessage 関数を使ってください
		--> 他のアプリを終了させる事には関係ない...らしい...



	<> Windowsコマンドでプロセスを終了させるコマンドについて
	"tskill "			--> 子供は終わらない
	"taskkill /pid pid /t"		--> ...終了しない...
	"taskkill /f /pid pid /t"	--> 親子両方強制終了
	"taskkill /f /pid pid"		--> 強制終了
  */

  /*
	messageを送り、指定のキューをセットし、アプリを終了させる、
	終了Dialog開く、等々
	アプリケーションの終了を待たずにかえってくることに注意

	キューをセットする順番
		プロセスtreeの親から、
		windows treeの後ろから前へ
			(=子?windowから親へ)
			(=オーナー付windowからオーナーwindowへ)
  */
  IGS_RESOURCE_IRF_EXPORT void post_close_to_process(const DWORD pid);
  IGS_RESOURCE_IRF_EXPORT void post_destroy_to_process(const DWORD pid);
  IGS_RESOURCE_IRF_EXPORT void post_quit_to_process(const DWORD pid);

  /*
	プロセスを無条件に終了させます。
	プロセスtreeを、親から、TerminateProcessする

	TerminateProcess()関数を使用している。
		vc2008 MSDNより
		"特別な状況でのみこの関数を使ってください。
		ExitProcess 関数ではなく TerminateProcess 関数を使うと、
		DLL(ダイナミックリンクライブラリ)が管理している
		グローバルデータは、整合性のない状態に陥る可能性があります。
		この関数は、指定したプロセスに所属するすべてのスレッドを
		終了させ、そのプロセスも終了させます。
		しかし、終了処理が進行中であることを、
		そのプロセスにアタッチしている DLL へ通知しません"
	--> DLLで使用しているメモリの開放や値のリセットをしない、、、
  */
  IGS_RESOURCE_IRF_EXPORT void terminate_process(const DWORD pid);
 }
}
#else //--------------------------------------------------------------
#include <signal.h>	// pid_t
namespace igs {
 namespace resource {
  /*
	killpg_sig...() & kill_...() --- プロセスツリーを終了させる
	使い方
	1. sigterm まず終了(termination)シグナルを送る
	2, sigint  SIGTERMで始末できない場合は、
			キーボードからの割り込み(Interrupt)シグナルを送る
	3. sigkill SIGTERM,SIGINTで始末できない場合(ゾンビプロセス)は、
			これ(kill)を送る。ただし、プロセスは、跡始末できない
  */

  /*
	killpg_sig...()
	動作
		指定したシグナルを指定したプロセスグループに送る
		killpg()を使い子プロセスまで終了する
	bug?
		システムモニターを同時に表示していると閉じてしまう.原因不明
  */
  void killpg_sigterm(const pid_t pid);
  void killpg_sigint(const pid_t pid);
  void killpg_sigkill(const pid_t pid);

  /*
	kill_sig...()
	親のプロセスから子供の順に終了させる
	子プロセスは一旦initの子プロセスとなり次にその子プロセスを終了させる

	基本的にkillpg_sig...()を使うこと。
	killpg_sig...()で全て済むことが明確になったら、
	kill_sig...()は削除する。
  */
  void kill_sigterm(const pid_t pid);
  void kill_sigint(const pid_t pid);
  void kill_sigkill(const pid_t pid);
 }
}
#endif //-------------------------------------------------------------

#endif /* !igs_resource_kill_process_h */
