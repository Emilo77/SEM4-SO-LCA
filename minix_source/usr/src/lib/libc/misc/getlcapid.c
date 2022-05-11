#include <lib.h>
#include <minix/rs.h>
#include <unistd.h>
#include <string.h>

static int get_pm_endpt(endpoint_t *pt)
{
	return minix_rs_lookup("pm", pt);
}

pid_t getlcapid(pid_t pid_1, pid_t pid_2)
{
	endpoint_t pm_pt;
	message m;
	memset(&m, 0, sizeof(m));
	m.m1_i1 = pid_1;
	m.m1_i2 = pid_2;
	if (get_pm_endpt(&pm_pt) != 0)
	{
		errno = ENOSYS;
		return -1;
	}
	return (_syscall(pm_pt, PM_GETLCAPID, &m));
}
