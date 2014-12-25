#include <cstring> // wcslen(),strlen()
#include <iostream> // std::cout
#include <iomanip> // std::setw()
#include <sstream> // std::ostringstream
#include "igs_resource_msg_from_err.h"
#include "igs_resource_wsvc.h"

SC_HANDLE igs::resource::wsvc::open_scm_for_create(void) {
	/*
	新たにサービスを生成するためサービスコントロールマネージャーを開く
	Administrator特権のあるプロセスだけが
	CreateProcessで使用するデータベースのハンドルを開くことが出来る
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
	,LPCTSTR service_name /* 256文字以内 */
	,LPCTSTR display_name /* 256文字以内 */
	,LPCTSTR binary_path_name
	,LPCTSTR dependencies
	,LPCTSTR service_start_name
	,LPCTSTR password
) {
	/*
	新たにサービスを生成する。
	CreateService 関数は1つのサービスオブジェクトを作成し、
	次のレジストリキーの下にサービス名と同じ名前のキーを
	作成することにより、
	サービス制御マネージャのデータベース内に
	そのオブジェクトをインストールします。
		HKEY_LOCAL_MACHINE\System\CurrentControlSet\Services 
	この関数を使って指定した情報は、このキーの下の値として保存されます。
	*/
	SC_HANDLE svc = ::CreateService(
		scm
		,service_name		/* サービス名 */
		,display_name		/* サービスの表示名 */
		,SERVICE_ALL_ACCESS	/* 必要なアクセス権 */
		,SERVICE_WIN32_OWN_PROCESS	/* サービスの種類 */
		,SERVICE_AUTO_START	/* 開始方法:boot時は自動開始する */
		,SERVICE_ERROR_NORMAL	/* エラー発生時の対処法 */
		,binary_path_name	/* サービスのパス名 */
		,NULL			/* ロード順序グループの指定 */
		,NULL		/* ロード順序グループ内のタグを受け取る */
	/*
		dependenciesの設定について...
		--------------------------------------------------
		サービス名(こちら指定)	項目
		表示名		実行ファイルのパス
		--------------------------------------------------
		lanmanserver		ファイル共有サーバ側
		Server		C:\WINDOWS\system32\svchost.exe -k netsvcs
		lanmanworkstation	ファイル共有クライアント側
		Workstation	C:\WINDOWS\system32\svchost.exe -k netsvcs
		--------------------------------------------------
		,"LanmanWorkstation"
			--> 動作OK(2011-07-11)
		,"LanmanWorkstation\0LanmanServer"
	*/
		,dependencies	/* 依存するサービス名のリスト */
		,service_start_name/* アカウントのID(DomainName\UserName) */
		,password		/* パスワード */
	);
	if (NULL == svc) { /* サービスの生成でエラー */
		throw std::domain_error(igs_resource_msg_from_err(
			TEXT("CreateService(-)"),::GetLastError()
		));
	}
	return svc;
}
/* 説明文を設定する */
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
	/* 自動開始サービスを遅延(約2分後)させる
		Windows Vista/Windows 2008以降 */
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
	サービスを開始する
	*/
	if (0 == ::StartService(svc ,0 ,NULL)) {
		throw std::domain_error(igs_resource_msg_from_err(
			TEXT("StartService(-)"),::GetLastError()
		));
	}
}
void igs::resource::wsvc::close(SC_HANDLE scm_or_svc) {
	/*
	サービスコントロールマネージャーを閉じる、
	あるいは、サービスを閉じる(のも同じルーチンで行う)
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
	サービスの削除のためサービスコントロールマネージャーを開く
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
	指定のサービスが存在するかどうか調べる
	*/
	SC_HANDLE svc = ::OpenService(
		scm
		,service_name	/* サービス名 */
		,DELETE		/* アクセス権 */
	);
	if (NULL == svc) { /* サービスを開けない */
		DWORD err = ::GetLastError();
		if (ERROR_SERVICE_DOES_NOT_EXIST == err) {
			return false; /* 指定のサービスは存在しない */
		}
		/* エラーなので例外を投げる */
		throw std::domain_error(igs_resource_msg_from_err(
			TEXT("OpenService(-)"),err
		));
	}
	/* サービスが存在するなら閉じておく */
	igs::resource::wsvc::close(svc);
	return true; /* 指定のサービスが存在する */
}
SC_HANDLE igs::resource::wsvc::open(SC_HANDLE scm ,LPCTSTR service_name) {
	/*
	既存のサービスのハンドルを開きます
	指定のサービスが存在しない時もエラーとする
	*/
	SC_HANDLE svc = ::OpenService(
		scm
		,service_name	/* サービス名 */

		/* QueryServicesStatus(-)アクセス権 */
		,SERVICE_QUERY_STATUS

		/* ControlService(SERVICE_CONTROL_STOP)アクセス権 */
		|SERVICE_STOP

		/* DeleteService()アクセス権 */
		|DELETE

		/* StartService()アクセス権 */
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
	サービスの状態を得る
	現在のステータスを返す
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
	サービス停止要求(制御コード)をサービスアプリケーションへ送信します
	*/
	SERVICE_STATUS service_status;
	::ZeroMemory(&service_status,sizeof(SERVICE_STATUS));
	if (0 == ::ControlService(
		svc /* サービスのハンドル */
		,SERVICE_CONTROL_STOP /* 制御コード *//* サービス停止要求 */
		,&service_status  /* ステータス情報 */
	)) {
		throw std::domain_error(igs_resource_msg_from_err(
			TEXT("ControlService(-)"),::GetLastError()
		));
	}
}
void igs::resource::wsvc::mark_for_delete(SC_HANDLE svc) {
	/*
	指定したサービスに
	scmのデータベースから削除するためのマークを付けます
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
	LPCTSTR service_name /* 256文字以内 */
	,LPCTSTR display_name /* 256文字以内 */
	,LPCTSTR binary_path_name
	,LPCTSTR dependencies
	,LPCTSTR service_start_name
	,LPCTSTR password
	,LPCTSTR description_cp932
) {
	dg_pr_("01.新サービスを生成するためService Control Managerを開く");
	SC_HANDLE scm = igs::resource::wsvc::open_scm_for_create();
	dg_pr_("02.指定のサービスが存在するかどうか調べる");
	if (!igs::resource::wsvc::exist_for_delete(scm,service_name)) {
	dg_pr_("03.指定のサービスがないので新サービスを生成する");
		SC_HANDLE svc = igs::resource::wsvc::create( scm
			,service_name ,display_name ,binary_path_name
			,dependencies ,service_start_name ,password
		);
	dg_pr_("04.説明文を設定する");
		igs::resource::wsvc::description(svc,description_cp932);

	OSVERSIONINFO os_ver;
	os_ver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	::GetVersionEx( &os_ver );
	if (5 < os_ver.dwMajorVersion) { /* Windows Vista以後 */
	 dg_pr_("05.自動開始サービスを遅延開始に設定する");
		igs::resource::wsvc::delayed_auto_start( svc ,true );
	} else {			/* Windows 2003以前(Xp等) */
	 dg_pr_("06.自動開始サービスは(OSが古いため)遅延開始せず自動のまま");
	}

	dg_pr_("07.サービスを開始する");
		igs::resource::wsvc::start(svc);
	dg_pr_("08.サービスを閉じる");
		igs::resource::wsvc::close(svc);
	} else {
	dg_pr_("09.指定のサービスが既にあるのでなにもしない");
	}
	dg_pr_("10.Service Control Managerを閉じる");
	igs::resource::wsvc::close(scm);
}
void igs::resource::wsvc::start_only(
	LPCTSTR service_name /* 256文字以内 */
) {
	dg_pr_("01.Service Control Managerを開く");
	SC_HANDLE scm = igs::resource::wsvc::open_scm_for_create();
	dg_pr_("02.指定のサービスが存在するかどうか調べる");
	if (igs::resource::wsvc::exist_for_delete(scm,service_name)) {
	dg_pr_("03.サービスを開く");
		SC_HANDLE svc = igs::resource::wsvc::open(scm,service_name);
	dg_pr_("04.サービスを開始する");
		igs::resource::wsvc::start(svc);
	dg_pr_("05.サービスを閉じる");
		igs::resource::wsvc::close(svc);
	} else {
	dg_pr_("06.指定のサービスがないのでサービス開始できない");
	}
	dg_pr_("07.Service Control Managerを閉じる");
	igs::resource::wsvc::close(scm);
}
void igs::resource::wsvc::stop_delete(
	LPCTSTR service_name /* 256文字以内 */
	,const int msec
	,const int time_out_count
	,const bool delete_sw
) {
	if (delete_sw) {
 dg_pr_("01.サービス削除のためService Control ManagerをFull Accessで開く");
	} else {
 dg_pr_("02.Service Control ManagerをFull Accessで開く");
	}
	SC_HANDLE scm = igs::resource::wsvc::open_scm_for_access();

 dg_pr_("03.指定のサービスが存在するかどうか調べる");
	if (igs::resource::wsvc::exist_for_delete(scm,service_name)) {

 dg_pr_("04.指定のサービスがあるのでそのサービスを開く");
		SC_HANDLE svc = igs::resource::wsvc::open(scm,service_name);

/*
igs::resource::wsvc::status(svc);
igs::resource::wsvc::send_stop(svc);
は"user1"logonでアクセスを拒否されてしまう
は"user2"logonでもアクセスを拒否されてしまう
LoginUser()でuser2になる???
*/
 dg_pr_("05.サービスの状態を得る");
		DWORD sta = igs::resource::wsvc::status(svc);
		/* STOPしてなければ... */
		if (SERVICE_STOPPED != sta) {
			/* STOP信号を一回送り... */
 dg_pr_("06.STOPしてないのでサービスアプリケーションへ停止要求を送信する");
			igs::resource::wsvc::send_stop(svc);
		}
		/* STOP状態になるまでループで待つ... */
		for (int ii=0;;++ii) {
			if (time_out_count <= ii) {
	dg_pr_("07.STOPしないので、time outで抜ける --> エラーです");
				break;
			}

			/* 静止 */
			::Sleep(msec);

			/* サービスの状態を得る */
			sta = igs::resource::wsvc::status(svc);

			/* STOPしたら抜ける */
			std::ostringstream ost; ost << (ii+1)*msec/1000.;
			if (SERVICE_STOPPED == sta) {
				std::string str("08.STOPした ");
				str += ost.str(); str += "sec";
	dg_pr_(str);
				break;
			}
			{
				std::string str("09.まだSTOPしない");
				str += ost.str(); str += "sec";
	dg_pr_(str);
			}
		}
		if (delete_sw) {
	dg_pr_("10.指定のサービスにscmのデータベースから削除するためのマークを付けます");
		 igs::resource::wsvc::mark_for_delete(svc);
	dg_pr_("11.実際に削除し、サービスを閉じる");
		} else {
	dg_pr_("12.サービスを閉じる");
		}
		igs::resource::wsvc::close(svc);
	} else {
	dg_pr_("12.指定のサービスは存在しないのでなにもしない");
	}
	dg_pr_("13.Service Control Managerを閉じる");
	igs::resource::wsvc::close(scm);
}
