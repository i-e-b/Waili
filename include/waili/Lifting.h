//
//	Integer Lifting Operations
//
//  $Id: Lifting.h,v 4.3.4.1 1999/07/20 16:15:44 geert Exp $
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

#ifndef WAILI_LIFTING_H
#define WAILI_LIFTING_H

#include "Types.h"


class NTChannel;


    //  Integer Lifting Abstract Base Class

class Lifting {
    public:
	virtual ~Lifting();

	// Lifting Steps (Full Rounding)
	virtual void Lift_L1R1_FR(int primal, const s16 b[2], const u16 a)
	    const = 0;
	virtual void ILift_L1R1_FR(int primal, const s16 b[2], const u16 a)
	    const = 0;
	virtual void Lift_L2R2_FR(int primal, const s16 b[4], const u16 a)
	    const = 0;
	virtual void ILift_L2R2_FR(int primal, const s16 b[4], const u16 a)
	    const = 0;
	virtual void Lift_L3R3_FR(int primal, const s16 b[6], const u16 a)
	    const = 0;
	virtual void ILift_L3R3_FR(int primal, const s16 b[6], const u16 a)
	    const = 0;

	// Lifting Steps (No Rounding)
	virtual void Lift_L1R1_NR(int primal, const s16 b[2], const u16 a)
	    const = 0;
	virtual void ILift_L1R1_NR(int primal, const s16 b[2], const u16 a)
	    const = 0;
	virtual void Lift_L2R2_NR(int primal, const s16 b[4], const u16 a)
	    const = 0;
	virtual void ILift_L2R2_NR(int primal, const s16 b[4], const u16 a)
	    const = 0;
	virtual void Lift_L3R3_NR(int primal, const s16 b[6], const u16 a)
	    const = 0;
	virtual void ILift_L3R3_NR(int primal, const s16 b[6], const u16 a)
	    const = 0;

	// Lifting Steps (Mixed)
	virtual void Lift_L1R1_MX(int primal, const s16 b[2], const u16 a1,
	    const u16 a2) const = 0;
	virtual void ILift_L1R1_MX(int primal, const s16 b[2], const u16 a1,
	    const u16 a2) const = 0;
	virtual void Lift_L2R2_MX(int primal, const s16 b[4], const u16 a1,
	    const u16 a2) const = 0;
	virtual void ILift_L2R2_MX(int primal, const s16 b[4], const u16 a1,
	    const u16 a2) const = 0;
	virtual void Lift_L3R3_MX(int primal, const s16 b[6], const u16 a1,
	    const u16 a2) const = 0;
	virtual void ILift_L3R3_MX(int primal, const s16 b[6], const u16 a1,
	    const u16 a2) const = 0;

	// Optimized functions for specific wavelet transforms
	virtual void Lift_L1R1_NR_0_m1_m1_2(void) const = 0;
	virtual void ILift_L1R1_NR_0_m1_m1_2(void) const = 0;
	virtual void Lift_L1R1_FR_1_1_1_8(void) const = 0;
	virtual void ILift_L1R1_FR_1_1_1_8(void) const = 0;
	virtual void Lift_L1R1_FR_0_m1_m1_2(void) const = 0;
	virtual void ILift_L1R1_FR_0_m1_m1_2(void) const = 0;
	virtual void Lift_L1R1_FR_1_1_1_4(void) const = 0;
	virtual void ILift_L1R1_FR_1_1_1_4(void) const = 0;
	virtual void Lift_L1R1_FR_0_m1_0_1(void) const = 0;
	virtual void ILift_L1R1_FR_0_m1_0_1(void) const = 0;
	virtual void Lift_L1R1_FR_1_0_1_2(void) const = 0;
	virtual void ILift_L1R1_FR_1_0_1_2(void) const = 0;

    private:
	static const char *rcsid;
};


// ----------------------------------------------------------------------------


    //  Derived Classes


    //  Integer Lifting Operations on the Rows of NTChannels

class LiftChannelR : public Lifting {
    public:
	LiftChannelR(NTChannel *lowpass, NTChannel *highpass);
	virtual ~LiftChannelR();

	// Lifting Steps (Full Rounding)
	virtual void Lift_L1R1_FR(int primal, const s16 b[2], const u16 a)
	    const;
	virtual void ILift_L1R1_FR(int primal, const s16 b[2], const u16 a)
	    const;
	virtual void Lift_L2R2_FR(int primal, const s16 b[4], const u16 a)
	    const;
	virtual void ILift_L2R2_FR(int primal, const s16 b[4], const u16 a)
	    const;
	virtual void Lift_L3R3_FR(int primal, const s16 b[6], const u16 a)
	    const;
	virtual void ILift_L3R3_FR(int primal, const s16 b[6], const u16 a)
	    const;

	// Lifting Steps (No Rounding)
	virtual void Lift_L1R1_NR(int primal, const s16 b[2], const u16 a)
	    const;
	virtual void ILift_L1R1_NR(int primal, const s16 b[2], const u16 a)
	    const;
	virtual void Lift_L2R2_NR(int primal, const s16 b[4], const u16 a)
	    const;
	virtual void ILift_L2R2_NR(int primal, const s16 b[4], const u16 a)
	    const;
	virtual void Lift_L3R3_NR(int primal, const s16 b[6], const u16 a)
	    const;
	virtual void ILift_L3R3_NR(int primal, const s16 b[6], const u16 a)
	    const;

	// Lifting Steps (Mixed)
	virtual void Lift_L1R1_MX(int primal, const s16 b[2], const u16 a1,
	    const u16 a2) const;
	virtual void ILift_L1R1_MX(int primal, const s16 b[2], const u16 a1,
	    const u16 a2) const;
	virtual void Lift_L2R2_MX(int primal, const s16 b[4], const u16 a1,
	    const u16 a2) const;
	virtual void ILift_L2R2_MX(int primal, const s16 b[4], const u16 a1,
	    const u16 a2) const;
	virtual void Lift_L3R3_MX(int primal, const s16 b[6], const u16 a1,
	    const u16 a2) const;
	virtual void ILift_L3R3_MX(int primal, const s16 b[6], const u16 a1,
	    const u16 a2) const;

	// Optimized functions for specific wavelet transforms
	virtual void Lift_L1R1_NR_0_m1_m1_2(void) const;
	virtual void ILift_L1R1_NR_0_m1_m1_2(void) const;
	virtual void Lift_L1R1_FR_1_1_1_8(void) const;
	virtual void ILift_L1R1_FR_1_1_1_8(void) const;
	virtual void Lift_L1R1_FR_0_m1_m1_2(void) const;
	virtual void ILift_L1R1_FR_0_m1_m1_2(void) const;
	virtual void Lift_L1R1_FR_1_1_1_4(void) const;
	virtual void ILift_L1R1_FR_1_1_1_4(void) const;
	virtual void Lift_L1R1_FR_0_m1_0_1(void) const;
	virtual void ILift_L1R1_FR_0_m1_0_1(void) const;
	virtual void Lift_L1R1_FR_1_0_1_2(void) const;
	virtual void ILift_L1R1_FR_1_0_1_2(void) const;

    protected:
	PixType *Even, *Odd;
	u_int Ceven, Codd, Rows;
	int Parity;
};


    //  Integer Lifting Operations on the Columns of NTChannels

class LiftChannelC : public Lifting {
    public:
	LiftChannelC(NTChannel *lowpass, NTChannel *highpass);
	virtual ~LiftChannelC();

	// Lifting Steps (Full Rounding)
	virtual void Lift_L1R1_FR(int primal, const s16 b[2], const u16 a)
	    const;
	virtual void ILift_L1R1_FR(int primal, const s16 b[2], const u16 a)
	    const;
	virtual void Lift_L2R2_FR(int primal, const s16 b[4], const u16 a)
	    const;
	virtual void ILift_L2R2_FR(int primal, const s16 b[4], const u16 a)
	    const;
	virtual void Lift_L3R3_FR(int primal, const s16 b[6], const u16 a)
	    const;
	virtual void ILift_L3R3_FR(int primal, const s16 b[6], const u16 a)
	    const;

	// Lifting Steps (No Rounding)
	virtual void Lift_L1R1_NR(int primal, const s16 b[2], const u16 a)
	    const;
	virtual void ILift_L1R1_NR(int primal, const s16 b[2], const u16 a)
	    const;
	virtual void Lift_L2R2_NR(int primal, const s16 b[4], const u16 a)
	    const;
	virtual void ILift_L2R2_NR(int primal, const s16 b[4], const u16 a)
	    const;
	virtual void Lift_L3R3_NR(int primal, const s16 b[6], const u16 a)
	    const;
	virtual void ILift_L3R3_NR(int primal, const s16 b[6], const u16 a)
	    const;

	// Lifting Steps (Mixed)
	virtual void Lift_L1R1_MX(int primal, const s16 b[2], const u16 a1,
	    const u16 a2) const;
	virtual void ILift_L1R1_MX(int primal, const s16 b[2], const u16 a1,
	    const u16 a2) const;
	virtual void Lift_L2R2_MX(int primal, const s16 b[4], const u16 a1,
	    const u16 a2) const;
	virtual void ILift_L2R2_MX(int primal, const s16 b[4], const u16 a1,
	    const u16 a2) const;
	virtual void Lift_L3R3_MX(int primal, const s16 b[6], const u16 a1,
	    const u16 a2) const;
	virtual void ILift_L3R3_MX(int primal, const s16 b[6], const u16 a1,
	    const u16 a2) const;

	// Optimized functions for specific wavelet transforms
	virtual void Lift_L1R1_NR_0_m1_m1_2(void) const;
	virtual void ILift_L1R1_NR_0_m1_m1_2(void) const;
	virtual void Lift_L1R1_FR_1_1_1_8(void) const;
	virtual void ILift_L1R1_FR_1_1_1_8(void) const;
	virtual void Lift_L1R1_FR_0_m1_m1_2(void) const;
	virtual void ILift_L1R1_FR_0_m1_m1_2(void) const;
	virtual void Lift_L1R1_FR_1_1_1_4(void) const;
	virtual void ILift_L1R1_FR_1_1_1_4(void) const;
	virtual void Lift_L1R1_FR_0_m1_0_1(void) const;
	virtual void ILift_L1R1_FR_0_m1_0_1(void) const;
	virtual void Lift_L1R1_FR_1_0_1_2(void) const;
	virtual void ILift_L1R1_FR_1_0_1_2(void) const;

    protected:
	PixType *Even, *Odd;
	u_int Cols, Reven, Rodd;
	int Parity;
};

#endif // WAILI_LIFTING_H
