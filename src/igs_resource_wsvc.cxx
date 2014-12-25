#include <cstring> // wcslen(),strlen()
#include <iostream> // std::cout
#include <iomanip> // std::setw()
#include <sstream> // std::ostringstream
#include "igs_resource_msg_from_err.h"
#include "igs_resource_wsvc.h"

SC_HANDLE igs::resource::wsvc::open_scm_for_create(void) {
	/*
	�V���ɃT�[�r�X�𐶐����邽�߃T�[�r�X�R���g���[���}�l�[�W���[���J��
	Administrator�����̂���v���Z�X������
	CreateProcess�Ŏg�p����f�[�^�x�[�X�̃n���h�����J�����Ƃ��o����
	*/
	SC_HANDLE scm= ::OpenSCManager(NULL,NULL,SC_MANAGER_CREATE_SERVICE);
	if (NULL == scm) {
		throw std::domain_error(igs_resource_msg_from_err(
	TEXT("OpenSCManager(,,SC_MANAGER_CREATE_SERVICE)"),::GetLastError()
		));
	}
	return scm;
}
SC_HANDLE igs::resource::wsvc::create(
	SC_HANDLE scm
	,LPCTSTR service_name /* 256�����ȓ� */
	,LPCTSTR display_name /* 256�����ȓ� */
	,LPCTSTR binary_path_name
	,LPCTSTR dependencies
	,LPCTSTR service_start_name
	,LPCTSTR password
) {
	/*
	�V���ɃT�[�r�X�𐶐�����B
	CreateService �֐���1�̃T�[�r�X�I�u�W�F�N�g���쐬���A
	���̃��W�X�g���L�[�̉��ɃT�[�r�X���Ɠ������O�̃L�[��
	�쐬���邱�Ƃɂ��A
	�T�[�r�X����}�l�[�W���̃f�[�^�x�[�X����
	���̃I�u�W�F�N�g���C���X�g�[�����܂��B
		HKEY_LOCAL_MACHINE\System\CurrentControlSet\Services 
	���̊֐����g���Ďw�肵�����́A���̃L�[�̉��̒l�Ƃ��ĕۑ�����܂��B
	*/
	SC_HANDLE svc = ::CreateService(
		scm
		,service_name		/* �T�[�r�X�� */
		,display_name		/* �T�[�r�X�̕\���� */
		,SERVICE_ALL_ACCESS	/* �K�v�ȃA�N�Z�X�� */
		,SERVICE_WIN32_OWN_PROCESS	/* �T�[�r�X�̎�� */
		,SERVICE_AUTO_START	/* �J�n���@:boot���͎����J�n���� */
		,SERVICE_ERROR_NORMAL	/* �G���[�������̑Ώ��@ */
		,binary_path_name	/* �T�[�r�X�̃p�X�� */
		,NULL			/* ���[�h�����O���[�v�̎w�� */
		,NULL		/* ���[�h�����O���[�v���̃^�O���󂯎�� */
	/*
		dependencies�̐ݒ�ɂ���...
		--------------------------------------------------
		�T�[�r�X��(������w��)	����
		�\����		���s�t�@�C���̃p�X
		--------------------------------------------------
		lanmanserver		�t�@�C�����L�T�[�o��
		Server		C:\WINDOWS\system32\svchost.exe -k netsvcs
		lanmanworkstation	�t�@�C�����L�N���C�A���g��
		Workstation	C:\WINDOWS\system32\svchost.exe -k netsvcs
		--------------------------------------------------
		,"LanmanWorkstation"
			--> ����OK(2011-07-11)
		,"LanmanWorkstation\0LanmanServer"
	*/
		,dependencies	/* �ˑ�����T�[�r�X���̃��X�g */
		,service_start_name/* �A�J�E���g��ID(DomainName\UserName) */
		,password		/* �p�X���[�h */
	);
	if (NULL == svc) { /* �T�[�r�X�̐����ŃG���[ */
		throw std::domain_error(igs_resource_msg_from_err(
			TEXT("CreateService(-)"),::GetLastError()
		));
	}
	return svc;
}
/* ��������ݒ肷�� */
void igs::resource::wsvc::description(
	SC_HANDLE svc
	,LPCTSTR description_cp932
) {
	SERVICE_DESCRIPTION desc;
	desc.lpDescription = const_cast<LPTSTR>(description_cp932);
	if (0 == ::ChangeServiceConfig2(
		svc
		,SERVICE_CONFIG_DESCRIPTION 
		,&desc
	)) {
		throw std::domain_error(igs_resource_msg_from_err(
			TEXT("ChangeServiceConfig2(-)"),::GetLastError()
		));
	}
}
void igs::resource::wsvc::delayed_auto_start(
	SC_HANDLE svc
	,bool delayed
) {
	/* �����J�n�T�[�r�X��x��(��2����)������
		Windows Vista/Windows 2008�ȍ~ */
	SERVICE_DELAYED_AUTO_START_INFO info = { delayed };
	if (0 == ::ChangeServiceConfig2(
		svc
		,SERVICE_CONFIG_DELAYED_AUTO_START_INFO
		,&info
	)) {
		throw std::domain_error(igs_resource_msg_from_err(
			TEXT("ChangeServiceConfig2(-)"),::GetLastError()
		));
	}
}
void igs::resource::wsvc::start(SC_HANDLE svc) {
	/*
	�T�[�r�X���J�n����
	*/
	if (0 == ::StartService(svc ,0 ,NULL)) {
		throw std::domain_error(igs_resource_msg_from_err(
			TEXT("StartService(-)"),::GetLastError()
		));
	}
}
void igs::resource::wsvc::close(SC_HANDLE scm_or_svc) {
	/*
	�T�[�r�X�R���g���[���}�l�[�W���[�����A
	���邢�́A�T�[�r�X�����(�̂��������[�`���ōs��)
	*/
	if (0 == ::CloseServiceHandle(scm_or_svc)) {
		throw std::domain_error(igs_resource_msg_from_err(
			TEXT("CloseServiceHandle(-)"),::GetLastError()
		));
	}
}
//-------------------------------------------------------
SC_HANDLE igs::resource::wsvc::open_scm_for_access(void) {
	/*
	�T�[�r�X�̍폜�̂��߃T�[�r�X�R���g���[���}�l�[�W���[���J��
	*/
	SC_HANDLE scm = ::OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if (NULL == scm) {
		throw std::domain_error(igs_resource_msg_from_err(
	TEXT("OpenSCManager(,,SC_MANAGER_ALL_ACCESS)"),::GetLastError()
		));
	}
	return scm;
}
bool igs::resource::wsvc::exist_for_delete(
	SC_HANDLE scm ,LPCTSTR service_name
) {
	/*
	�w��̃T�[�r�X�����݂��邩�ǂ������ׂ�
	*/
	SC_HANDLE svc = ::OpenService(
		scm
		,service_name	/* �T�[�r�X�� */
		,DELETE		/* �A�N�Z�X�� */
	);
	if (NULL == svc) { /* �T�[�r�X���J���Ȃ� */
		DWORD err = ::GetLastError();
		if (ERROR_SERVICE_DOES_NOT_EXIST == err) {
			return false; /* �w��̃T�[�r�X�͑��݂��Ȃ� */
		}
		/* �G���[�Ȃ̂ŗ�O�𓊂��� */
		throw std::domain_error(igs_resource_msg_from_err(
			TEXT("OpenService(-)"),err
		));
	}
	/* �T�[�r�X�����݂���Ȃ���Ă��� */
	igs::resource::wsvc::close(svc);
	return true; /* �w��̃T�[�r�X�����݂��� */
}
SC_HANDLE igs::resource::wsvc::open(SC_HANDLE scm ,LPCTSTR service_name) {
	/*
	�����̃T�[�r�X�̃n���h�����J���܂�
	�w��̃T�[�r�X�����݂��Ȃ������G���[�Ƃ���
	*/
	SC_HANDLE svc = ::OpenService(
		scm
		,service_name	/* �T�[�r�X�� */

		/* QueryServicesStatus(-)�A�N�Z�X�� */
		,SERVICE_QUERY_STATUS

		/* ControlService(SERVICE_CONTROL_STOP)�A�N�Z�X�� */
		|SERVICE_STOP

		/* DeleteService()�A�N�Z�X�� */
		|DELETE

		/* StartService()�A�N�Z�X�� */
		|SERVICE_START
	);
	if (NULL == svc) {
		throw std::domain_error(igs_resource_msg_from_err(
			TEXT("OpenService(-)"),::GetLastError()
		));
	}
	return svc;
}

DWORD igs::resource::wsvc::status(SC_HANDLE svc) {
	/*
	�T�[�r�X�̏�Ԃ𓾂�
	���݂̃X�e�[�^�X��Ԃ�
	SERVICE_CONTINUE_PENDING	The service continue is pending.
	SERVICE_PAUSE_PENDING		The service pause is pending.
	SERVICE_PAUSED			The service is paused.
	SERVICE_RUNNING			The service is running.
	SERVICE_START_PENDING		The service is starting.
	SERVICE_STOP_PENDING		The service is stopping.
	SERVICE_STOPPED			The service is not running.

			STOPPED
			  ^  |
			  |  v
		+----------  -----------+
		^			|
		|			v
	STOP_PENDING		START_PENDING
		^			|
		|			v
		+----------  -----------+
			  ^  |
			  |  v
			RUNNING
			  ^  |
			  |  v
		+----------  -----------+
		^			|
		|			v
	CONTINUE_PENDING	PAUSE_PENDING
		^			|
		|			v
		+----------  -----------+
			  ^  |
			  |  v
			PAUSED
	*/
	SERVICE_STATUS service_status;
	::ZeroMemory(&service_status,sizeof(SERVICE_STATUS));
	if (0 == ::QueryServiceStatus(svc,&service_status)) {
		throw std::domain_error( igs_resource_msg_from_err(
			TEXT("QueryServiceStatus(-)"),::GetLastError()
		) );
	}
	return service_status.dwCurrentState;
}
void igs::resource::wsvc::send_stop(SC_HANDLE svc) {
	/*
	�T�[�r�X��~�v��(����R�[�h)���T�[�r�X�A�v���P�[�V�����֑��M���܂�
	*/
	SERVICE_STATUS service_status;
	::ZeroMemory(&service_status,sizeof(SERVICE_STATUS));
	if (0 == ::ControlService(
		svc /* �T�[�r�X�̃n���h�� */
		,SERVICE_CONTROL_STOP /* ����R�[�h *//* �T�[�r�X��~�v�� */
		,&service_status  /* �X�e�[�^�X��� */
	)) {
		throw std::domain_error(igs_resource_msg_from_err(
			TEXT("ControlService(-)"),::GetLastError()
		));
	}
}
void igs::resource::wsvc::mark_for_delete(SC_HANDLE svc) {
	/*
	�w�肵���T�[�r�X��
	scm�̃f�[�^�x�[�X����폜���邽�߂̃}�[�N��t���܂�
	*/
	if (0 == ::DeleteService(svc)) {
		throw std::domain_error(igs_resource_msg_from_err(
			TEXT("DeleteService(-)"),::GetLastError()
		));
	}
}
#if defined DEBUG_IGS_RESOURCE_WSVC
namespace {
 void dg_pr__(const std::string&msg) {
#if defined DEBUG_IGS_RESOURCE_WSVC
	std::cout << msg << std::endl;
#else
	msg; // for stopping warning
#endif  /* !DEBUG_IGS_RESOURCE_WSVC */
 }
}
#define dg_pr_(msg) dg_pr__(msg)
#else
#define dg_pr_(msg) ;
#endif  /* !DEBUG_IGS_RESOURCE_WSVC */
void igs::resource::wsvc::create_start(
	LPCTSTR service_name /* 256�����ȓ� */
	,LPCTSTR display_name /* 256�����ȓ� */
	,LPCTSTR binary_path_name
	,LPCTSTR dependencies
	,LPCTSTR service_start_name
	,LPCTSTR password
	,LPCTSTR description_cp932
) {
	dg_pr_("01.�V�T�[�r�X�𐶐����邽��Service Control Manager���J��");
	SC_HANDLE scm = igs::resource::wsvc::open_scm_for_create();
	dg_pr_("02.�w��̃T�[�r�X�����݂��邩�ǂ������ׂ�");
	if (!igs::resource::wsvc::exist_for_delete(scm,service_name)) {
	dg_pr_("03.�w��̃T�[�r�X���Ȃ��̂ŐV�T�[�r�X�𐶐�����");
		SC_HANDLE svc = igs::resource::wsvc::create( scm
			,service_name ,display_name ,binary_path_name
			,dependencies ,service_start_name ,password
		);
	dg_pr_("04.��������ݒ肷��");
		igs::resource::wsvc::description(svc,description_cp932);

	OSVERSIONINFO os_ver;
	os_ver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	::GetVersionEx( &os_ver );
	if (5 < os_ver.dwMajorVersion) { /* Windows Vista�Ȍ� */
	 dg_pr_("05.�����J�n�T�[�r�X��x���J�n�ɐݒ肷��");
		igs::resource::wsvc::delayed_auto_start( svc ,true );
	} else {			/* Windows 2003�ȑO(Xp��) */
	 dg_pr_("06.�����J�n�T�[�r�X��(OS���Â�����)�x���J�n���������̂܂�");
	}

	dg_pr_("07.�T�[�r�X���J�n����");
		igs::resource::wsvc::start(svc);
	dg_pr_("08.�T�[�r�X�����");
		igs::resource::wsvc::close(svc);
	} else {
	dg_pr_("09.�w��̃T�[�r�X�����ɂ���̂łȂɂ����Ȃ�");
	}
	dg_pr_("10.Service Control Manager�����");
	igs::resource::wsvc::close(scm);
}
void igs::resource::wsvc::start_only(
	LPCTSTR service_name /* 256�����ȓ� */
) {
	dg_pr_("01.Service Control Manager���J��");
	SC_HANDLE scm = igs::resource::wsvc::open_scm_for_create();
	dg_pr_("02.�w��̃T�[�r�X�����݂��邩�ǂ������ׂ�");
	if (igs::resource::wsvc::exist_for_delete(scm,service_name)) {
	dg_pr_("03.�T�[�r�X���J��");
		SC_HANDLE svc = igs::resource::wsvc::open(scm,service_name);
	dg_pr_("04.�T�[�r�X���J�n����");
		igs::resource::wsvc::start(svc);
	dg_pr_("05.�T�[�r�X�����");
		igs::resource::wsvc::close(svc);
	} else {
	dg_pr_("06.�w��̃T�[�r�X���Ȃ��̂ŃT�[�r�X�J�n�ł��Ȃ�");
	}
	dg_pr_("07.Service Control Manager�����");
	igs::resource::wsvc::close(scm);
}
void igs::resource::wsvc::stop_delete(
	LPCTSTR service_name /* 256�����ȓ� */
	,const int msec
	,const int time_out_count
	,const bool delete_sw
) {
	if (delete_sw) {
 dg_pr_("01.�T�[�r�X�폜�̂���Service Control Manager��Full Access�ŊJ��");
	} else {
 dg_pr_("02.Service Control Manager��Full Access�ŊJ��");
	}
	SC_HANDLE scm = igs::resource::wsvc::open_scm_for_access();

 dg_pr_("03.�w��̃T�[�r�X�����݂��邩�ǂ������ׂ�");
	if (igs::resource::wsvc::exist_for_delete(scm,service_name)) {

 dg_pr_("04.�w��̃T�[�r�X������̂ł��̃T�[�r�X���J��");
		SC_HANDLE svc = igs::resource::wsvc::open(scm,service_name);

/*
igs::resource::wsvc::status(svc);
igs::resource::wsvc::send_stop(svc);
��"user1"logon�ŃA�N�Z�X�����ۂ���Ă��܂�
��"user2"logon�ł��A�N�Z�X�����ۂ���Ă��܂�
LoginUser()��user2�ɂȂ�???
*/
 dg_pr_("05.�T�[�r�X�̏�Ԃ𓾂�");
		DWORD sta = igs::resource::wsvc::status(svc);
		/* STOP���ĂȂ����... */
		if (SERVICE_STOPPED != sta) {
			/* STOP�M������񑗂�... */
 dg_pr_("06.STOP���ĂȂ��̂ŃT�[�r�X�A�v���P�[�V�����֒�~�v���𑗐M����");
			igs::resource::wsvc::send_stop(svc);
		}
		/* STOP��ԂɂȂ�܂Ń��[�v�ő҂�... */
		for (int ii=0;;++ii) {
			if (time_out_count <= ii) {
	dg_pr_("07.STOP���Ȃ��̂ŁAtime out�Ŕ����� --> �G���[�ł�");
				break;
			}

			/* �Î~ */
			::Sleep(msec);

			/* �T�[�r�X�̏�Ԃ𓾂� */
			sta = igs::resource::wsvc::status(svc);

			/* STOP�����甲���� */
			std::ostringstream ost; ost << (ii+1)*msec/1000.;
			if (SERVICE_STOPPED == sta) {
				std::string str("08.STOP���� ");
				str += ost.str(); str += "sec";
	dg_pr_(str);
				break;
			}
			{
				std::string str("09.�܂�STOP���Ȃ�");
				str += ost.str(); str += "sec";
	dg_pr_(str);
			}
		}
		if (delete_sw) {
	dg_pr_("10.�w��̃T�[�r�X��scm�̃f�[�^�x�[�X����폜���邽�߂̃}�[�N��t���܂�");
		 igs::resource::wsvc::mark_for_delete(svc);
	dg_pr_("11.���ۂɍ폜���A�T�[�r�X�����");
		} else {
	dg_pr_("12.�T�[�r�X�����");
		}
		igs::resource::wsvc::close(svc);
	} else {
	dg_pr_("12.�w��̃T�[�r�X�͑��݂��Ȃ��̂łȂɂ����Ȃ�");
	}
	dg_pr_("13.Service Control Manager�����");
	igs::resource::wsvc::close(scm);
}
