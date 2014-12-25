#ifndef igs_resource_wsvc_h
#define igs_resource_wsvc_h

#if defined _WIN32 //-------------------------------------------------
# include <windows.h>
# ifndef IGS_RESOURCE_IRF_EXPORT
#  define IGS_RESOURCE_IRF_EXPORT
# endif
namespace igs {
 namespace resource {
  namespace wsvc {
   SC_HANDLE open_scm_for_create(void);
   SC_HANDLE create(
	SC_HANDLE scm
	,LPCTSTR service_name /* 256文字以内 */
	,LPCTSTR display_name /* 256文字以内 */
	,LPCTSTR binary_path_name
	,LPCTSTR dependencies
	,LPCTSTR service_start_name
	,LPCTSTR password
   );
   void description(SC_HANDLE svc,LPCTSTR description_cp932);
   void delayed_auto_start( SC_HANDLE svc ,bool delayed );
   void start(SC_HANDLE svc);
   void close(SC_HANDLE scm_or_svc);
   SC_HANDLE open_scm_for_access(void);
   bool exist_for_delete(SC_HANDLE scm ,LPCTSTR service_name);
   SC_HANDLE open(SC_HANDLE scm ,LPCTSTR service_name);
   DWORD status(SC_HANDLE svc);
   void send_stop(SC_HANDLE svc);
   void mark_for_delete(SC_HANDLE svc);
   //--------------------------------------------------------
   void create_start(
	LPCTSTR service_name /* 256文字以内 */
	,LPCTSTR display_name /* 256文字以内 */
	,LPCTSTR binary_path_name
	,LPCTSTR dependencies
	,LPCTSTR service_start_name
	,LPCTSTR password
	,LPCTSTR description_cp932
   );
   void stop_delete(
	LPCTSTR service_name /* 256文字以内 */
	,const int msec = 500 // 100? // 0.5sec
	,const int time_out_count = 120 // 100? // 120 * 0.5sec = 60sec
	,const bool delete_sw=true /* delete処理sw。通常はdeleteする */
   );
   void start_only(
	LPCTSTR service_name /* 256文字以内 */
   );
  }
 }
}
#endif //-------------------------------------------------------------

#endif /* !igs_resource_wsvc_h */
