/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "rpc.h"

bool_t
xdr_msg (XDR *xdrs, msg *objp)
{
	register int32_t *buf;

	int i;
	 if (!xdr_uint8_t (xdrs, &objp->Version_CommandType))
		 return FALSE;
	 if (!xdr_uint8_t (xdrs, &objp->Length))
		 return FALSE;
	 if (!xdr_uint16_t (xdrs, &objp->Checksum))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->Card, 128,
		sizeof (int8_t), (xdrproc_t) xdr_int8_t))
		 return FALSE;
	return TRUE;
}
