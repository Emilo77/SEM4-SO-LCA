/* This is a function returning the lowest common ancestor of two processes.
 * */

#include "pm.h"
#include "mproc.h"  // for process table mproc
#include "stdio.h" // for printf
/*===========================================================================*
 *				get_lca_pid				     *
 *===========================================================================*/
int do_getlcapid(void)
//gets the lowest common ancestor of two processes
//if they are not related, returns -1 and sets errno to ESRCH
//if one of the processes is not found, returns -1 and sets errno to EINVAL
//if both processes are found, returns the lca pid
{
	int pid_index_1;
	int pid_index_2;

	for (int i = 0; i < NR_PROCS; i++)
	{
		if (mproc[i].mp_pid == m_in.m1_i1)
		{
			pid_index_1 = i;
		}
		if (mproc[i].mp_pid == m_in.m1_i2)
		{
			pid_index_2 = i;
		}
	}

	int temp_index_1 = pid_index_1;
	int temp_index_2 = pid_index_2;
	int depth1 = 0;
	int depth2 = 0;

	register struct mproc *rmp;			/* check process table */

	if(m_in.m1_i1 < 0 || m_in.m1_i2 < 0
		|| !(mproc[pid_index_1].mp_flags & IN_USE)
		|| !(mproc[pid_index_2].mp_flags & IN_USE))
	{
		errno = EINVAL;
		printf("-1\n");
		return -1;
	}

	while (temp_index_1 != mproc[temp_index_1].mp_parent)
	{
//		printf("Parent of pid1 with index: %d is index %d\n", temp_index_1,
//			   mproc[temp_index_1].mp_parent);
		temp_index_1 = mproc[temp_index_1].mp_parent;
		depth1++;
	}
	while (temp_index_2 != mproc[temp_index_2].mp_parent)
	{
//		printf("Parent of pid2 with index: %d is index %d\n", temp_index_2,
//			   mproc[temp_index_2].mp_parent);
		temp_index_2 = mproc[temp_index_2].mp_parent;
		depth2++;
	}

	if (depth1 == 0 || depth2 == 0)
	{
		errno = ESRCH;
		printf("-1\n");
		return -1;
	}

//	printf("depth1: %d\n", depth1);
//	printf("depth2: %d\n", depth2);


	temp_index_1 = pid_index_1;
	temp_index_2 = pid_index_2;

	while (depth1 > depth2)
	{
		temp_index_1 = mproc[temp_index_1].mp_parent;
		depth1--;
	}

	while (depth2 > depth1)
	{
		temp_index_2 = mproc[temp_index_2].mp_parent;
		depth2--;
	}

	temp_index_1 = mproc[temp_index_1].mp_parent;
	temp_index_2 = mproc[temp_index_2].mp_parent;

	while (temp_index_1 != temp_index_2)
	{
		temp_index_1 = mproc[temp_index_1].mp_parent;
		temp_index_2 = mproc[temp_index_2].mp_parent;
	}


//	printf("Result %d with index %d\n", mproc[temp_index_1].mp_pid, temp_index_1);
	return mproc[temp_index_1].mp_pid;
}