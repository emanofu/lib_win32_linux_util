#include <sstream> // std::basic_ostringstream<TCHAR>
#include <stdexcept> /* std::domain_error */
#include "igs_resource_thread.h"
#include "igs_resource_msg_from_err.h"

const HANDLE igs::resource::thread_run(
	unsigned (__stdcall *function)(void *)
	, void *func_arg
	, const int priority
) {
	HANDLE thread_id=reinterpret_cast<HANDLE>(::_beginthreadex(
		NULL, 0, function, func_arg, 0, NULL
	));
	if (0 == thread_id) {
		std::basic_ostringstream<TCHAR> ost;
		ost	<< TEXT("_beginthreadex(-)")
			<< TEXT(":errno=") << errno
			<< TEXT(":_doserrno=") << _doserrno
			;
		throw std::domain_error(igs_resource_msg_from_er(
			ost.str()
		));
	}
	/*
	threadの優先順位をコントロールする
	1job/1cpuとして管理されているが
	2cpuマシンでは、、、
		vc6mt では、
			THREAD_PRIORITY_BELOW_NORMALとすると、
			2つのcpuをmaxminプロセスが90%、
			他方プロセスが10%の割合で使う
			(maxminが9(8+1)thread他方が1thread)
			priorityを下げると他processと50%対50%になる
			thread2,4,8,16で同じとなる。
			理由は不明だが、結果OK。
		vc2005md では、
			THREAD_PRIORITY_BELOW_NORMALとすると、
			thread4以上で30%程度にしかならない
	*/
	/*
	vc2005 MSDN より
	BOOL SetThreadPriority(int nPriority);
	に与える値を優先度の高いものから順に並べます。
	THREAD_PRIORITY_TIME_CRITICAL		プロセスにより15 or 31
	THREAD_PRIORITY_HIGHEST			標準より2ポイント高い
	THREAD_PRIORITY_ABOVE_NORMAL		標準より1ポイント高い
	THREAD_PRIORITY_NORMAL		-1	標準
	THREAD_PRIORITY_BELOW_NORMAL		標準より1ポイント低い
	THREAD_PRIORITY_LOWEST			標準より2ポイント低い
	THREAD_PRIORITY_IDLE			プロセスにより1 or 16
	*/
	if (0 == ::SetThreadPriority(thread_id,priority)) {
		throw std::domain_error(igs_resource_msg_from_err(
			TEXT("SetThreadPriority(-)"),::GetLastError()
		));
	}
	return thread_id;
}

/*
以下の関数でthreadの終了を見るよりも、
thread内で終了時に終了スイッチを入れる方法で感知する??
linuxでのやり方にあわせる?? 2011-03-30
*/
const bool igs::resource::thread_was_done(const HANDLE thread_id) {
	DWORD exit_code=0;
	if (0 == ::GetExitCodeThread(thread_id, &exit_code)) {
		throw std::domain_error(igs_resource_msg_from_err(
			TEXT("GetExitCodeThread(-)"),::GetLastError()
		));
	}
	if (exit_code == STILL_ACTIVE) { return false; }
	return true;
}
void igs::resource::thread_join(const HANDLE thread_id) {
	thread_wait(thread_id);
	thread_close(thread_id);
}
void igs::resource::thread_wait(const HANDLE thread_id) {
	/* _endthreadex(-)はスレッドハンドルを閉じない??? */
	if (WAIT_FAILED == ::WaitForSingleObject( thread_id, INFINITE )) {
		throw std::domain_error(igs_resource_msg_from_err(
			TEXT("WaitForSingleObject(-)"),::GetLastError()
		));
	}
}
void igs::resource::thread_close(const HANDLE thread_id) {
	if (0 == ::CloseHandle( thread_id )) {
		throw std::domain_error(igs_resource_msg_from_err(
			TEXT("CloseHandle(-)"),::GetLastError()
		));
	}
}
#if defined DEBUG_IGS_RESOURCE_THREAD
#include <iostream>
#include <vector>
#include "igs_resource_msg_from_err.cxx"
#include "igs_resource_sleep.h"
#include "igs_resource_sleep.cxx"
namespace {
  class params {
  public:
	params(const int msec);
	void execute(void);
	void run(void);
	const bool was_done(void);
	void join(void);
	void close(void);

	int milli_second;
  private:
	HANDLE thread_id_;
  };
  unsigned __stdcall thread_function(void *arg) {
	static_cast<params *>(arg)->execute();
	return 0;
  }
  params::params(const int msec)
	:milli_second(msec)
  {}
  void params::run(void) {
	this->thread_id_ = igs::resource::thread_run(thread_function,this);
  }
  const bool params::was_done(void) {
	if (0 == this->thread_id_) { return true; }
	return igs::resource::thread_was_done( this->thread_id_ );
  }
  void params::close(void) {
	if (0 != this->thread_id_) {
		igs::resource::thread_close( this->thread_id_ );
		this->thread_id_ = 0;
	}
  }
  void params::execute(void) {
	std::cout << "thread begin<" << this->milli_second << ">"
							<< std::endl;
	igs::resource::sleep_m(this->milli_second); /* ミリ秒で指定 */
	std::cout << "thread end<" << this->milli_second << ">"
							<< std::endl;
  }
  void params::join(void) {
	igs::resource::thread_join( this->thread_id_ );
  }
}
int main(void)
{
  {
	std::cout << "--- thread using for speed up ---" << std::endl;
	params	rend_th1(1 * 1000)
		,rend_th2(2 * 1000)
		,rend_th3(3 * 1000)
		;
	rend_th1.run();
	igs::resource::sleep_m(100);	// 0.1sec
	rend_th2.run();
	igs::resource::sleep_m(100);	// 0.1sec
	rend_th3.run();

	rend_th1.join(); std::cout << "after join1" << std::endl;
	rend_th2.join(); std::cout << "after join2" << std::endl;
	rend_th3.join(); std::cout << "after join3" << std::endl;
  }
  {
	std::cout << "--- thread using for jobs control ---" << std::endl;
	params	rend_th1(1 * 1000)
		,rend_th2(2 * 1000)
		,rend_th3(3 * 1000)
		;
	bool	sw1 = true
		,sw2 = true
		,sw3 = true
		;
	rend_th1.run();
	igs::resource::sleep_m(100);	// 0.1sec
	rend_th2.run();
	igs::resource::sleep_m(100);	// 0.1sec
	rend_th3.run();
	bool loop_sw = true;
	while (loop_sw) {
		if (sw1 && rend_th1.was_done()) {
			rend_th1.close(); sw1 = false;
			std::cout << "rend_th1 was done" << std::endl;
		}
		if (sw2 && rend_th2.was_done()) {
			rend_th2.close(); sw2 = false;
			std::cout << "rend_th2 was done" << std::endl;
		}
		if (sw3 && rend_th3.was_done()) {
			rend_th3.close(); sw3 = false;
			std::cout << "rend_th3 was done" << std::endl;
		}
		if (!sw1 && !sw2 && !sw3) { break; }
		igs::resource::sleep_m(1000);
	}
	std::cout << "all done" << std::endl;
  }
	return 0;
}
#endif  /* !DEBUG_IGS_RESOURCE_THREAD */
/*
rem vc2008sp1(32/64bits) :201,218 w! tes_msc_igs_rsc_thread.bat
set _DEB=DEBUG_IGS_RESOURCE_THREAD
set _SRC=igs_resource_thread
set _TGT=tes_msc_%PROCESSOR_ARCHITECTURE%_igs_rsc_thread
set _INC=.
set _LIB=
cl /W4 /WX /MD /EHa /O2 /wd4819 /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _TGT=tes_msc_%PROCESSOR_ARCHITECTURE%_igs_rsc_thread_uc
cl /W4 /WX /MD /EHa /O2 /wd4819 /DUNICODE /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _LIB=
set _INC=
set _TGT=
set _SRC=
set _DEB=
*/
