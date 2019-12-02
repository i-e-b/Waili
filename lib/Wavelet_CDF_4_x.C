//
//	Integer Wavelet Classes
//
//	Biorthogonal Cohen-Daubechies-Feauveau (4, x)
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
//  $Id: Wavelet_CDF_4_x.C,v 4.2.2.1.2.1 1999/07/20 16:15:52 geert Exp $
//

#include <waili/Util.h>
#include <waili/Channel.h>


const char *Wavelet_CDF_4_x::rcsid = "$Id: Wavelet_CDF_4_x.C,v 4.2.2.1.2.1 1999/07/20 16:15:52 geert Exp $";


const s16 LiftCoef_CDF_4_x::P1b[2] = { -1, -1 };
const u16 LiftCoef_CDF_4_x::P1a1 = 2;
const u16 LiftCoef_CDF_4_x::P1a2 = 2;
const s16 LiftCoef_CDF_4_x::D2b[2] = { -1, -1 };
const u16 LiftCoef_CDF_4_x::D2a = 2;


    //  Biorthogonal Cohen-Daubechies-Feauveau (4, 2)

const s16 LiftCoef_CDF_4_2::P3b[2] = { 3, 3 };
const u16 LiftCoef_CDF_4_2::P3a = 8;


void Wavelet_CDF_4_2::CakeWalk(Lifting &lifting) const
{
    lifting.Lift_L1R1_MX(1, P1b, P1a1, P1a2);
    lifting.Lift_L1R1_FR(0, D2b, D2a);
    lifting.Lift_L1R1_FR(1, P3b, P3a);
}

void Wavelet_CDF_4_2::ICakeWalk(Lifting &lifting) const
{
    lifting.ILift_L1R1_FR(1, P3b, P3a);
    lifting.ILift_L1R1_FR(0, D2b, D2a);
    lifting.ILift_L1R1_MX(1, P1b, P1a1, P1a2);
}


    //  Biorthogonal Cohen-Daubechies-Feauveau (4, 4)

const s16 LiftCoef_CDF_4_4::P3b[4] = { -5, 29, 29, -5 };
const u16 LiftCoef_CDF_4_4::P3a = 64;

void Wavelet_CDF_4_4::CakeWalk(Lifting &lifting) const
{
    lifting.Lift_L1R1_MX(1, P1b, P1a1, P1a2);
    lifting.Lift_L1R1_FR(0, D2b, D2a);
    lifting.Lift_L2R2_FR(1, P3b, P3a);
}

void Wavelet_CDF_4_4::ICakeWalk(Lifting &lifting) const
{
    lifting.ILift_L2R2_FR(1, P3b, P3a);
    lifting.ILift_L1R1_FR(0, D2b, D2a);
    lifting.ILift_L1R1_MX(1, P1b, P1a1, P1a2);
}


    //  Biorthogonal Cohen-Daubechies-Feauveau (4, 6)

const s16 LiftCoef_CDF_4_6::P3b[6] = { 35, -265, 998, 998, -265, 35 };
const u16 LiftCoef_CDF_4_6::P3a = 2048;

void Wavelet_CDF_4_6::CakeWalk(Lifting &lifting) const
{
    lifting.Lift_L1R1_MX(1, P1b, P1a1, P1a2);
    lifting.Lift_L1R1_FR(0, D2b, D2a);
    lifting.Lift_L3R3_FR(1, P3b, P3a);
}

void Wavelet_CDF_4_6::ICakeWalk(Lifting &lifting) const
{
    lifting.ILift_L3R3_FR(1, P3b, P3a);
    lifting.ILift_L1R1_FR(0, D2b, D2a);
    lifting.ILift_L1R1_MX(1, P1b, P1a1, P1a2);
}
