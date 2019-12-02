//
//	Integer Wavelet Classes
//
//	Biorthogonal Cohen-Daubechies-Feauveau (1, x)
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
//  $Id: Wavelet_CDF_1_x.C,v 4.1.4.1 1999/07/20 16:15:52 geert Exp $
//

#include <waili/Util.h>
#include <waili/Channel.h>


const char *Wavelet_CDF_1_x::rcsid = "$Id: Wavelet_CDF_1_x.C,v 4.1.4.1 1999/07/20 16:15:52 geert Exp $";


const s16 LiftCoef_CDF_1_x::D1b[2] = { -1, 0 };
const u16 LiftCoef_CDF_1_x::D1a = 1;


    //  Biorthogonal Cohen-Daubechies-Feauveau (1, 1)

const s16 LiftCoef_CDF_1_1::P2b[2] = { 0, 1 };
const u16 LiftCoef_CDF_1_1::P2a = 2;

void Wavelet_CDF_1_1::CakeWalk(Lifting &lifting) const
{
#if LIFTING_OPTIMIZE_STEPS
    lifting.Lift_L1R1_FR_0_m1_0_1();
    lifting.Lift_L1R1_FR_1_0_1_2();
#else // !LIFTING_OPTIMIZE_STEPS
    lifting.Lift_L1R1_FR(0, D1b, D1a);
    lifting.Lift_L1R1_FR(1, P2b, P2a);
#endif // !LIFTING_OPTIMIZE_STEPS
}

void Wavelet_CDF_1_1::ICakeWalk(Lifting &lifting) const
{
#if LIFTING_OPTIMIZE_STEPS
    lifting.ILift_L1R1_FR_1_0_1_2();
    lifting.ILift_L1R1_FR_0_m1_0_1();
#else // !LIFTING_OPTIMIZE_STEPS
    lifting.ILift_L1R1_FR(1, P2b, P2a);
    lifting.ILift_L1R1_FR(0, D1b, D1a);
#endif // !LIFTING_OPTIMIZE_STEPS
}


    //  Biorthogonal Cohen-Daubechies-Feauveau (1, 3)

const s16 LiftCoef_CDF_1_3::P2b[4] = { 0, 1, 8, -1 };
const u16 LiftCoef_CDF_1_3::P2a = 16;

void Wavelet_CDF_1_3::CakeWalk(Lifting &lifting) const
{
#if LIFTING_OPTIMIZE_STEPS
    lifting.Lift_L1R1_FR_0_m1_0_1();
#else // !LIFTING_OPTIMIZE_STEPS
    lifting.Lift_L1R1_FR(0, D1b, D1a);
#endif // !LIFTING_OPTIMIZE_STEPS
    lifting.Lift_L2R2_FR(1, P2b, P2a);
}

void Wavelet_CDF_1_3::ICakeWalk(Lifting &lifting) const
{
    lifting.ILift_L2R2_FR(1, P2b, P2a);
#if LIFTING_OPTIMIZE_STEPS
    lifting.ILift_L1R1_FR_0_m1_0_1();
#else // !LIFTING_OPTIMIZE_STEPS
    lifting.ILift_L1R1_FR(0, D1b, D1a);
#endif // !LIFTING_OPTIMIZE_STEPS
}


    //  Biorthogonal Cohen-Daubechies-Feauveau (1, 5)

const s16 LiftCoef_CDF_1_5::P2b[6] = { 0, -3, 22, 128, -22, 3 };
const u16 LiftCoef_CDF_1_5::P2a = 256;

void Wavelet_CDF_1_5::CakeWalk(Lifting &lifting) const
{
#if LIFTING_OPTIMIZE_STEPS
    lifting.Lift_L1R1_FR_0_m1_0_1();
#else // !LIFTING_OPTIMIZE_STEPS
    lifting.Lift_L1R1_FR(0, D1b, D1a);
#endif // !LIFTING_OPTIMIZE_STEPS
    lifting.Lift_L3R3_FR(1, P2b, P2a);
}

void Wavelet_CDF_1_5::ICakeWalk(Lifting &lifting) const
{
    lifting.ILift_L3R3_FR(1, P2b, P2a);
#if LIFTING_OPTIMIZE_STEPS
    lifting.ILift_L1R1_FR_0_m1_0_1();
#else // !LIFTING_OPTIMIZE_STEPS
    lifting.ILift_L1R1_FR(0, D1b, D1a);
#endif // !LIFTING_OPTIMIZE_STEPS
}
