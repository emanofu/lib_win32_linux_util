#ifndef igs_resource_memory_status_h
#define igs_resource_memory_status_h

#ifndef IGS_RESOURCE_IRF_EXPORT
# define IGS_RESOURCE_IRF_EXPORT
#endif

namespace igs {
 namespace resource {
  IGS_RESOURCE_IRF_EXPORT void memory_status(
	unsigned long long int&commit_charge_total_kbytes   /* �������� */
	,unsigned long long int&commit_charge_limit_kbytes /* ���������� */
	,unsigned long long int&total_physical_kbytes      /* ʪ����     */
  );
 }
}

#endif /* !igs_resource_memory_status_h */
