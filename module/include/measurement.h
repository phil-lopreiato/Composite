#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#define MEASUREMENTS

typedef enum { 
	COS_MEAS_SWITCH_SELF,
	COS_MEAS_SWITCH_COOP,
	COS_MEAS_SWITCH_PREEMPT,
	COS_MEAS_INT_PREEMPT,
	COS_MEAS_INVOCATIONS,
	COS_MEAS_UPCALLS,
	COS_MEAS_BRAND_UC,
	COS_MEAS_BRAND_PEND,
	COS_MEAS_BRAND_COMPLETION_UC,
	COS_MEAS_BRAND_SCHED_PREEMPTED,
	COS_MEAS_FINISHED_BRANDS,
	COS_MEAS_INT_PREEMPT_USER,
	COS_MEAS_INT_PREEMPT_KERN,
	COS_MEAS_INT_COS_THD,
	COS_MEAS_OTHER_THD,
	COS_PG_FAULT,
	COS_LINUX_PG_FAULT,
	COS_UNKNOWN_FAULT,
	COS_MPD_ALLOC,
	COS_MPD_SUBORDINATE,
	COS_MPD_SPLIT_REUSE,
	COS_MPD_FREE,
	COS_MPD_REFCNT_INC,
	COS_MPD_REFCNT_DEC,
	COS_MPD_IPC_REFCNT_INC,
	COS_MPD_IPC_REFCNT_DEC,
	COS_ALLOC_PGTBL,
	COS_FREE_PGTBL,
	COS_MAP_GRANT,
	COS_MAP_REVOKE,
	COS_MEAS_ATOMIC_RBK,
	COS_MEAS_ATOMIC_STALE_LOCK,
	COS_MEAS_ATOMIC_LOCK,
	COS_MEAS_ATOMIC_UNLOCK,
	COS_MEAS_PACKET_RECEPTION,
	COS_MEAS_MAX_SIZE
} cos_meas_t;

#ifdef MEASUREMENTS
void cos_meas_init(void);
void cos_meas_report(void);

extern unsigned long long cos_measurements[COS_MEAS_MAX_SIZE];
static inline void cos_meas_event(cos_meas_t type)
{
	/* silent error is better here as we wish to avoid
	 * conditionals in hotpaths to check for the return value */
	if (type >= COS_MEAS_MAX_SIZE)
		return;

	cos_measurements[type]++;

	return;
}

#else

#define cos_meas_event(t)
#define cos_meas_init()
#define cos_meas_report()

#endif

#endif
