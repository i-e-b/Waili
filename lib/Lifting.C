//
//	Integer Lifting Operations
//
//  $Id: Lifting.C,v 4.5.2.1.2.1 1999/07/20 16:15:51 geert Exp $
//
//  Copyright (C) 1996-1999 Department of Computer Science, K.U.Leuven, Belgium
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include <waili/Channel.h>


const char *Lifting::rcsid = "$Id: Lifting.C,v 4.5.2.1.2.1 1999/07/20 16:15:51 geert Exp $";


    //  Clipping with symmetric signal extension

static inline int clip_11(int idx, u_int len)
{
    if (idx < 0)
	idx = -idx;
    idx %= 2*(len-1);
    if ((u_int)idx >= len)
	idx = 2*len-idx-2;
    return(idx);
}

static inline int clip_12(int idx, u_int len)
{
    if (idx < 0)
	idx = -idx;
    idx %= 2*len-1;
    if ((u_int)idx >= len)
	idx = 2*len-idx-1;
    return(idx);
}

static inline int clip_21(int idx, u_int len)
{
    if (idx < 0)
	idx = -idx-1;
    idx %= 2*len-1;
    if ((u_int)idx >= len)
	idx = 2*len-idx-2;
    return(idx);
}

static inline int clip_22(int idx, u_int len)
{
    if (idx < 0)
	idx = -idx-1;
    idx %= 2*len;
    if ((u_int)idx >= len)
	idx = 2*len-idx-1;
    return(idx);
}


    //  Integer division with rounding

template <class Type>
static inline Type divrnd(Type p, u_int d)
{
    int d2 = d>>1;
    if (p < 0)
	d2 = -d2;
    p += d2;
    return(p/(int)d);
}


// ----------------------------------------------------------------------------
//
//	Integer Lifting Operations on the Rows of NTChannels
//
// ----------------------------------------------------------------------------


    //  Lifting Steps (Full Rounding)

void LiftChannelR::Lift_L1R1_FR(int primal, const s16 b[2], const u16 a) const
{
    PixType *even = Even;
    PixType *odd = Odd;

    if (primal ^ Parity)
	for (u_int i = 0; i < Rows; i++) {
	    even[0] += divrnd((b[0]+b[1])*odd[0], a);
	    for (u_int j = 1; j < Codd; j++)
		even[j] += divrnd(b[0]*odd[j-1]+b[1]*odd[j], a);
	    if (Ceven != Codd)
		even[Ceven-1] += divrnd((b[0]+b[1])*odd[Codd-1], a);
	    even += Ceven;
	    odd += Codd;
	}
    else
	for (u_int i = 0; i < Rows; i++) {
	    for (u_int j = 0; j < Ceven-1; j++)
		odd[j] += divrnd(b[0]*even[j]+b[1]*even[j+1], a);
	    if (Ceven == Codd)
		odd[Codd-1] += divrnd((b[0]+b[1])*even[Ceven-1], a);
	    even += Ceven;
	    odd += Codd;
	}
}

void LiftChannelR::ILift_L1R1_FR(int primal, const s16 b[2], const u16 a) const
{
    PixType *even = Even;
    PixType *odd = Odd;

    if (primal ^ Parity)
	for (u_int i = 0; i < Rows; i++) {
	    even[0] -= divrnd((b[0]+b[1])*odd[0], a);
	    for (u_int j = 1; j < Codd; j++)
		even[j] -= divrnd(b[0]*odd[j-1]+b[1]*odd[j], a);
	    if (Ceven != Codd)
		even[Ceven-1] -= divrnd((b[0]+b[1])*odd[Codd-1], a);
	    even += Ceven;
	    odd += Codd;
	}
    else
	for (u_int i = 0; i < Rows; i++) {
	    for (u_int j = 0; j < Ceven-1; j++)
		odd[j] -= divrnd(b[0]*even[j]+b[1]*even[j+1], a);
	    if (Ceven == Codd)
		odd[Codd-1] -= divrnd((b[0]+b[1])*even[Ceven-1], a);
	    even += Ceven;
	    odd += Codd;
	}
}


void LiftChannelR::Lift_L2R2_FR(int primal, const s16 b[4], const u16 a) const
{
    PixType *src, *dst;
    u_int srclen, dstlen;
    int equal_idx_left;
    int (*clip)(int idx, u_int len);

    if (primal ^ Parity) {
	srclen = Codd;
	src = Odd;
	dst = Even;
	dstlen = Ceven;
	equal_idx_left = 0;
	clip = Ceven == Codd ? clip_21 : clip_22;
    } else {
	src = Even;
	srclen = Ceven;
	dst = Odd;
	dstlen = Codd;
	equal_idx_left = 1;
	clip = Ceven == Codd ? clip_12 : clip_11;
    }
    for (u_int i = 0; i < Rows; i++) {
	for (u_int j = 0; j < dstlen; j++)
	    dst[j] += divrnd(b[0]*src[clip(equal_idx_left+j-2, srclen)]+
			     b[1]*src[clip(equal_idx_left+j-1, srclen)]+
			     b[2]*src[clip(equal_idx_left+j, srclen)]+
			     b[3]*src[clip(equal_idx_left+j+1, srclen)], a);
	dst += dstlen;
	src += srclen;
    }
}

void LiftChannelR::ILift_L2R2_FR(int primal, const s16 b[4], const u16 a) const
{
    PixType *src, *dst;
    u_int srclen, dstlen;
    int equal_idx_left;
    int (*clip)(int idx, u_int len);

    if (primal ^ Parity) {
	src = Odd;
	srclen = Codd;
	dst = Even;
	dstlen = Ceven;
	equal_idx_left = 0;
	clip = Ceven == Codd ? clip_21 : clip_22;
    } else {
	src = Even;
	srclen = Ceven;
	dst = Odd;
	dstlen = Codd;
	equal_idx_left = 1;
	clip = Ceven == Codd ? clip_12 : clip_11;
    }
    for (u_int i = 0; i < Rows; i++) {
	for (u_int j = 0; j < dstlen; j++)
	    dst[j] -= divrnd(b[0]*src[clip(equal_idx_left+j-2, srclen)]+
			     b[1]*src[clip(equal_idx_left+j-1, srclen)]+
			     b[2]*src[clip(equal_idx_left+j, srclen)]+
			     b[3]*src[clip(equal_idx_left+j+1, srclen)], a);
	dst += dstlen;
	src += srclen;
    }
}

void LiftChannelR::Lift_L3R3_FR(int primal, const s16 b[6], const u16 a) const
{
    PixType *src, *dst;
    u_int srclen, dstlen;
    int equal_idx_left;
    int (*clip)(int idx, u_int len);

    if (primal ^ Parity) {
	src = Odd;
	srclen = Codd;
	dst = Even;
	dstlen = Ceven;
	equal_idx_left = 0;
	clip = Ceven == Codd ? clip_21 : clip_22;
    } else {
	src = Even;
	srclen = Ceven;
	dst = Odd;
	dstlen = Codd;
	equal_idx_left = 1;
	clip = Ceven == Codd ? clip_12 : clip_11;
    }
    for (u_int i = 0; i < Rows; i++) {
	for (u_int j = 0; j < dstlen; j++)
	    dst[j] += divrnd(b[0]*src[clip(equal_idx_left+j-3, srclen)]+
			     b[1]*src[clip(equal_idx_left+j-2, srclen)]+
			     b[2]*src[clip(equal_idx_left+j-1, srclen)]+
			     b[3]*src[clip(equal_idx_left+j, srclen)]+
			     b[4]*src[clip(equal_idx_left+j+1, srclen)]+
			     b[5]*src[clip(equal_idx_left+j+2, srclen)], a);
	dst += dstlen;
	src += srclen;
    }
}

void LiftChannelR::ILift_L3R3_FR(int primal, const s16 b[6], const u16 a) const
{
    PixType *src, *dst;
    u_int srclen, dstlen;
    int equal_idx_left;
    int (*clip)(int idx, u_int len);

    if (primal ^ Parity) {
	src = Odd;
	srclen = Codd;
	dst = Even;
	dstlen = Ceven;
	equal_idx_left = 0;
	clip = Ceven == Codd ? clip_21 : clip_22;
    } else {
	src = Even;
	srclen = Ceven;
	dst = Odd;
	dstlen = Codd;
	equal_idx_left = 1;
	clip = Ceven == Codd ? clip_12 : clip_11;
    }
    for (u_int i = 0; i < Rows; i++) {
	for (u_int j = 0; j < dstlen; j++)
	    dst[j] -= divrnd(b[0]*src[clip(equal_idx_left+j-3, srclen)]+
			     b[1]*src[clip(equal_idx_left+j-2, srclen)]+
			     b[2]*src[clip(equal_idx_left+j-1, srclen)]+
			     b[3]*src[clip(equal_idx_left+j, srclen)]+
			     b[4]*src[clip(equal_idx_left+j+1, srclen)]+
			     b[5]*src[clip(equal_idx_left+j+2, srclen)], a);
	dst += dstlen;
	src += srclen;
    }
}


    //  Lifting Steps (No Rounding)

void LiftChannelR::Lift_L1R1_NR(int primal, const s16 b[2], const u16 a) const
{
    PixType *even = Even;
    PixType *odd = Odd;

    if (primal ^ Parity)
	for (u_int i = 0; i < Rows; i++) {
	    even[0] = a*even[0]+(b[0]+b[1])*odd[0];
	    for (u_int j = 1; j < Codd; j++)
		even[j] = a*even[j]+b[0]*odd[j-1]+b[1]*odd[j];
	    if (Ceven != Codd)
		even[Ceven-1] = a*even[Ceven-1]+(b[0]+b[1])*odd[Codd-1];
	    even += Ceven;
	    odd += Codd;
	}
    else
	for (u_int i = 0; i < Rows; i++) {
	    for (u_int j = 0; j < Ceven-1; j++)
		odd[j] = a*odd[j]+b[0]*even[j]+b[1]*even[j+1];
	    if (Ceven == Codd)
		odd[Codd-1] = a*odd[Codd-1]+(b[0]+b[1])*even[Ceven-1];
	    even += Ceven;
	    odd += Codd;
	}
}

void LiftChannelR::ILift_L1R1_NR(int primal, const s16 b[2], const u16 a) const
{
    PixType *even = Even;
    PixType *odd = Odd;

    if (primal ^ Parity)
	for (u_int i = 0; i < Rows; i++) {
	    even[0] = (even[0]-(b[0]+b[1])*odd[0])/a;
	    for (u_int j = 1; j < Codd; j++)
		even[j] = (even[j]-b[0]*odd[j-1]-b[1]*odd[j])/a;
	    if (Ceven != Codd)
		even[Ceven-1] = (even[Ceven-1]-(b[0]+b[1])*odd[Codd-1])/a;
	    even += Ceven;
	    odd += Codd;
	}
    else
	for (u_int i = 0; i < Rows; i++) {
	    for (u_int j = 0; j < Ceven-1; j++)
		odd[j] = (odd[j]-b[0]*even[j]-b[1]*even[j+1])/a;
	    if (Ceven == Codd)
		odd[Codd-1] = (odd[Codd-1]-(b[0]+b[1])*even[Ceven-1])/a;
	    even += Ceven;
	    odd += Codd;
	}
}

void LiftChannelR::Lift_L2R2_NR(int primal, const s16 b[4], const u16 a) const
{
    PixType *src, *dst;
    u_int srclen, dstlen;
    int equal_idx_left;
    int (*clip)(int idx, u_int len);

    if (primal ^ Parity) {
	src = Odd;
	srclen = Codd;
	dst = Even;
	dstlen = Ceven;
	equal_idx_left = 0;
	clip = Ceven == Codd ? clip_21 : clip_22;
    } else {
	src = Even;
	srclen = Ceven;
	dst = Odd;
	dstlen = Codd;
	equal_idx_left = 1;
	clip = Ceven == Codd ? clip_12 : clip_11;
    }
    for (u_int i = 0; i < Rows; i++) {
	for (u_int j = 0; j < dstlen; j++)
	    dst[j] = a*dst[j]+b[0]*src[clip(equal_idx_left+j-2, srclen)]+
			      b[1]*src[clip(equal_idx_left+j-1, srclen)]+
			      b[2]*src[clip(equal_idx_left+j, srclen)]+
			      b[3]*src[clip(equal_idx_left+j+1, srclen)];
	dst += dstlen;
	src += srclen;
    }
}

void LiftChannelR::ILift_L2R2_NR(int primal, const s16 b[4], const u16 a) const
{
    PixType *src, *dst;
    u_int srclen, dstlen;
    int equal_idx_left;
    int (*clip)(int idx, u_int len);

    if (primal ^ Parity) {
	src = Odd;
	srclen = Codd;
	dst = Even;
	dstlen = Ceven;
	equal_idx_left = 0;
	clip = Ceven == Codd ? clip_21 : clip_22;
    } else {
	src = Even;
	srclen = Ceven;
	dst = Odd;
	dstlen = Codd;
	equal_idx_left = 1;
	clip = Ceven == Codd ? clip_12 : clip_11;
    }
    for (u_int i = 0; i < Rows; i++) {
	for (u_int j = 0; j < dstlen; j++)
	    dst[j] = (dst[j]-(b[0]*src[clip(equal_idx_left+j-2, srclen)]+
			      b[1]*src[clip(equal_idx_left+j-1, srclen)]+
			      b[2]*src[clip(equal_idx_left+j, srclen)]+
			      b[3]*src[clip(equal_idx_left+j+1, srclen)]))/a;
	dst += dstlen;
	src += srclen;
    }
}

void LiftChannelR::Lift_L3R3_NR(int primal, const s16 b[6], const u16 a) const
{
    PixType *src, *dst;
    u_int srclen, dstlen;
    int equal_idx_left;
    int (*clip)(int idx, u_int len);

    if (primal ^ Parity) {
	src = Odd;
	srclen = Codd;
	dst = Even;
	dstlen = Ceven;
	equal_idx_left = 0;
	clip = Ceven == Codd ? clip_21 : clip_22;
    } else {
	src = Even;
	srclen = Ceven;
	dst = Odd;
	dstlen = Codd;
	equal_idx_left = 1;
	clip = Ceven == Codd ? clip_12 : clip_11;
    }
    for (u_int i = 0; i < Rows; i++) {
	for (u_int j = 0; j < dstlen; j++)
	    dst[j] = a*dst[j]+b[0]*src[clip(equal_idx_left+j-3, srclen)]+
			      b[1]*src[clip(equal_idx_left+j-2, srclen)]+
			      b[2]*src[clip(equal_idx_left+j-1, srclen)]+
			      b[3]*src[clip(equal_idx_left+j, srclen)]+
			      b[4]*src[clip(equal_idx_left+j+1, srclen)]+
			      b[5]*src[clip(equal_idx_left+j+2, srclen)];
	dst += dstlen;
	src += srclen;
    }
}

void LiftChannelR::ILift_L3R3_NR(int primal, const s16 b[6], const u16 a) const
{
    PixType *src, *dst;
    u_int srclen, dstlen;
    int equal_idx_left;
    int (*clip)(int idx, u_int len);

    if (primal ^ Parity) {
	src = Odd;
	srclen = Codd;
	dst = Even;
	dstlen = Ceven;
	equal_idx_left = 0;
	clip = Ceven == Codd ? clip_21 : clip_22;
    } else {
	src = Even;
	srclen = Ceven;
	dst = Odd;
	dstlen = Codd;
	equal_idx_left = 1;
	clip = Ceven == Codd ? clip_12 : clip_11;
    }
    for (u_int i = 0; i < Rows; i++) {
	for (u_int j = 0; j < dstlen; j++)
	    dst[j] = (dst[j]-(b[0]*src[clip(equal_idx_left+j-3, srclen)]+
			      b[1]*src[clip(equal_idx_left+j-2, srclen)]+
			      b[2]*src[clip(equal_idx_left+j-1, srclen)]+
			      b[3]*src[clip(equal_idx_left+j, srclen)]+
			      b[4]*src[clip(equal_idx_left+j+1, srclen)]+
			      b[5]*src[clip(equal_idx_left+j+2, srclen)]))/a;
	dst += dstlen;
	src += srclen;
    }
}


    //  Lifting Steps (Mixed)

void LiftChannelR::Lift_L1R1_MX(int primal, const s16 b[2], const u16 a1,
				const u16 a2) const
{
    PixType *even = Even;
    PixType *odd = Odd;

    if (primal ^ Parity)
	for (u_int i = 0; i < Rows; i++) {
	    even[0] = a1*even[0]+divrnd((b[0]+b[1])*odd[0], a2);
	    for (u_int j = 1; j < Codd; j++)
		even[j] = a1*even[j]+divrnd(b[0]*odd[j-1]+b[1]*odd[j], a2);
	    if (Ceven != Codd)
		even[Ceven-1] = a1*even[Ceven-1]+
				divrnd((b[0]+b[1])*odd[Codd-1], a2);
	    even += Ceven;
	    odd += Codd;
	}
    else
	for (u_int i = 0; i < Rows; i++) {
	    for (u_int j = 0; j < Ceven-1; j++)
		odd[j] = a1*odd[j]+divrnd(b[0]*even[j]+b[1]*even[j+1], a2);
	    if (Ceven == Codd)
		odd[Codd-1] = a1*odd[Codd-1]+
			      divrnd((b[0]+b[1])*even[Ceven-1], a2);
	    even += Ceven;
	    odd += Codd;
	}
}

void LiftChannelR::ILift_L1R1_MX(int primal, const s16 b[2], const u16 a1,
				 const u16 a2) const
{
    PixType *even = Even;
    PixType *odd = Odd;

    if (primal ^ Parity)
	for (u_int i = 0; i < Rows; i++) {
	    even[0] = (even[0]-divrnd((b[0]+b[1])*odd[0], a2))/a1;
	    for (u_int j = 1; j < Codd; j++)
		even[j] = (even[j]-divrnd(b[0]*odd[j-1]+b[1]*odd[j], a2))/a1;
	    if (Ceven != Codd)
		even[Ceven-1] = (even[Ceven-1]-
				 divrnd((b[0]+b[1])*odd[Codd-1], a2))/a1;
	    even += Ceven;
	    odd += Codd;
	}
    else
	for (u_int i = 0; i < Rows; i++) {
	    for (u_int j = 0; j < Ceven-1; j++)
		odd[j] = (odd[j]-divrnd(b[0]*even[j]+b[1]*even[j+1], a2))/a1;
	    if (Ceven == Codd)
		odd[Codd-1] = (odd[Codd-1]-
			       divrnd((b[0]+b[1])*even[Ceven-1], a2))/a1;
	    even += Ceven;
	    odd += Codd;
	}
}

void LiftChannelR::Lift_L2R2_MX(int primal, const s16 b[4], const u16 a1,
				const u16 a2) const
{
    PixType *src, *dst;
    u_int srclen, dstlen;
    int equal_idx_left;
    int (*clip)(int idx, u_int len);

    if (primal ^ Parity) {
	src = Odd;
	srclen = Codd;
	dst = Even;
	dstlen = Ceven;
	equal_idx_left = 0;
	clip = Ceven == Codd ? clip_21 : clip_22;
    } else {
	src = Even;
	srclen = Ceven;
	dst = Odd;
	dstlen = Codd;
	equal_idx_left = 1;
	clip = Ceven == Codd ? clip_12 : clip_11;
    }
    for (u_int i = 0; i < Rows; i++) {
	for (u_int j = 0; j < dstlen; j++)
	    dst[j] = a1*dst[j]+
		     divrnd(b[0]*src[clip(equal_idx_left+j-2, srclen)]+
			    b[1]*src[clip(equal_idx_left+j-1, srclen)]+
			    b[2]*src[clip(equal_idx_left+j, srclen)]+
			    b[3]*src[clip(equal_idx_left+j+1, srclen)], a2);
	dst += dstlen;
	src += srclen;
    }
}

void LiftChannelR::ILift_L2R2_MX(int primal, const s16 b[4], const u16 a1,
				 const u16 a2) const
{
    PixType *src, *dst;
    u_int srclen, dstlen;
    int equal_idx_left;
    int (*clip)(int idx, u_int len);

    if (primal ^ Parity) {
	src = Odd;
	srclen = Codd;
	dst = Even;
	dstlen = Ceven;
	equal_idx_left = 0;
	clip = Ceven == Codd ? clip_21 : clip_22;
    } else {
	src = Even;
	srclen = Ceven;
	dst = Odd;
	dstlen = Codd;
	equal_idx_left = 1;
	clip = Ceven == Codd ? clip_12 : clip_11;
    }
    for (u_int i = 0; i < Rows; i++) {
	for (u_int j = 0; j < dstlen; j++)
	    dst[j] = (dst[j]-
		      divrnd(b[0]*src[clip(equal_idx_left+j-2, srclen)]+
			     b[1]*src[clip(equal_idx_left+j-1, srclen)]+
			     b[2]*src[clip(equal_idx_left+j, srclen)]+
			     b[3]*src[clip(equal_idx_left+j+1, srclen)], a2))/
		     a1;
	dst += dstlen;
	src += srclen;
    }
}

void LiftChannelR::Lift_L3R3_MX(int primal, const s16 b[6], const u16 a1,
				const u16 a2) const
{
    PixType *src, *dst;
    u_int srclen, dstlen;
    int equal_idx_left;
    int (*clip)(int idx, u_int len);

    if (primal ^ Parity) {
	src = Odd;
	srclen = Codd;
	dst = Even;
	dstlen = Ceven;
	equal_idx_left = 0;
	clip = Ceven == Codd ? clip_21 : clip_22;
    } else {
	src = Even;
	srclen = Ceven;
	dst = Odd;
	dstlen = Codd;
	equal_idx_left = 1;
	clip = Ceven == Codd ? clip_12 : clip_11;
    }
    for (u_int i = 0; i < Rows; i++) {
	for (u_int j = 0; j < dstlen; j++)
	    dst[j] = a1*dst[j]+
		     divrnd(b[0]*src[clip(equal_idx_left+j-3, srclen)]+
			    b[1]*src[clip(equal_idx_left+j-2, srclen)]+
			    b[2]*src[clip(equal_idx_left+j-1, srclen)]+
			    b[3]*src[clip(equal_idx_left+j, srclen)]+
			    b[4]*src[clip(equal_idx_left+j+1, srclen)]+
			    b[5]*src[clip(equal_idx_left+j+2, srclen)], a2);
	dst += dstlen;
	src += srclen;
    }
}

void LiftChannelR::ILift_L3R3_MX(int primal, const s16 b[6], const u16 a1,
				 const u16 a2) const
{
    PixType *src, *dst;
    u_int srclen, dstlen;
    int equal_idx_left;
    int (*clip)(int idx, u_int len);

    if (primal ^ Parity) {
	src = Odd;
	srclen = Codd;
	dst = Even;
	dstlen = Ceven;
	equal_idx_left = 0;
	clip = Ceven == Codd ? clip_21 : clip_22;
    } else {
	src = Even;
	srclen = Ceven;
	dst = Odd;
	dstlen = Codd;
	equal_idx_left = 1;
	clip = Ceven == Codd ? clip_12 : clip_11;
    }
    for (u_int i = 0; i < Rows; i++) {
	for (u_int j = 0; j < dstlen; j++)
	    dst[j] = (dst[j]-
		      divrnd(b[0]*src[clip(equal_idx_left+j-3, srclen)]+
			     b[1]*src[clip(equal_idx_left+j-2, srclen)]+
			     b[2]*src[clip(equal_idx_left+j-1, srclen)]+
			     b[3]*src[clip(equal_idx_left+j, srclen)]+
			     b[4]*src[clip(equal_idx_left+j+1, srclen)]+
			     b[5]*src[clip(equal_idx_left+j+2, srclen)], a2))/
		     a1;
	dst += dstlen;
	src += srclen;
    }
}


    //  Optimized functions for specific wavelet transforms

void LiftChannelR::Lift_L1R1_NR_0_m1_m1_2(void) const
{
    PixType *even = Even;
    PixType *odd = Odd;

    if (Parity)
	for (u_int i = 0; i < Rows; i++) {
	    even[0] = (even[0]-odd[0])<<1;
	    for (u_int j = 1; j < Codd; j++)
		even[j] = (even[j]<<1)-odd[j-1]-odd[j];
	    if (Ceven != Codd)
		even[Ceven-1] = (even[Ceven-1]-odd[Codd-1])<<1;
	    even += Ceven;
	    odd += Codd;
	}
    else
	for (u_int i = 0; i < Rows; i++) {
	    for (u_int j = 0; j < Ceven-1; j++)
		odd[j] = (odd[j]<<1)-even[j]-even[j+1];
	    if (Ceven == Codd)
		odd[Codd-1] = (odd[Codd-1]-even[Ceven-1])<<1;
	    even += Ceven;
	    odd += Codd;
	}
}

void LiftChannelR::ILift_L1R1_NR_0_m1_m1_2(void) const
{
    PixType *even = Even;
    PixType *odd = Odd;

    if (Parity)
	for (u_int i = 0; i < Rows; i++) {
	    even[0] = (even[0]>>1)+odd[0];
	    for (u_int j = 1; j < Codd; j++)
		even[j] = (even[j]+odd[j-1]+odd[j])>>1;
	    if (Ceven != Codd)
		even[Ceven-1] = (even[Ceven-1]>>1)+odd[Codd-1];
	    even += Ceven;
	    odd += Codd;
	}
    else
	for (u_int i = 0; i < Rows; i++) {
	    for (u_int j = 0; j < Ceven-1; j++)
		odd[j] = (odd[j]+even[j]+even[j+1])>>1;
	    if (Ceven == Codd)
		odd[Codd-1] = (odd[Codd-1]>>1)+even[Ceven-1];
	    even += Ceven;
	    odd += Codd;
	}
}

void LiftChannelR::Lift_L1R1_FR_1_1_1_8(void) const
{
    PixType *even = Even;
    PixType *odd = Odd;

    if (!Parity)
	for (u_int i = 0; i < Rows; i++) {
	    even[0] += divrnd(odd[0], 4);
	    for (u_int j = 1; j < Codd; j++)
		even[j] += divrnd(odd[j-1]+odd[j], 8);
	    if (Ceven != Codd)
		even[Ceven-1] += divrnd(odd[Codd-1], 4);
	    even += Ceven;
	    odd += Codd;
	}
    else
	for (u_int i = 0; i < Rows; i++) {
	    for (u_int j = 0; j < Ceven-1; j++)
		odd[j] += divrnd(even[j]+even[j+1], 8);
	    if (Ceven == Codd)
		odd[Codd-1] += divrnd(even[Ceven-1], 4);
	    even += Ceven;
	    odd += Codd;
	}
}

void LiftChannelR::ILift_L1R1_FR_1_1_1_8(void) const
{
    PixType *even = Even;
    PixType *odd = Odd;

    if (!Parity)
	for (u_int i = 0; i < Rows; i++) {
	    even[0] -= divrnd(odd[0], 4);
	    for (u_int j = 1; j < Codd; j++)
		even[j] -= divrnd(odd[j-1]+odd[j], 8);
	    if (Ceven != Codd)
		even[Ceven-1] -= divrnd(odd[Codd-1], 4);
	    even += Ceven;
	    odd += Codd;
	}
    else
	for (u_int i = 0; i < Rows; i++) {
	    for (u_int j = 0; j < Ceven-1; j++)
		odd[j] -= divrnd(even[j]+even[j+1], 8);
	    if (Ceven == Codd)
		odd[Codd-1] -= divrnd(even[Ceven-1], 4);
	    even += Ceven;
	    odd += Codd;
	}
}


void LiftChannelR::Lift_L1R1_FR_0_m1_m1_2(void) const
{
    PixType *even = Even;
    PixType *odd = Odd;

    if (Parity)
	for (u_int i = 0; i < Rows; i++) {
	    even[0] -= odd[0];
	    for (u_int j = 1; j < Codd; j++)
		even[j] += divrnd(-odd[j-1]-odd[j], 2);
	    if (Ceven != Codd)
		even[Ceven-1] -= odd[Codd-1];
	    even += Ceven;
	    odd += Codd;
	}
    else
	for (u_int i = 0; i < Rows; i++) {
	    for (u_int j = 0; j < Ceven-1; j++)
		odd[j] += divrnd(-even[j]-even[j+1], 2);
	    if (Ceven == Codd)
		odd[Codd-1] -= even[Ceven-1];
	    even += Ceven;
	    odd += Codd;
	}
}

void LiftChannelR::ILift_L1R1_FR_0_m1_m1_2(void) const
{
    PixType *even = Even;
    PixType *odd = Odd;

    if (Parity)
	for (u_int i = 0; i < Rows; i++) {
	    even[0] += odd[0];
	    for (u_int j = 1; j < Codd; j++)
		even[j] -= divrnd(-odd[j-1]-odd[j], 2);
	    if (Ceven != Codd)
		even[Ceven-1] += odd[Codd-1];
	    even += Ceven;
	    odd += Codd;
	}
    else
	for (u_int i = 0; i < Rows; i++) {
	    for (u_int j = 0; j < Ceven-1; j++)
		odd[j] -= divrnd(-even[j]-even[j+1], 2);
	    if (Ceven == Codd)
		odd[Codd-1] += even[Ceven-1];
	    even += Ceven;
	    odd += Codd;
	}
}


void LiftChannelR::Lift_L1R1_FR_1_1_1_4(void) const
{
    PixType *even = Even;
    PixType *odd = Odd;

    if (!Parity)
	for (u_int i = 0; i < Rows; i++) {
	    even[0] += divrnd(odd[0], 2);
	    for (u_int j = 1; j < Codd; j++)
		even[j] += divrnd(odd[j-1]+odd[j], 4);
	    if (Ceven != Codd)
		even[Ceven-1] += divrnd(odd[Codd-1], 2);
	    even += Ceven;
	    odd += Codd;
	}
    else
	for (u_int i = 0; i < Rows; i++) {
	    for (u_int j = 0; j < Ceven-1; j++)
		odd[j] += divrnd(even[j]+even[j+1], 4);
	    if (Ceven == Codd)
		odd[Codd-1] += divrnd(even[Ceven-1], 2);
	    even += Ceven;
	    odd += Codd;
	}
}

void LiftChannelR::ILift_L1R1_FR_1_1_1_4(void) const
{
    PixType *even = Even;
    PixType *odd = Odd;

    if (!Parity)
	for (u_int i = 0; i < Rows; i++) {
	    even[0] -= divrnd(odd[0], 2);
	    for (u_int j = 1; j < Codd; j++)
		even[j] -= divrnd(odd[j-1]+odd[j], 4);
	    if (Ceven != Codd)
		even[Ceven-1] -= divrnd(odd[Codd-1], 2);
	    even += Ceven;
	    odd += Codd;
	}
    else
	for (u_int i = 0; i < Rows; i++) {
	    for (u_int j = 0; j < Ceven-1; j++)
		odd[j] -= divrnd(even[j]+even[j+1], 4);
	    if (Ceven == Codd)
		odd[Codd-1] -= divrnd(even[Ceven-1], 2);
	    even += Ceven;
	    odd += Codd;
	}
}


void LiftChannelR::Lift_L1R1_FR_0_m1_0_1(void) const
{
    PixType *even = Even;
    PixType *odd = Odd;

    if (Parity)
	for (u_int i = 0; i < Rows; i++) {
	    even[0] -= odd[0];
	    for (u_int j = 1; j < Codd; j++)
		even[j] -= odd[j-1];
	    if (Ceven != Codd)
		even[Ceven-1] -= odd[Codd-1];
	    even += Ceven;
	    odd += Codd;
	}
    else
	for (u_int i = 0; i < Rows; i++) {
	    for (u_int j = 0; j < Ceven-1; j++)
		odd[j] -= even[j];
	    if (Ceven == Codd)
		odd[Codd-1] -= even[Ceven-1];
	    even += Ceven;
	    odd += Codd;
	}
}

void LiftChannelR::ILift_L1R1_FR_0_m1_0_1(void) const
{
    PixType *even = Even;
    PixType *odd = Odd;

    if (Parity)
	for (u_int i = 0; i < Rows; i++) {
	    even[0] += odd[0];
	    for (u_int j = 1; j < Codd; j++)
		even[j] += odd[j-1];
	    if (Ceven != Codd)
		even[Ceven-1] += odd[Codd-1];
	    even += Ceven;
	    odd += Codd;
	}
    else
	for (u_int i = 0; i < Rows; i++) {
	    for (u_int j = 0; j < Ceven-1; j++)
		odd[j] += even[j];
	    if (Ceven == Codd)
		odd[Codd-1] += even[Ceven-1];
	    even += Ceven;
	    odd += Codd;
	}
}


void LiftChannelR::Lift_L1R1_FR_1_0_1_2(void) const
{
    PixType *even = Even;
    PixType *odd = Odd;

    if (!Parity)
	for (u_int i = 0; i < Rows; i++) {
	    even[0] += divrnd(odd[0], 2);
	    for (u_int j = 1; j < Codd; j++)
		even[j] += divrnd(odd[j], 2);
	    if (Ceven != Codd)
		even[Ceven-1] += divrnd(odd[Codd-1], 2);
	    even += Ceven;
	    odd += Codd;
	}
    else
	for (u_int i = 0; i < Rows; i++) {
	    for (u_int j = 0; j < Ceven-1; j++)
		odd[j] += divrnd(even[j+1], 2);
	    if (Ceven == Codd)
		odd[Codd-1] += divrnd(even[Ceven-1], 2);
	    even += Ceven;
	    odd += Codd;
	}
}

void LiftChannelR::ILift_L1R1_FR_1_0_1_2(void) const
{
    PixType *even = Even;
    PixType *odd = Odd;

    if (!Parity)
	for (u_int i = 0; i < Rows; i++) {
	    even[0] -= divrnd(odd[0], 2);
	    for (u_int j = 1; j < Codd; j++)
		even[j] -= divrnd(odd[j], 2);
	    if (Ceven != Codd)
		even[Ceven-1] -= divrnd(odd[Codd-1], 2);
	    even += Ceven;
	    odd += Codd;
	}
    else
	for (u_int i = 0; i < Rows; i++) {
	    for (u_int j = 0; j < Ceven-1; j++)
		odd[j] -= divrnd(even[j+1], 2);
	    if (Ceven == Codd)
		odd[Codd-1] -= divrnd(even[Ceven-1], 2);
	    even += Ceven;
	    odd += Codd;
	}
}


// ----------------------------------------------------------------------------
//
//	Integer Lifting Operations on the Columns of NTChannels
//
// ----------------------------------------------------------------------------


    //  Lifting Steps (Full Rounding)

void LiftChannelC::Lift_L1R1_FR(int primal, const s16 b[2], const u16 a) const
{
    PixType *even = Even;
    PixType *odd = Odd;

    if (primal ^ Parity) {
	for (u_int i = 0; i < Cols; i++)
	    even[i] += divrnd((b[0]+b[1])*odd[i], a);
	even += Cols;
	odd += Cols;
	for (u_int i = 1; i < Rodd; i++) {
	    for (u_int j = 0; j < Cols; j++)
		even[j] += divrnd(b[0]*odd[int(j-Cols)]+b[1]*odd[j], a);
	    even += Cols;
	    odd += Cols;
	}
	if (Reven != Rodd)
	    for (u_int i = 0; i < Cols; i++)
		even[i] += divrnd((b[0]+b[1])*odd[int(i-Cols)], a);
    } else {
	for (u_int i = 0; i < Reven-1; i++) {
	    for (u_int j = 0; j < Cols; j++)
		odd[j] += divrnd(b[0]*even[j]+b[1]*even[j+Cols], a);
	    even += Cols;
	    odd += Cols;
	}
	if (Reven == Rodd)
	    for (u_int j = 0; j < Cols; j++)
		odd[j] += divrnd((b[0]+b[1])*even[j], a);
    }
}

void LiftChannelC::ILift_L1R1_FR(int primal, const s16 b[2], const u16 a) const
{
    PixType *even = Even;
    PixType *odd = Odd;

    if (primal ^ Parity) {
	for (u_int i = 0; i < Cols; i++)
	    even[i] -= divrnd((b[0]+b[1])*odd[i], a);
	even += Cols;
	odd += Cols;
	for (u_int i = 1; i < Rodd; i++) {
	    for (u_int j = 0; j < Cols; j++)
		even[j] -= divrnd(b[0]*odd[int(j-Cols)]+b[1]*odd[j], a);
	    even += Cols;
	    odd += Cols;
	}
	if (Reven != Rodd)
	    for (u_int i = 0; i < Cols; i++)
		even[i] -= divrnd((b[0]+b[1])*odd[int(i-Cols)], a);
    } else {
	for (u_int i = 0; i < Reven-1; i++) {
	    for (u_int j = 0; j < Cols; j++)
		odd[j] -= divrnd(b[0]*even[j]+b[1]*even[j+Cols], a);
	    even += Cols;
	    odd += Cols;
	}
	if (Reven == Rodd)
	    for (u_int j = 0; j < Cols; j++)
		odd[j] -= divrnd((b[0]+b[1])*even[j], a);
    }
}

void LiftChannelC::Lift_L2R2_FR(int primal, const s16 b[4], const u16 a) const
{
    PixType *src, *dst;
    u_int srclen, dstlen;
    int equal_idx_left;
    int (*clip)(int idx, u_int len);

    if (primal ^ Parity) {
	src = Odd;
	srclen = Rodd;
	dst = Even;
	dstlen = Reven;
	equal_idx_left = 0;
	clip = Reven == Rodd ? clip_21 : clip_22;
    } else {
	src = Even;
	srclen = Reven;
	dst = Odd;
	dstlen = Rodd;
	equal_idx_left = 1;
	clip = Reven == Rodd ? clip_12 : clip_11;
    }
    for (u_int i = 0; i < dstlen; i++) {
	u_int il2 = clip(equal_idx_left+i-2, srclen)*Cols;
	u_int il1 = clip(equal_idx_left+i-1, srclen)*Cols;
	u_int ir1 = clip(equal_idx_left+i, srclen)*Cols;
	u_int ir2 = clip(equal_idx_left+i+1, srclen)*Cols;
	for (u_int j = 0; j < Cols; j++)
	    dst[j] += divrnd(b[0]*src[il2+j]+
			     b[1]*src[il1+j]+
			     b[2]*src[ir1+j]+
			     b[3]*src[ir2+j], a);
	dst += Cols;
    }
}

void LiftChannelC::ILift_L2R2_FR(int primal, const s16 b[4], const u16 a) const
{
    PixType *src, *dst;
    u_int srclen, dstlen;
    int equal_idx_left;
    int (*clip)(int idx, u_int len);

    if (primal ^ Parity) {
	src = Odd;
	srclen = Rodd;
	dst = Even;
	dstlen = Reven;
	equal_idx_left = 0;
	clip = Reven == Rodd ? clip_21 : clip_22;
    } else {
	src = Even;
	srclen = Reven;
	dst = Odd;
	dstlen = Rodd;
	equal_idx_left = 1;
	clip = Reven == Rodd ? clip_12 : clip_11;
    }
    for (u_int i = 0; i < dstlen; i++) {
	u_int il2 = clip(equal_idx_left+i-2, srclen)*Cols;
	u_int il1 = clip(equal_idx_left+i-1, srclen)*Cols;
	u_int ir1 = clip(equal_idx_left+i, srclen)*Cols;
	u_int ir2 = clip(equal_idx_left+i+1, srclen)*Cols;
	for (u_int j = 0; j < Cols; j++)
	    dst[j] -= divrnd(b[0]*src[il2+j]+
			     b[1]*src[il1+j]+
			     b[2]*src[ir1+j]+
			     b[3]*src[ir2+j], a);
	dst += Cols;
    }
}

void LiftChannelC::Lift_L3R3_FR(int primal, const s16 b[6], const u16 a) const
{
    PixType *src, *dst;
    u_int srclen, dstlen;
    int equal_idx_left;
    int (*clip)(int idx, u_int len);

    if (primal ^ Parity) {
	src = Odd;
	srclen = Rodd;
	dst = Even;
	dstlen = Reven;
	equal_idx_left = 0;
	clip = Reven == Rodd ? clip_21 : clip_22;
    } else {
	src = Even;
	srclen = Reven;
	dst = Odd;
	dstlen = Rodd;
	equal_idx_left = 1;
	clip = Reven == Rodd ? clip_12 : clip_11;
    }
    for (u_int i = 0; i < dstlen; i++) {
	u_int il3 = clip(equal_idx_left+i-3, srclen)*Cols;
	u_int il2 = clip(equal_idx_left+i-2, srclen)*Cols;
	u_int il1 = clip(equal_idx_left+i-1, srclen)*Cols;
	u_int ir1 = clip(equal_idx_left+i, srclen)*Cols;
	u_int ir2 = clip(equal_idx_left+i+1, srclen)*Cols;
	u_int ir3 = clip(equal_idx_left+i+2, srclen)*Cols;
	for (u_int j = 0; j < Cols; j++)
	    dst[j] += divrnd(b[0]*src[il3+j]+
			     b[1]*src[il2+j]+
			     b[2]*src[il1+j]+
			     b[3]*src[ir1+j]+
			     b[4]*src[ir2+j]+
			     b[5]*src[ir3+j], a);
	dst += Cols;
    }
}

void LiftChannelC::ILift_L3R3_FR(int primal, const s16 b[6], const u16 a) const
{
    PixType *src, *dst;
    u_int srclen, dstlen;
    int equal_idx_left;
    int (*clip)(int idx, u_int len);

    if (primal ^ Parity) {
	src = Odd;
	srclen = Rodd;
	dst = Even;
	dstlen = Reven;
	equal_idx_left = 0;
	clip = Reven == Rodd ? clip_21 : clip_22;
    } else {
	src = Even;
	srclen = Reven;
	dst = Odd;
	dstlen = Rodd;
	equal_idx_left = 1;
	clip = Reven == Rodd ? clip_12 : clip_11;
    }
    for (u_int i = 0; i < dstlen; i++) {
	u_int il3 = clip(equal_idx_left+i-3, srclen)*Cols;
	u_int il2 = clip(equal_idx_left+i-2, srclen)*Cols;
	u_int il1 = clip(equal_idx_left+i-1, srclen)*Cols;
	u_int ir1 = clip(equal_idx_left+i, srclen)*Cols;
	u_int ir2 = clip(equal_idx_left+i+1, srclen)*Cols;
	u_int ir3 = clip(equal_idx_left+i+2, srclen)*Cols;
	for (u_int j = 0; j < Cols; j++)
	    dst[j] -= divrnd(b[0]*src[il3+j]+
			     b[1]*src[il2+j]+
			     b[2]*src[il1+j]+
			     b[3]*src[ir1+j]+
			     b[4]*src[ir2+j]+
			     b[5]*src[ir3+j], a);
	dst += Cols;
    }
}


// ----------------------------------------------------------------------------


    //  Lifting Steps (No Rounding)

void LiftChannelC::Lift_L1R1_NR(int primal, const s16 b[2], const u16 a) const
{
    PixType *even = Even;
    PixType *odd = Odd;

    if (primal ^ Parity) {
	for (u_int i = 0; i < Cols; i++)
	    even[i] = a*even[i]+(b[0]+b[1])*odd[i];
	even += Cols;
	odd += Cols;
	for (u_int i = 1; i < Rodd; i++) {
	    for (u_int j = 0; j < Cols; j++)
		even[j] = a*even[j]+b[0]*odd[int(j-Cols)]+b[1]*odd[j];
	    even += Cols;
	    odd += Cols;
	}
	if (Reven != Rodd)
	    for (u_int i = 0; i < Cols; i++)
		even[i] = a*even[i]+(b[0]+b[1])*odd[int(i-Cols)];
    } else {
	for (u_int i = 0; i < Reven-1; i++) {
	    for (u_int j = 0; j < Cols; j++)
		odd[j] = a*odd[j]+b[0]*even[j]+b[1]*even[j+Cols];
	    even += Cols;
	    odd += Cols;
	}
	if (Reven == Rodd)
	    for (u_int j = 0; j < Cols; j++)
		odd[j] = a*odd[j]+(b[0]+b[1])*even[j];
    }
}

void LiftChannelC::ILift_L1R1_NR(int primal, const s16 b[2], const u16 a) const
{
    PixType *even = Even;
    PixType *odd = Odd;

    if (primal ^ Parity) {
	for (u_int i = 0; i < Cols; i++)
	    even[i] = (even[i]-(b[0]+b[1])*odd[i])/a;
	even += Cols;
	odd += Cols;
	for (u_int i = 1; i < Rodd; i++) {
	    for (u_int j = 0; j < Cols; j++)
		even[j] = (even[j]-b[0]*odd[int(j-Cols)]-b[1]*odd[j])/a;
	    even += Cols;
	    odd += Cols;
	}
	if (Reven != Rodd)
	    for (u_int i = 0; i < Cols; i++)
		even[i] = (even[i]-(b[0]+b[1])*odd[int(i-Cols)])/a;
    } else {
	for (u_int i = 0; i < Reven-1; i++) {
	    for (u_int j = 0; j < Cols; j++)
		odd[j] = (odd[j]-b[0]*even[j]-b[1]*even[j+Cols])/a;
	    even += Cols;
	    odd += Cols;
	}
	if (Reven == Rodd)
	    for (u_int j = 0; j < Cols; j++)
		odd[j] = (odd[j]-(b[0]+b[1])*even[j])/a;
    }
}

void LiftChannelC::Lift_L2R2_NR(int primal, const s16 b[4], const u16 a) const
{
    PixType *src, *dst;
    u_int srclen, dstlen;
    int equal_idx_left;
    int (*clip)(int idx, u_int len);

    if (primal ^ Parity) {
	src = Odd;
	srclen = Rodd;
	dst = Even;
	dstlen = Reven;
	equal_idx_left = 0;
	clip = Reven == Rodd ? clip_21 : clip_22;
    } else {
	src = Even;
	srclen = Reven;
	dst = Odd;
	dstlen = Rodd;
	equal_idx_left = 1;
	clip = Reven == Rodd ? clip_12 : clip_11;
    }
    for (u_int i = 0; i < dstlen; i++) {
	u_int il2 = clip(equal_idx_left+i-2, srclen)*Cols;
	u_int il1 = clip(equal_idx_left+i-1, srclen)*Cols;
	u_int ir1 = clip(equal_idx_left+i, srclen)*Cols;
	u_int ir2 = clip(equal_idx_left+i+1, srclen)*Cols;
	for (u_int j = 0; j < Cols; j++)
	    dst[j] = a*dst[j]+b[0]*src[il2+j]+
			      b[1]*src[il1+j]+
			      b[2]*src[ir1+j]+
			      b[3]*src[ir2+j];
	dst += Cols;
    }
}

void LiftChannelC::ILift_L2R2_NR(int primal, const s16 b[4], const u16 a) const
{
    PixType *src, *dst;
    u_int srclen, dstlen;
    int equal_idx_left;
    int (*clip)(int idx, u_int len);

    if (primal ^ Parity) {
	src = Odd;
	srclen = Rodd;
	dst = Even;
	dstlen = Reven;
	equal_idx_left = 0;
	clip = Reven == Rodd ? clip_21 : clip_22;
    } else {
	src = Even;
	srclen = Reven;
	dst = Odd;
	dstlen = Rodd;
	equal_idx_left = 1;
	clip = Reven == Rodd ? clip_12 : clip_11;
    }
    for (u_int i = 0; i < dstlen; i++) {
	u_int il2 = clip(equal_idx_left+i-2, srclen)*Cols;
	u_int il1 = clip(equal_idx_left+i-1, srclen)*Cols;
	u_int ir1 = clip(equal_idx_left+i, srclen)*Cols;
	u_int ir2 = clip(equal_idx_left+i+1, srclen)*Cols;
	for (u_int j = 0; j < Cols; j++)
	    dst[j] = (dst[j]-(b[0]*src[il2+j]+
			      b[1]*src[il1+j]+
			      b[2]*src[ir1+j]+
			      b[3]*src[ir2+j]))/a;
	dst += Cols;
    }
}

void LiftChannelC::Lift_L3R3_NR(int primal, const s16 b[6], const u16 a) const
{
    PixType *src, *dst;
    u_int srclen, dstlen;
    int equal_idx_left;
    int (*clip)(int idx, u_int len);

    if (primal ^ Parity) {
	src = Odd;
	srclen = Rodd;
	dst = Even;
	dstlen = Reven;
	equal_idx_left = 0;
	clip = Reven == Rodd ? clip_21 : clip_22;
    } else {
	src = Even;
	srclen = Reven;
	dst = Odd;
	dstlen = Rodd;
	equal_idx_left = 1;
	clip = Reven == Rodd ? clip_12 : clip_11;
    }
    for (u_int i = 0; i < dstlen; i++) {
	u_int il3 = clip(equal_idx_left+i-3, srclen)*Cols;
	u_int il2 = clip(equal_idx_left+i-2, srclen)*Cols;
	u_int il1 = clip(equal_idx_left+i-1, srclen)*Cols;
	u_int ir1 = clip(equal_idx_left+i, srclen)*Cols;
	u_int ir2 = clip(equal_idx_left+i+1, srclen)*Cols;
	u_int ir3 = clip(equal_idx_left+i+2, srclen)*Cols;
	for (u_int j = 0; j < Cols; j++)
	    dst[j] = a*dst[j]+b[0]*src[il3+j]+
			      b[1]*src[il2+j]+
			      b[2]*src[il1+j]+
			      b[3]*src[ir1+j]+
			      b[4]*src[ir2+j]+
			      b[5]*src[ir3+j];
	dst += Cols;
    }
}

void LiftChannelC::ILift_L3R3_NR(int primal, const s16 b[6], const u16 a) const
{
    PixType *src, *dst;
    u_int srclen, dstlen;
    int equal_idx_left;
    int (*clip)(int idx, u_int len);

    if (primal ^ Parity) {
	src = Odd;
	srclen = Rodd;
	dst = Even;
	dstlen = Reven;
	equal_idx_left = 0;
	clip = Reven == Rodd ? clip_21 : clip_22;
    } else {
	src = Even;
	srclen = Reven;
	dst = Odd;
	dstlen = Rodd;
	equal_idx_left = 1;
	clip = Reven == Rodd ? clip_12 : clip_11;
    }
    for (u_int i = 0; i < dstlen; i++) {
	u_int il3 = clip(equal_idx_left+i-3, srclen)*Cols;
	u_int il2 = clip(equal_idx_left+i-2, srclen)*Cols;
	u_int il1 = clip(equal_idx_left+i-1, srclen)*Cols;
	u_int ir1 = clip(equal_idx_left+i, srclen)*Cols;
	u_int ir2 = clip(equal_idx_left+i+1, srclen)*Cols;
	u_int ir3 = clip(equal_idx_left+i+2, srclen)*Cols;
	for (u_int j = 0; j < Cols; j++)
	    dst[j] = (a*dst[j]-(b[0]*src[il3+j]+
				b[1]*src[il2+j]+
				b[2]*src[il1+j]+
				b[3]*src[ir1+j]+
				b[4]*src[ir2+j]+
				b[5]*src[ir3+j]))/a;
	dst += Cols;
    }
}


// ----------------------------------------------------------------------------


    //  Lifting Steps (Mixed)

void LiftChannelC::Lift_L1R1_MX(int primal, const s16 b[2], const u16 a1,
				const u16 a2) const
{
    PixType *even = Even;
    PixType *odd = Odd;

    if (primal ^ Parity) {
	for (u_int i = 0; i < Cols; i++)
	    even[i] = a1*even[i]+divrnd((b[0]+b[1])*odd[i], a2);
	even += Cols;
	odd += Cols;
	for (u_int i = 1; i < Rodd; i++) {
	    for (u_int j = 0; j < Cols; j++)
		even[j] = a1*even[j]+divrnd(b[0]*odd[int(j-Cols)]+b[1]*odd[j],
					    a2);
	    even += Cols;
	    odd += Cols;
	}
	if (Reven != Rodd)
	    for (u_int i = 0; i < Cols; i++)
		even[i] = a1*even[i]+divrnd((b[0]+b[1])*odd[int(i-Cols)], a2);
    } else {
	for (u_int i = 0; i < Reven-1; i++) {
	    for (u_int j = 0; j < Cols; j++)
		odd[j] = a1*odd[j]+divrnd(b[0]*even[j]+b[1]*even[j+Cols], a2);
	    even += Cols;
	    odd += Cols;
	}
	if (Reven == Rodd)
	    for (u_int j = 0; j < Cols; j++)
		odd[j] = a1*odd[j]+divrnd((b[0]+b[1])*even[j], a2);
    }
}

void LiftChannelC::ILift_L1R1_MX(int primal, const s16 b[2], const u16 a1,
				 const u16 a2) const
{
    PixType *even = Even;
    PixType *odd = Odd;

    if (primal ^ Parity) {
	for (u_int i = 0; i < Cols; i++)
	    even[i] = (even[i]-divrnd((b[0]+b[1])*odd[i], a2))/a1;
	even += Cols;
	odd += Cols;
	for (u_int i = 1; i < Rodd; i++) {
	    for (u_int j = 0; j < Cols; j++)
		even[j] = (even[j]-divrnd(b[0]*odd[int(j-Cols)]+b[1]*odd[j],
					  a2))/a1;
	    even += Cols;
	    odd += Cols;
	}
	if (Reven != Rodd)
	    for (u_int i = 0; i < Cols; i++)
		even[i] = (even[i]-divrnd((b[0]+b[1])*odd[int(i-Cols)], a2))/
			  a1;
    } else {
	for (u_int i = 0; i < Reven-1; i++) {
	    for (u_int j = 0; j < Cols; j++)
		odd[j] = (odd[j]-divrnd(b[0]*even[j]+b[1]*even[j+Cols], a2))/a1;
	    even += Cols;
	    odd += Cols;
	}
	if (Reven == Rodd)
	    for (u_int j = 0; j < Cols; j++)
		odd[j] = (odd[j]-divrnd((b[0]+b[1])*even[j], a2))/a1;
    }
}

void LiftChannelC::Lift_L2R2_MX(int primal, const s16 b[4], const u16 a1,
				const u16 a2) const
{
    PixType *src, *dst;
    u_int srclen, dstlen;
    int equal_idx_left;
    int (*clip)(int idx, u_int len);

    if (primal ^ Parity) {
	src = Odd;
	srclen = Rodd;
	dst = Even;
	dstlen = Reven;
	equal_idx_left = 0;
	clip = Reven == Rodd ? clip_21 : clip_22;
    } else {
	src = Even;
	srclen = Reven;
	dst = Odd;
	dstlen = Rodd;
	equal_idx_left = 1;
	clip = Reven == Rodd ? clip_12 : clip_11;
    }
    for (u_int i = 0; i < dstlen; i++) {
	u_int il2 = clip(equal_idx_left+i-2, srclen)*Cols;
	u_int il1 = clip(equal_idx_left+i-1, srclen)*Cols;
	u_int ir1 = clip(equal_idx_left+i, srclen)*Cols;
	u_int ir2 = clip(equal_idx_left+i+1, srclen)*Cols;
	for (u_int j = 0; j < Cols; j++)
	    dst[j] = a1*dst[j]+divrnd(b[0]*src[il2+j]+
				      b[1]*src[il1+j]+
				      b[2]*src[ir1+j]+
				      b[3]*src[ir2+j], a2);
	dst += Cols;
    }
}

void LiftChannelC::ILift_L2R2_MX(int primal, const s16 b[4], const u16 a1,
				 const u16 a2) const
{
    PixType *src, *dst;
    u_int srclen, dstlen;
    int equal_idx_left;
    int (*clip)(int idx, u_int len);

    if (primal ^ Parity) {
	src = Odd;
	srclen = Rodd;
	dst = Even;
	dstlen = Reven;
	equal_idx_left = 0;
	clip = Reven == Rodd ? clip_21 : clip_22;
    } else {
	src = Even;
	srclen = Reven;
	dst = Odd;
	dstlen = Rodd;
	equal_idx_left = 1;
	clip = Reven == Rodd ? clip_12 : clip_11;
    }
    for (u_int i = 0; i < dstlen; i++) {
	u_int il2 = clip(equal_idx_left+i-2, srclen)*Cols;
	u_int il1 = clip(equal_idx_left+i-1, srclen)*Cols;
	u_int ir1 = clip(equal_idx_left+i, srclen)*Cols;
	u_int ir2 = clip(equal_idx_left+i+1, srclen)*Cols;
	for (u_int j = 0; j < Cols; j++)
	    dst[j] = (dst[j]-divrnd(b[0]*src[il2+j]+
				    b[1]*src[il1+j]+
				    b[2]*src[ir1+j]+
				    b[3]*src[ir2+j], a2))/a1;
	dst += Cols;
    }
}

void LiftChannelC::Lift_L3R3_MX(int primal, const s16 b[6], const u16 a1,
				const u16 a2) const
{
    PixType *src, *dst;
    u_int srclen, dstlen;
    int equal_idx_left;
    int (*clip)(int idx, u_int len);

    if (primal ^ Parity) {
	src = Odd;
	srclen = Rodd;
	dst = Even;
	dstlen = Reven;
	equal_idx_left = 0;
	clip = Reven == Rodd ? clip_21 : clip_22;
    } else {
	src = Even;
	srclen = Reven;
	dst = Odd;
	dstlen = Rodd;
	equal_idx_left = 1;
	clip = Reven == Rodd ? clip_12 : clip_11;
    }
    for (u_int i = 0; i < dstlen; i++) {
	u_int il3 = clip(equal_idx_left+i-3, srclen)*Cols;
	u_int il2 = clip(equal_idx_left+i-2, srclen)*Cols;
	u_int il1 = clip(equal_idx_left+i-1, srclen)*Cols;
	u_int ir1 = clip(equal_idx_left+i, srclen)*Cols;
	u_int ir2 = clip(equal_idx_left+i+1, srclen)*Cols;
	u_int ir3 = clip(equal_idx_left+i+2, srclen)*Cols;
	for (u_int j = 0; j < Cols; j++)
	    dst[j] = a1*dst[j]+divrnd(b[0]*src[il3+j]+
				      b[1]*src[il2+j]+
				      b[2]*src[il1+j]+
				      b[3]*src[ir1+j]+
				      b[4]*src[ir2+j]+
				      b[5]*src[ir3+j], a2);
	dst += Cols;
    }
}

void LiftChannelC::ILift_L3R3_MX(int primal, const s16 b[6], const u16 a1,
				 const u16 a2) const
{
    PixType *src, *dst;
    u_int srclen, dstlen;
    int equal_idx_left;
    int (*clip)(int idx, u_int len);

    if (primal ^ Parity) {
	src = Odd;
	srclen = Rodd;
	dst = Even;
	dstlen = Reven;
	equal_idx_left = 0;
	clip = Reven == Rodd ? clip_21 : clip_22;
    } else {
	src = Even;
	srclen = Reven;
	dst = Odd;
	dstlen = Rodd;
	equal_idx_left = 1;
	clip = Reven == Rodd ? clip_12 : clip_11;
    }
    for (u_int i = 0; i < dstlen; i++) {
	u_int il3 = clip(equal_idx_left+i-3, srclen)*Cols;
	u_int il2 = clip(equal_idx_left+i-2, srclen)*Cols;
	u_int il1 = clip(equal_idx_left+i-1, srclen)*Cols;
	u_int ir1 = clip(equal_idx_left+i, srclen)*Cols;
	u_int ir2 = clip(equal_idx_left+i+1, srclen)*Cols;
	u_int ir3 = clip(equal_idx_left+i+2, srclen)*Cols;
	for (u_int j = 0; j < Cols; j++)
	    dst[j] = (dst[j]-divrnd(b[0]*src[il3+j]+
				    b[1]*src[il2+j]+
				    b[2]*src[il1+j]+
				    b[3]*src[ir1+j]+
				    b[4]*src[ir2+j]+
				    b[5]*src[ir3+j], a2))/a1;
	dst += Cols;
    }
}


    //  Optimized functions for specific wavelet transforms

void LiftChannelC::Lift_L1R1_NR_0_m1_m1_2(void) const
{
    PixType *even = Even;
    PixType *odd = Odd;

    if (Parity) {
	for (u_int i = 0; i < Cols; i++)
	    even[i] = (even[i]-odd[i])<<1;
	even += Cols;
	odd += Cols;
	for (u_int i = 1; i < Rodd; i++) {
	    for (u_int j = 0; j < Cols; j++)
		even[j] = (even[j]<<1)-odd[int(j-Cols)]-odd[j];
	    even += Cols;
	    odd += Cols;
	}
	if (Reven != Rodd)
	    for (u_int i = 0; i < Cols; i++)
		even[i] = (even[i]-odd[int(i-Cols)])<<1;
    } else {
	for (u_int i = 0; i < Reven-1; i++) {
	    for (u_int j = 0; j < Cols; j++)
		odd[j] = (odd[j]<<1)-even[j]-even[j+Cols];
	    even += Cols;
	    odd += Cols;
	}
	if (Reven == Rodd)
	    for (u_int j = 0; j < Cols; j++)
		odd[j] = (odd[j]-even[j])<<1;
    }
}

void LiftChannelC::ILift_L1R1_NR_0_m1_m1_2(void) const
{
    PixType *even = Even;
    PixType *odd = Odd;

    if (Parity) {
	for (u_int i = 0; i < Cols; i++)
	    even[i] = (even[i]>>1)+odd[i];
	even += Cols;
	odd += Cols;
	for (u_int i = 1; i < Rodd; i++) {
	    for (u_int j = 0; j < Cols; j++)
		even[j] = (even[j]+odd[int(j-Cols)]+odd[j])>>1;
	    even += Cols;
	    odd += Cols;
	}
	if (Reven != Rodd)
	    for (u_int i = 0; i < Cols; i++)
		even[i] = (even[i]>>1)+odd[int(i-Cols)];
    } else {
	for (u_int i = 0; i < Reven-1; i++) {
	    for (u_int j = 0; j < Cols; j++)
		odd[j] = (odd[j]+even[j]+even[j+Cols])>>1;
	    even += Cols;
	    odd += Cols;
	}
	if (Reven == Rodd)
	    for (u_int j = 0; j < Cols; j++)
		odd[j] = (odd[j]>>1)+even[j];
    }
}

void LiftChannelC::Lift_L1R1_FR_1_1_1_8(void) const
{
    PixType *even = Even;
    PixType *odd = Odd;

    if (!Parity) {
	for (u_int i = 0; i < Cols; i++)
	    even[i] += divrnd(odd[i], 4);
	even += Cols;
	odd += Cols;
	for (u_int i = 1; i < Rodd; i++) {
	    for (u_int j = 0; j < Cols; j++)
		even[j] += divrnd(odd[int(j-Cols)]+odd[j], 8);
	    even += Cols;
	    odd += Cols;
	}
	if (Reven != Rodd)
	    for (u_int i = 0; i < Cols; i++)
		even[i] += divrnd(odd[int(i-Cols)], 4);
    } else {
	for (u_int i = 0; i < Reven-1; i++) {
	    for (u_int j = 0; j < Cols; j++)
		odd[j] += divrnd(even[j]+even[j+Cols], 8);
	    even += Cols;
	    odd += Cols;
	}
	if (Reven == Rodd)
	    for (u_int j = 0; j < Cols; j++)
		odd[j] += divrnd(even[j], 4);
    }
}

void LiftChannelC::ILift_L1R1_FR_1_1_1_8(void) const
{
    PixType *even = Even;
    PixType *odd = Odd;

    if (!Parity) {
	for (u_int i = 0; i < Cols; i++)
	    even[i] -= divrnd(odd[i], 4);
	even += Cols;
	odd += Cols;
	for (u_int i = 1; i < Rodd; i++) {
	    for (u_int j = 0; j < Cols; j++)
		even[j] -= divrnd(odd[int(j-Cols)]+odd[j], 8);
	    even += Cols;
	    odd += Cols;
	}
	if (Reven != Rodd)
	    for (u_int i = 0; i < Cols; i++)
		even[i] -= divrnd(odd[int(i-Cols)], 4);
    } else {
	for (u_int i = 0; i < Reven-1; i++) {
	    for (u_int j = 0; j < Cols; j++)
		odd[j] -= divrnd(even[j]+even[j+Cols], 8);
	    even += Cols;
	    odd += Cols;
	}
	if (Reven == Rodd)
	    for (u_int j = 0; j < Cols; j++)
		odd[j] -= divrnd(even[j], 4);
    }
}


void LiftChannelC::Lift_L1R1_FR_0_m1_m1_2(void) const
{
    PixType *even = Even;
    PixType *odd = Odd;

    if (Parity) {
	for (u_int i = 0; i < Cols; i++)
	    even[i] -= odd[i];
	even += Cols;
	odd += Cols;
	for (u_int i = 1; i < Rodd; i++) {
	    for (u_int j = 0; j < Cols; j++)
		even[j] += divrnd(-odd[int(j-Cols)]-odd[j], 2);
	    even += Cols;
	    odd += Cols;
	}
	if (Reven != Rodd)
	    for (u_int i = 0; i < Cols; i++)
		even[i] -= odd[int(i-Cols)];
    } else {
	for (u_int i = 0; i < Reven-1; i++) {
	    for (u_int j = 0; j < Cols; j++)
		odd[j] += divrnd(-even[j]-even[j+Cols], 2);
	    even += Cols;
	    odd += Cols;
	}
	if (Reven == Rodd)
	    for (u_int j = 0; j < Cols; j++)
		odd[j] -= even[j];
    }
}

void LiftChannelC::ILift_L1R1_FR_0_m1_m1_2(void) const
{
    PixType *even = Even;
    PixType *odd = Odd;

    if (Parity) {
	for (u_int i = 0; i < Cols; i++)
	    even[i] += odd[i];
	even += Cols;
	odd += Cols;
	for (u_int i = 1; i < Rodd; i++) {
	    for (u_int j = 0; j < Cols; j++)
		even[j] -= divrnd(-odd[int(j-Cols)]-odd[j], 2);
	    even += Cols;
	    odd += Cols;
	}
	if (Reven != Rodd)
	    for (u_int i = 0; i < Cols; i++)
		even[i] += odd[int(i-Cols)];
    } else {
	for (u_int i = 0; i < Reven-1; i++) {
	    for (u_int j = 0; j < Cols; j++)
		odd[j] -= divrnd(-even[j]-even[j+Cols], 2);
	    even += Cols;
	    odd += Cols;
	}
	if (Reven == Rodd)
	    for (u_int j = 0; j < Cols; j++)
		odd[j] += even[j];
    }
}

void LiftChannelC::Lift_L1R1_FR_1_1_1_4(void) const
{
    PixType *even = Even;
    PixType *odd = Odd;

    if (!Parity) {
	for (u_int i = 0; i < Cols; i++)
	    even[i] += divrnd(odd[i], 2);
	even += Cols;
	odd += Cols;
	for (u_int i = 1; i < Rodd; i++) {
	    for (u_int j = 0; j < Cols; j++)
		even[j] += divrnd(odd[int(j-Cols)]+odd[j], 4);
	    even += Cols;
	    odd += Cols;
	}
	if (Reven != Rodd)
	    for (u_int i = 0; i < Cols; i++)
		even[i] += divrnd(odd[int(i-Cols)], 2);
    } else {
	for (u_int i = 0; i < Reven-1; i++) {
	    for (u_int j = 0; j < Cols; j++)
		odd[j] += divrnd(even[j]+even[j+Cols], 4);
	    even += Cols;
	    odd += Cols;
	}
	if (Reven == Rodd)
	    for (u_int j = 0; j < Cols; j++)
		odd[j] += divrnd(even[j], 2);
    }
}

void LiftChannelC::ILift_L1R1_FR_1_1_1_4(void) const
{
    PixType *even = Even;
    PixType *odd = Odd;

    if (!Parity) {
	for (u_int i = 0; i < Cols; i++)
	    even[i] -= divrnd(odd[i], 2);
	even += Cols;
	odd += Cols;
	for (u_int i = 1; i < Rodd; i++) {
	    for (u_int j = 0; j < Cols; j++)
		even[j] -= divrnd(odd[int(j-Cols)]+odd[j], 4);
	    even += Cols;
	    odd += Cols;
	}
	if (Reven != Rodd)
	    for (u_int i = 0; i < Cols; i++)
		even[i] -= divrnd(odd[int(i-Cols)], 2);
    } else {
	for (u_int i = 0; i < Reven-1; i++) {
	    for (u_int j = 0; j < Cols; j++)
		odd[j] -= divrnd(even[j]+even[j+Cols], 4);
	    even += Cols;
	    odd += Cols;
	}
	if (Reven == Rodd)
	    for (u_int j = 0; j < Cols; j++)
		odd[j] -= divrnd(even[j], 2);
    }
}


void LiftChannelC::Lift_L1R1_FR_0_m1_0_1(void) const
{
    PixType *even = Even;
    PixType *odd = Odd;

    if (Parity) {
	for (u_int i = 0; i < Cols; i++)
	    even[i] -= odd[i];
	even += Cols;
	odd += Cols;
	for (u_int i = 1; i < Rodd; i++) {
	    for (u_int j = 0; j < Cols; j++)
		even[j] -= odd[int(j-Cols)];
	    even += Cols;
	    odd += Cols;
	}
	if (Reven != Rodd)
	    for (u_int i = 0; i < Cols; i++)
		even[i] -= odd[int(i-Cols)];
    } else {
	for (u_int i = 0; i < Reven-1; i++) {
	    for (u_int j = 0; j < Cols; j++)
		odd[j] -= even[j];
	    even += Cols;
	    odd += Cols;
	}
	if (Reven == Rodd)
	    for (u_int j = 0; j < Cols; j++)
		odd[j] -= even[j];
    }
}

void LiftChannelC::ILift_L1R1_FR_0_m1_0_1(void) const
{
    PixType *even = Even;
    PixType *odd = Odd;

    if (Parity) {
	for (u_int i = 0; i < Cols; i++)
	    even[i] += odd[i];
	even += Cols;
	odd += Cols;
	for (u_int i = 1; i < Rodd; i++) {
	    for (u_int j = 0; j < Cols; j++)
		even[j] += odd[int(j-Cols)];
	    even += Cols;
	    odd += Cols;
	}
	if (Reven != Rodd)
	    for (u_int i = 0; i < Cols; i++)
		even[i] += odd[int(i-Cols)];
    } else {
	for (u_int i = 0; i < Reven-1; i++) {
	    for (u_int j = 0; j < Cols; j++)
		odd[j] += even[j];
	    even += Cols;
	    odd += Cols;
	}
	if (Reven == Rodd)
	    for (u_int j = 0; j < Cols; j++)
		odd[j] += even[j];
    }
}


void LiftChannelC::Lift_L1R1_FR_1_0_1_2(void) const
{
    PixType *even = Even;
    PixType *odd = Odd;

    if (!Parity) {
	for (u_int i = 0; i < Cols; i++)
	    even[i] += divrnd(odd[i], 2);
	even += Cols;
	odd += Cols;
	for (u_int i = 1; i < Rodd; i++) {
	    for (u_int j = 0; j < Cols; j++)
		even[j] += divrnd(odd[j], 2);
	    even += Cols;
	    odd += Cols;
	}
	if (Reven != Rodd)
	    for (u_int i = 0; i < Cols; i++)
		even[i] += divrnd(odd[int(i-Cols)], 2);
    } else {
	for (u_int i = 0; i < Reven-1; i++) {
	    for (u_int j = 0; j < Cols; j++)
		odd[j] += divrnd(even[j+Cols], 2);
	    even += Cols;
	    odd += Cols;
	}
	if (Reven == Rodd)
	    for (u_int j = 0; j < Cols; j++)
		odd[j] += divrnd(even[j], 2);
    }
}

void LiftChannelC::ILift_L1R1_FR_1_0_1_2(void) const
{
    PixType *even = Even;
    PixType *odd = Odd;

    if (!Parity) {
	for (u_int i = 0; i < Cols; i++)
	    even[i] -= divrnd(odd[i], 2);
	even += Cols;
	odd += Cols;
	for (u_int i = 1; i < Rodd; i++) {
	    for (u_int j = 0; j < Cols; j++)
		even[j] -= divrnd(odd[j], 2);
	    even += Cols;
	    odd += Cols;
	}
	if (Reven != Rodd)
	    for (u_int i = 0; i < Cols; i++)
		even[i] -= divrnd(odd[int(i-Cols)], 2);
    } else {
	for (u_int i = 0; i < Reven-1; i++) {
	    for (u_int j = 0; j < Cols; j++)
		odd[j] -= divrnd(even[j+Cols], 2);
	    even += Cols;
	    odd += Cols;
	}
	if (Reven == Rodd)
	    for (u_int j = 0; j < Cols; j++)
		odd[j] -= divrnd(even[j], 2);
    }
}
