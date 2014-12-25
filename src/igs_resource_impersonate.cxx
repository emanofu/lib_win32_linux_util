#include "igs_resource_msg_from_err.h"
#include "igs_resource_impersonate.h"

igs::resource::impersonate::impersonate(
	const std::basic_string<TCHAR>&user_name
	,const std::basic_string<TCHAR>&password
)	:hToken_(0)
{
/*	LogonUser(-)
	���[�J���R���s���[�^�Ƀ��[�U�[�����O�I�������悤�Ƃ��܂��B
	�����[�g�R���s���[�^�ւ̃��O�I���͂ł��܂���B
	���[�U�[���ƃh���C�����g���ă��[�U�[���w�肵�A
	�����p�X���[�h���g���ă��[�U�[��F�؂��܂��B
	���̊֐�����������ƁA
	���Ƀ��O�I�����Ă��郆�[�U�[��\���g�[�N���̃n���h��
	���擾�ł��܂��B
	���ɁA���̃g�[�N���n���h�����g���āA
	�w�肵�����[�U�[���U�����邩�A
	�����̏ꍇ�͎w�肵�����[�U�[�̃R���e�L�X�g���œ��삷��
	�v���Z�X���쐬�ł��܂��B
*/
	if (0 == ::LogonUser( 
		user_name.c_str()		/* ���[�U�[���w�蕶����   */
		,NULL		      /* �h���C���܂��̓T�[�o�[�w�蕶���� */
		,password.c_str()		/* �p�X���[�h�w�蕶����   */
		,LOGON32_LOGON_INTERACTIVE	/* ���O�I������^�C�v�w�� */

		/*
		LOGON32_LOGON_BATCH
			1385 : ���O�I�����s: �v�����ꂽ��ނ̃��O�I���́A
			���̃R���s���[�^�[�ł̓��[�U�[�ɋ�����Ă��܂���B
		LOGON32_LOGON_SERVICE
			ImpersonateLoggedOnUser(-)�̐����̂��ƁA
			mkdir(-)�ŃG���[
 		*/

		,LOGON32_PROVIDER_DEFAULT	/* ���O�I���v���o�C�_�w�� */
		,&this->hToken_			/* �g�[�N���n���h������ */
	)) {
		throw std::domain_error(igs_resource_msg_from_err(
			TEXT("LogonUser(-)"),::GetLastError()
		));
	}
/*	ImpersonateLoggedOnUser(-)
	�Ăяo�����̃X���b�h���A
	���Ƀ��O�I�����Ă��郆�[�U�[�̃Z�L�����e�B�R���e�L�X�g
	���U���ł���悤�ɂ��܂��B
	���̃��[�U�[���g�[�N���n���h���ŕ\���܂��B
	(����1)
		���[�U�[�ݒ����ϐ��Ȃǂ́A
		���[�U�[�v���t�@�C���͈����p���Ȃ�
	(����2)
		Dialog��GUI�\�����s��Desktop�̏W�܂�ł���A
		�E�C���h�E�X�e�[�V�����͈����p���Ȃ�
*/
	if (0 == ::ImpersonateLoggedOnUser(
		/* ���Ƀ��O�I�����Ă��郆�[�U�[��\���g�[�N���̃n���h�� */
		this->hToken_
	)) {
		this->close_handle_();
		throw std::domain_error(igs_resource_msg_from_err(
			TEXT("ImpersonateLoggedOnUser(-)"),::GetLastError()
		));
	}
}
void igs::resource::impersonate::close_handle_(void) {
	/* �J���Ă���I�u�W�F�N�g�n���h������܂� */
	if (0 == ::CloseHandle( this->hToken_ )) {
		throw std::domain_error(igs_resource_msg_from_err(
			TEXT("CloseHandle(-)") ,::GetLastError()
		));
	}
}
void igs::resource::impersonate::close(void) {
	/* �N���C�A���g�A�v���P�[�V�����ɂ��U�����I�����܂� */
	if (0 == ::RevertToSelf()) {
		throw std::domain_error(igs_resource_msg_from_err(
			TEXT("RevertToSelf(-)") ,::GetLastError()
		));
	}
	this->close_handle_();
}
igs::resource::impersonate::~impersonate() {
	try {igs::resource::impersonate::close();} catch(...) {}
}

#if defined DEBUG_IGS_RESOURCE_IMPERSONATE
#include <iostream>
#include "igs_resource_msg_from_err.cxx"
#include "igs_resource_user_name.cxx"
namespace {
 std::basic_string<TCHAR> str;
 const std::basic_string<TCHAR>&user_name_(void) {
	igs::resource::user_name(str);
	return str;
 }
}
int main(int argc, char *argv[]) {
	argc;
	argv;

	std::basic_ostringstream<TCHAR> ost;
	ost << TEXT("user_name1=\"") << user_name_() << TEXT("\"\n");

	igs::resource::impersonate impe( TEXT("user1") ,TEXT("user1") );
	ost << TEXT("user_name2=\"") << user_name_() << TEXT("\"\n");
	impe.close();

	ost << TEXT("user_name3=\"") << user_name_() << TEXT("\"\n");

	std::TCOUT << ost.str(); 
	return 0;
}
#endif /* !DEBUG_IGS_RESOURCE_IMPERSONATE */
/*
rem vc2008sp1(32/64bits) :110,127 w! tes_msc_igs_rsc_impersonate.bat
set _DEB=DEBUG_IGS_RESOURCE_IMPERSONATE
set _SRC=igs_resource_impersonate
set _TGT=tes_msc_%PROCESSOR_ARCHITECTURE%_igs_rsc_impersonate_mb
set _INC=.
set _LIB=advapi32.lib
cl /W4 /WX /MD /EHa /O2 /wd4819 /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _TGT=tes_msc_%PROCESSOR_ARCHITECTURE%_igs_rsc_impersonate_wc
cl /W4 /WX /MD /EHa /O2 /wd4819 /DUNICODE /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _LIB=
set _INC=
set _TGT=
set _SRC=
set _DEB=
# gcc(32/64bits) :681,682 w! tes_gcc_igs_rsc_impersonate.csh
g++ -Wall -O2 -g -DDEBUG_IGS_RESOURCE_IMPERSONATE -I. igs_resource_impersonate.cxx -o tes_gcc_${HOSTTYPE}_igs_rsc_impersonate
*/
