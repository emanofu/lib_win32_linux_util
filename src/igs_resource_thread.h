#ifndef igs_resource_thread_h
#define igs_resource_thread_h

#if defined _WIN32 //-------------------------------------------------
# include <windows.h>	// HANDLE
# include <process.h>	// _beginthreadex()
# ifndef IGS_RESOURCE_IFX_EXPORT
#  define IGS_RESOURCE_IFX_EXPORT
# endif
namespace igs {
 namespace resource {
//HANDLE = unsigned long(vc6.0) = void *(vc2005)
  IGS_RESOURCE_IFX_EXPORT const HANDLE thread_run(
	unsigned (__stdcall *function)(void *)
	, void *func_arg
	, const int priority = THREAD_PRIORITY_NORMAL
	/*
	priority��Ϳ�����ͤ�ͥ���٤ι⤤��Τ������¤٤ޤ���
	THREAD_PRIORITY_TIME_CRITICAL		�ץ����ˤ��15 or 31
	THREAD_PRIORITY_HIGHEST			ɸ����2�ݥ���ȹ⤤
	THREAD_PRIORITY_ABOVE_NORMAL		ɸ����1�ݥ���ȹ⤤
	THREAD_PRIORITY_NORMAL			ɸ��
	THREAD_PRIORITY_BELOW_NORMAL		ɸ����1�ݥ�����㤤
	THREAD_PRIORITY_LOWEST			ɸ����2�ݥ�����㤤
	THREAD_PRIORITY_IDLE			�ץ����ˤ��1 or 16
	*/
  );
  IGS_RESOURCE_IFX_EXPORT const bool thread_was_done(const HANDLE thread_id);
  IGS_RESOURCE_IFX_EXPORT void thread_join(const HANDLE thread_id);
  IGS_RESOURCE_IFX_EXPORT void thread_wait(const HANDLE thread_id);
  IGS_RESOURCE_IFX_EXPORT void thread_close(const HANDLE thread_id);
 }
}
#else //--------------------------------------------------------------
# include <pthread.h>	// pthread_t,pthread_create(),pthread_join()
namespace igs {
 namespace resource {
//pthread_t = unsigned long int(rhel4)
  pthread_t thread_run(
	void *(*function)(void *)
	, void *func_arg
	, const int state = PTHREAD_CREATE_JOINABLE
	/*
	state ��
	PTHREAD_CREATE_JOINABLE �ʤ顢pthread_join()��Ƥ�ǽ�λ���Ԥ�
	PTHREAD_CREATE_DETACHED �ʤ顢�ʤˤ�Ƥ�ɬ�פ��ʤ�����
		thread��λ���Τ�ˤϼ����ǻųݤ���ɬ�ס�
	*/
  );
  void thread_join(const pthread_t thread_id);
 }
}
#endif //-------------------------------------------------------------

#endif  /* !igs_resource_thread_h */
