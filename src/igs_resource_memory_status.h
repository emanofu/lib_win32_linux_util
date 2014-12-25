#ifndef igs_resource_memory_status_h
#define igs_resource_memory_status_h

#ifndef IGS_RESOURCE_IRF_EXPORT
# define IGS_RESOURCE_IRF_EXPORT
#endif

namespace igs {
 namespace resource {
  IGS_RESOURCE_IRF_EXPORT void memory_status(
	unsigned long long int&commit_charge_total_kbytes   /* 使用中合計 */
	,unsigned long long int&commit_charge_limit_kbytes /* 使用制限値 */
	,unsigned long long int&total_physical_kbytes      /* 物理量     */
  );
 }
}

#endif /* !igs_resource_memory_status_h */
