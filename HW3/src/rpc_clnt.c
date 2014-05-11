/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include <memory.h> /* for memset */
#include "rpc.h"

/* Default timeout can be changed using clnt_control() */
static struct timeval TIMEOUT = { 25, 0 };

msg *
hello_1(msg *argp, CLIENT *clnt)
{
	static msg clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, hello,
		(xdrproc_t) xdr_msg, (caddr_t) argp,
		(xdrproc_t) xdr_msg, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

msg *
start_1(msg *argp, CLIENT *clnt)
{
	static msg clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, start,
		(xdrproc_t) xdr_msg, (caddr_t) argp,
		(xdrproc_t) xdr_msg, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

msg *
send_1(msg *argp, CLIENT *clnt)
{
	static msg clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, send,
		(xdrproc_t) xdr_msg, (caddr_t) argp,
		(xdrproc_t) xdr_msg, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

msg *
betdouble_1(msg *argp, CLIENT *clnt)
{
	static msg clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, betdouble,
		(xdrproc_t) xdr_msg, (caddr_t) argp,
		(xdrproc_t) xdr_msg, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

msg *
hit_1(msg *argp, CLIENT *clnt)
{
	static msg clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, hit,
		(xdrproc_t) xdr_msg, (caddr_t) argp,
		(xdrproc_t) xdr_msg, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

msg *
stand_1(msg *argp, CLIENT *clnt)
{
	static msg clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, stand,
		(xdrproc_t) xdr_msg, (caddr_t) argp,
		(xdrproc_t) xdr_msg, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

msg *
ack_1(msg *argp, CLIENT *clnt)
{
	static msg clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, ack,
		(xdrproc_t) xdr_msg, (caddr_t) argp,
		(xdrproc_t) xdr_msg, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}
