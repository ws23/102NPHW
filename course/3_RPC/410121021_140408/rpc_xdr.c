/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "rpc.h"

bool_t
xdr_msg (XDR *xdrs, msg *objp)
{
	register int32_t *buf;

	 if (!xdr_int (xdrs, &objp->value))
		 return FALSE;
	return TRUE;
}
