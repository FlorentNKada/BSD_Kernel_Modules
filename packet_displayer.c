#include <sys/types.h>
#include <sys/module.h>
#include <sys/systm.h>
#include <sys/errno.h>
#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/conf.h>
#include <sys/uio.h>
#include <sys/malloc.h>
#include <sys/ioccom.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/sysent.h>
#include <sys/sysproto.h>
#include <sys/proc.h>
#include <sys/syscall.h>

#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <net/if.h>
#include <net/if_var.h>
#include <net/pfil.h>

static int packet_displayer_hooked = 0;

static int
chkinput(void *arg, struct mbuf **m, struct ifnet *ifp, int dir, struct inpcb *inp) {
	/*
	* arg      argument
	* mbuf     pointer to the packet structure
	* ifp      pointer to the interface structure (the origin of the packet)
	* dir      direction (PFIL_IN or PFIL_OUT)
	* inp      pointer to the protocol control block structure
	*/
	//(*m)->m_len //size of the packet

	uprintf("size of incomming packet: %lu\nif_name: %s\n",sizeof((*m)->m_len), ifp->if_xname); /* a formatter; *(*m)->m_data */
	switch(dir) {
	case PFIL_IN:
		uprintf("paquet entrant\n");
		break;
	case PFIL_OUT:
		uprintf("packet sortant\n");
		break;
	default:
		uprintf("erreur avec la variable dir\n");
		break;
	}
	uprintf("\n");
	return 0;
}

static int
chkoutput(void *arg, struct mbuf **m, struct ifnet *ifp, int dir, struct inpcb *inp) {
	uprintf("size of outgoing packet: %lu\n",sizeof((*m)->m_len)); /* a formatter */
	return 0;
}

static int init_module() {
	struct pfil_head *pfh_inet;

	if(packet_displayer_hooked)
		return 0;
	pfh_inet = pfil_head_get(PFIL_TYPE_AF, AF_INET);

	if(pfh_inet == NULL)
		return ESRCH;

	pfil_add_hook(chkinput, NULL, PFIL_IN | PFIL_WAITOK, pfh_inet);
	pfil_add_hook(chkoutput, NULL, PFIL_OUT | PFIL_WAITOK, pfh_inet);
	packet_displayer_hooked = 1;
	uprintf("MOD packet_displayer loaded\n");
	return 0;
}

static int
deinit_module() {
	struct pfil_head *pfh_inet;

	if(!packet_displayer_hooked)
		return 0;

	pfh_inet = pfil_head_get(PFIL_TYPE_AF, AF_INET);

	if(pfh_inet == NULL)
		return ESRCH;

	pfil_remove_hook(chkinput, NULL, PFIL_IN | PFIL_WAITOK, pfh_inet);
	pfil_remove_hook(chkoutput, NULL, PFIL_OUT | PFIL_WAITOK, pfh_inet);
	packet_displayer_hooked = 0;
	//free(flwbuf, M_TEMP);
	//destroy_dev(sdev);
	uprintf("MOD packet_displayer unloaded\n");
	return 0;
}

/* Module event handler */
static int
event_handler(struct module *m, int what, void *arg) {
	int err = 0;

	switch(what) {
	case MOD_LOAD:
		err = init_module();
		break;
	case MOD_UNLOAD:
		err = deinit_module();
		break;
	default:
		err = EINVAL;
		break;
	}
	return err;
}

static moduledata_t packet_displayer_conf = {
	"packet_displayer",
	event_handler,
	NULL
};

DECLARE_MODULE(packet_displayer, packet_displayer_conf, SI_SUB_DRIVERS, SI_ORDER_MIDDLE);
