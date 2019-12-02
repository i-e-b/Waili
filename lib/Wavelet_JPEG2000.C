//
//	Integer Wavelet Classes
//
//	Some more wavelets used by JPEG2000
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
//  $Id: Wavelet_JPEG2000.C,v 5.1.2.1.2.1 1999/07/20 16:15:53 geert Exp $
//

#include <waili/Wavelet.h>


    //  Biorthogonal CRF (13, 7)

const char *Wavelet_CRF_13_7::rcsid = "$Id: Wavelet_JPEG2000.C,v 5.1.2.1.2.1 1999/07/20 16:15:53 geert Exp $";

const s16 Wavelet_CRF_13_7::D1b[4] = { 1, -9, -9, 1 };
const u16 Wavelet_CRF_13_7::D1a = 16;

const s16 Wavelet_CRF_13_7::P2b[4] = { -1, 5, 5, -1 };
const u16 Wavelet_CRF_13_7::P2a = 16;

void Wavelet_CRF_13_7::CakeWalk(Lifting &lifting) const
{
    lifting.Lift_L2R2_FR(0, D1b, D1a);
    lifting.Lift_L2R2_FR(1, P2b, P2a);
}

void Wavelet_CRF_13_7::ICakeWalk(Lifting &lifting) const
{
    lifting.ILift_L2R2_FR(1, P2b, P2a);
    lifting.ILift_L2R2_FR(0, D1b, D1a);
}


    //  Biorthogonal SWE (13, 7)

const char *Wavelet_SWE_13_7::rcsid = "$Id: Wavelet_JPEG2000.C,v 5.1.2.1.2.1 1999/07/20 16:15:53 geert Exp $";

const s16 Wavelet_SWE_13_7::D1b[4] = { 1, -9, -9, 1 };
const u16 Wavelet_SWE_13_7::D1a = 16;

const s16 Wavelet_SWE_13_7::P2b[4] = { -1, 9, 9, -1 };
const u16 Wavelet_SWE_13_7::P2a = 32;

void Wavelet_SWE_13_7::CakeWalk(Lifting &lifting) const
{
    lifting.Lift_L2R2_FR(0, D1b, D1a);
    lifting.Lift_L2R2_FR(1, P2b, P2a);
}

void Wavelet_SWE_13_7::ICakeWalk(Lifting &lifting) const
{
    lifting.ILift_L2R2_FR(1, P2b, P2a);
    lifting.ILift_L2R2_FR(0, D1b, D1a);
}
