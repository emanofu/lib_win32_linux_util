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
	(*1) [x]�ܥ��󲡤�window�˴�����Ȥ������Ƥ����å�����
	(*2) ���ץꥱ��������λ�����å�����
		MSDN(vs2008)���
		PostMessage �ؿ���Ȥä� WM_QUIT ��å�������
		�ݥ��Ȥ��뤳�Ȥ��򤱤Ƥ���������
		����ˡ�PostQuitMessage �ؿ���ȤäƤ�������
		--> ¾�Υ��ץ��λ��������ˤϴط��ʤ�...�餷��...



	<> Windows���ޥ�ɤǥץ�����λ�����륳�ޥ�ɤˤĤ���
	"tskill "			--> �Ҷ��Ͻ����ʤ�
	"taskkill /pid pid /t"		--> ...��λ���ʤ�...
	"taskkill /f /pid pid /t"	--> �ƻ�ξ��������λ
	"taskkill /f /pid pid"		--> ������λ
  */

  /*
	message�����ꡢ����Υ��塼�򥻥åȤ������ץ��λ�����롢
	��λDialog����������
	���ץꥱ�������ν�λ���Ԥ����ˤ����äƤ��뤳�Ȥ����

	���塼�򥻥åȤ������
		�ץ���tree�οƤ��顢
		windows tree�θ��������
			(=��?window����Ƥ�)
			(=�����ʡ���window���饪���ʡ�window��)
  */
  IGS_RESOURCE_IRF_EXPORT void post_close_to_process(const DWORD pid);
  IGS_RESOURCE_IRF_EXPORT void post_destroy_to_process(const DWORD pid);
  IGS_RESOURCE_IRF_EXPORT void post_quit_to_process(const DWORD pid);

  /*
	�ץ�����̵���˽�λ�����ޤ���
	�ץ���tree�򡢿Ƥ��顢TerminateProcess����

	TerminateProcess()�ؿ�����Ѥ��Ƥ��롣
		vc2008 MSDN���
		"���̤ʾ����ǤΤߤ��δؿ���ȤäƤ���������
		ExitProcess �ؿ��ǤϤʤ� TerminateProcess �ؿ���Ȥ��ȡ�
		DLL(�����ʥߥå���󥯥饤�֥��)���������Ƥ���
		�����Х�ǡ����ϡ��������Τʤ����֤˴٤��ǽ��������ޤ���
		���δؿ��ϡ����ꤷ���ץ����˽�°���뤹�٤ƤΥ���åɤ�
		��λ���������Υץ����⽪λ�����ޤ���
		����������λ�������ʹ���Ǥ��뤳�Ȥ�
		���Υץ����˥����å����Ƥ��� DLL �����Τ��ޤ���"
	--> DLL�ǻ��Ѥ��Ƥ������γ������ͤΥꥻ�åȤ򤷤ʤ�������
  */
  IGS_RESOURCE_IRF_EXPORT void terminate_process(const DWORD pid);
 }
}
#else //--------------------------------------------------------------
#include <signal.h>	// pid_t
namespace igs {
 namespace resource {
  /*
	killpg_sig...() & kill_...() --- �ץ����ĥ꡼��λ������
	�Ȥ���
	1. sigterm �ޤ���λ(termination)�����ʥ������
	2, sigint  SIGTERM�ǻ����Ǥ��ʤ����ϡ�
			�����ܡ��ɤ���γ�����(Interrupt)�����ʥ������
	3. sigkill SIGTERM,SIGINT�ǻ����Ǥ��ʤ����(����ӥץ���)�ϡ�
			����(kill)�����롣���������ץ����ϡ��׻����Ǥ��ʤ�
  */

  /*
	killpg_sig...()
	ư��
		���ꤷ�������ʥ����ꤷ���ץ������롼�פ�����
		killpg()��Ȥ��ҥץ����ޤǽ�λ����
	bug?
		�����ƥ��˥�����Ʊ����ɽ�����Ƥ�����Ĥ��Ƥ��ޤ�.��������
  */
  void killpg_sigterm(const pid_t pid);
  void killpg_sigint(const pid_t pid);
  void killpg_sigkill(const pid_t pid);

  /*
	kill_sig...()
	�ƤΥץ�������Ҷ��ν�˽�λ������
	�ҥץ����ϰ�öinit�λҥץ����Ȥʤ꼡�ˤ��λҥץ�����λ������

	����Ū��killpg_sig...()��Ȥ����ȡ�
	killpg_sig...()�����ƺѤळ�Ȥ����Τˤʤä��顢
	kill_sig...()�Ϻ�����롣
  */
  void kill_sigterm(const pid_t pid);
  void kill_sigint(const pid_t pid);
  void kill_sigkill(const pid_t pid);
 }
}
#endif //-------------------------------------------------------------

#endif /* !igs_resource_kill_process_h */
