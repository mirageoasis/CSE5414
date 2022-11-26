/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "cal.h"
#include <stdio.h>
#include <stdlib.h>
#include <rpc/pmap_clnt.h>
#include <string.h>
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>

#ifndef SIG_PF
#define SIG_PF void(*)(int)
#endif

static int *
_add_1 (add_1_argument *argp, struct svc_req *rqstp)
{
	return (add_1_svc(argp->first, argp->second, rqstp));
}

static int *
_sub_1 (sub_1_argument *argp, struct svc_req *rqstp)
{
	return (sub_1_svc(argp->first, argp->second, rqstp));
}

static int *
_div_1 (div_1_argument *argp, struct svc_req *rqstp)
{
	return (div_1_svc(argp->first, argp->second, rqstp));
}

static int *
_mul_1 (mul_1_argument *argp, struct svc_req *rqstp)
{
	return (mul_1_svc(argp->first, argp->second, rqstp));
}

static int *
_pow_1 (pow_1_argument *argp, struct svc_req *rqstp)
{
	return (pow_1_svc(argp->first, argp->second, rqstp));
}

static void
caleprog_1(struct svc_req *rqstp, register SVCXPRT *transp)
{
	union {
		add_1_argument add_1_arg;
		sub_1_argument sub_1_arg;
		div_1_argument div_1_arg;
		mul_1_argument mul_1_arg;
		pow_1_argument pow_1_arg;
	} argument;
	char *result;
	xdrproc_t _xdr_argument, _xdr_result;
	char *(*local)(char *, struct svc_req *);

	switch (rqstp->rq_proc) {
	case NULLPROC:
		(void) svc_sendreply (transp, (xdrproc_t) xdr_void, (char *)NULL);
		return;

	case ADD:
		_xdr_argument = (xdrproc_t) xdr_add_1_argument;
		_xdr_result = (xdrproc_t) xdr_int;
		local = (char *(*)(char *, struct svc_req *)) _add_1;
		break;

	case SUB:
		_xdr_argument = (xdrproc_t) xdr_sub_1_argument;
		_xdr_result = (xdrproc_t) xdr_int;
		local = (char *(*)(char *, struct svc_req *)) _sub_1;
		break;

	case DIV:
		_xdr_argument = (xdrproc_t) xdr_div_1_argument;
		_xdr_result = (xdrproc_t) xdr_int;
		local = (char *(*)(char *, struct svc_req *)) _div_1;
		break;

	case MUL:
		_xdr_argument = (xdrproc_t) xdr_mul_1_argument;
		_xdr_result = (xdrproc_t) xdr_int;
		local = (char *(*)(char *, struct svc_req *)) _mul_1;
		break;

	case POW:
		_xdr_argument = (xdrproc_t) xdr_pow_1_argument;
		_xdr_result = (xdrproc_t) xdr_int;
		local = (char *(*)(char *, struct svc_req *)) _pow_1;
		break;

	default:
		svcerr_noproc (transp);
		return;
	}
	memset ((char *)&argument, 0, sizeof (argument));
	if (!svc_getargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
		svcerr_decode (transp);
		return;
	}
	result = (*local)((char *)&argument, rqstp);
	if (result != NULL && !svc_sendreply(transp, (xdrproc_t) _xdr_result, result)) {
		svcerr_systemerr (transp);
	}
	if (!svc_freeargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
		fprintf (stderr, "%s", "unable to free arguments");
		exit (1);
	}
	return;
}

int
main (int argc, char **argv)
{
	register SVCXPRT *transp;

	pmap_unset (CALEPROG, CALMESSAGEVERS);

	transp = svcudp_create(RPC_ANYSOCK);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create udp service.");
		exit(1);
	}
	if (!svc_register(transp, CALEPROG, CALMESSAGEVERS, caleprog_1, IPPROTO_UDP)) {
		fprintf (stderr, "%s", "unable to register (CALEPROG, CALMESSAGEVERS, udp).");
		exit(1);
	}

	transp = svctcp_create(RPC_ANYSOCK, 0, 0);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create tcp service.");
		exit(1);
	}
	if (!svc_register(transp, CALEPROG, CALMESSAGEVERS, caleprog_1, IPPROTO_TCP)) {
		fprintf (stderr, "%s", "unable to register (CALEPROG, CALMESSAGEVERS, tcp).");
		exit(1);
	}

	svc_run ();
	fprintf (stderr, "%s", "svc_run returned");
	exit (1);
	/* NOTREACHED */
}
