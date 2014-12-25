#ifndef igs_resource_lock_h
#define igs_resource_lock_h

#include <string>
#include "igs_os_type.h"

namespace igs {
 namespace resource {
  namespace lock {
   struct parameters {
   public:
	/* instance�����͈����K�{
		�����͎��Ԃ����������̂�n���A�[���͓n���Ȃ�
		�����̐����͈ȉ��e�p�����[�^�̐���������
	*/
	parameters(
		const std::basic_string<TCHAR>&dir_path_
		,DWORD wait_milli_seconds_
		,int wait_times_
		,const std::basic_string<TCHAR>&lock_by_file_path_
		,const std::basic_string<TCHAR>&host_name_
		,const std::basic_string<TCHAR>&user_name_
	);

	/* ���b�N(���ł��邱�Ƃ�\��)directory�ւ̃p�X
	*/
	const std::basic_string<TCHAR>&dir_path;

	/* ���b�N���|���悤�Ƃ��đ҂b���Ɖ�
		Timeout���ً͋}�[�u�Ƃ��ă��b�N���O���A�G���[��O�𓊂���

		�T�[�r�X�̒�~�v���́A
			stop_msec_ = 15
		�Ȃ̂ŁA
	2012-06-26
		15�b�ȓ��ɂ���K�v����B
			10msec(0.01�b) x 1000times = 10�b
		�ł���Ă���
	2012-09-12
		�������A�������d���Ȃ����Ƃ�
		TimeOut�G���[�������Ă��܂��̂ŁA
			10msec(0.1�b) x 6000times = 60�b
		�ƒ������āA�T�[�r�X�̒�~�v�����͋����I���ɂ���邱�Ƃɂ���
	*/
	DWORD wait_milli_seconds;	/* =  10(0.01�b) */
	int wait_times;			/* =6000(60�b) */

	/* ���b�Ndirectory�̍쐬�z�X�g���ƃ��[�U�����L������File�p�X
	*/
	const std::basic_string<TCHAR>&lock_by_file_path;

	/* �L�����Ă����z�X�g���ƃ��[�U��
		�G���[�Ńf�b�h���b�N�����Ƃ��A
		�ǂ��ŋN��������������
	*/
	const std::basic_string<TCHAR>&host_name;
	const std::basic_string<TCHAR>&user_name;
#if defined _WIN32
	/* VC2008sp1 C++�ł͑�����Z�q���I�[�o�[���[�h����
		�Q�ƃ����o���邢��const�����o������̂�
		"������Z�q�𐶐��ł��܂���"�Ƃ���warning���o��
		�����ł���������
	*/
		//parameters& operator=(parameters& obj){return obj;}
		void operator=(const parameters& obj) {obj;}
#endif
   };
   /* �l�b�g���[�N�z�X�g�Ԃŏ�񋤗L���邽�߂̋��L�A�N�Z�X���b�N�@�\
	�f�B���N�g���쐬�ɂ��lock�����s�A
	lock�ł��Ȃ��ꍇ�A�w�莞��sleep�Ŏw���retry���A
	����ł����߂Ȃ�G���[�Ƃ��ė�O�𓊂��� --> �v���I�G���[
	���ӁF	rhel5��win7�ԂŎ��s����ƁArhel5����
"../src/igs_resource_exist_unix.cxx:14:__GNUC__:4.1.2-48:2012:Sep:15:10:00:34" "const bool<unnamed>::exist_file_or_directory_(const TCHAR*, stat&)" "lock.dir:Input/output error"
		�̃G���[���N���邱�Ƃ���B����ď����͂��̕��@�͎~�߂āA
		�l�b�g���[�N�ʐM�ɂ�鋤�L�̕��@���m�����邱�ƁB
   */
	void on( const igs::resource::lock::parameters&lock_pa );
	void off( const igs::resource::lock::parameters&lock_pa );
  }
 }
}

#endif /* !igs_resource_lock_h */
