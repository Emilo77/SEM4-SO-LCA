/* This is a function returning the lowest common ancestor of two processes.
 * */

#include "pm.h"
//todo może jakiś include
/*===========================================================================*
 *				get_lca_pid				     *
 *===========================================================================*/
int do_getlcapid(void)
//gets the lowest common ancestor of two processes
//if they are not related, returns -1 and sets errno to ESRCH
//if one of the processes is not found, returns -1 and sets errno to EINVAL
//if both processes are found, returns the lca pid
{
	int pid1 = m_in.m1_i1;
	int pid2 = m_in.m1_i2;
	int temp_pid1 = pid1;
	int temp_pid2 = pid2;
	int depth1 = 0;
	int depth2 = 0;

	register struct mproc *rmp;			/* check process table */

	if(pid1 < 0 || pid2 < 0 || !(mproc[pid1].mp_flags & IN_USE)
	   || !(mproc[pid2].mp_flags& IN_USE))
	{
		errno = EINVAL;
		return -1;
	}

	while (mproc[temp_pid1].mp_parent != temp_pid1)
	{
		temp_pid1 = mproc[temp_pid1].mp_parent;
		depth1++;
	}
	while (mproc[temp_pid2].mp_parent != temp_pid2)
	{
		temp_pid2 = mproc[temp_pid2].mp_parent;
		depth2++;
	}

	temp_pid1 = pid1;
	temp_pid2 = pid2;

	if (depth1 > depth2)
	{
		while (depth1 > depth2)
		{
			temp_pid1 = mproc[temp_pid1].mp_parent;
			depth1--;
		}
	}
	else if (depth2 > depth1)
	{
		while (depth2 > depth1)
		{
			temp_pid2 = mproc[temp_pid2].mp_parent;
			depth2--;
		}
	}

	while (depth1 != 0 && temp_pid1 != temp_pid2)
	{
		temp_pid1 = mproc[temp_pid1].mp_parent;
		temp_pid2 = mproc[temp_pid2].mp_parent;
		depth1--;
	}
	if (temp_pid1 != temp_pid2)
	{
		errno = ESRCH;
		return -1;
	}

	return temp_pid1;
}