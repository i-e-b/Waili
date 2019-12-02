//
//	Lifting Operations
//
//  $Id: Lifting.inline.h,v 4.0.2.1.2.1 1999/07/20 16:15:44 geert Exp $
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

#ifndef WAILI_LIFTING_INLINE_H
#define WAILI_LIFTING_INLINE_H


    //  Inline Class Member Functions


    //  Integer Lifting Abstract Base Class

inline Lifting::~Lifting()
{}


    //  Integer Lifting Operations on the Rows of NTChannels

inline LiftChannelR::LiftChannelR(NTChannel *lowpass, NTChannel *highpass)
{
    assert(lowpass != NULL);
    assert(highpass != NULL);
    if (lowpass->GetOffsetX() != highpass->GetOffsetX()) {
	Parity = 1;
	Even = highpass->Data;		/* Even in memory */
	Ceven = highpass->GetCols();
	Odd = lowpass->Data;		/* Odd in memory */
	Codd = lowpass->GetCols();
    } else {
	Parity = 0;
	Even = lowpass->Data;		/* Even in memory */
	Ceven = lowpass->GetCols();
	Odd = highpass->Data;		/* Odd in memory */
	Codd = highpass->GetCols();
    }
    assert(Ceven >= Codd);
    assert(Ceven <= Codd+1);
    Rows = lowpass->GetRows();
    assert(highpass->GetRows() == Rows);
}

inline LiftChannelR::~LiftChannelR()
{}


    //  Integer Lifting Operations on the Columns of NTChannels

inline LiftChannelC::LiftChannelC(NTChannel *lowpass, NTChannel *highpass)
{
    assert(lowpass != NULL);
    assert(highpass != NULL);
    Cols = lowpass->GetCols();
    assert(highpass->GetCols() == Cols);
    if (lowpass->GetOffsetY() != highpass->GetOffsetY()) {
	Parity = 1;
	Even = highpass->Data;		/* Even in memory */
	Reven = highpass->GetRows();
	Odd = lowpass->Data;		/* Odd in memory */
	Rodd = lowpass->GetRows();
    } else {
	Parity = 0;
	Even = lowpass->Data;		/* Even in memory */
	Reven = lowpass->GetRows();
	Odd = highpass->Data;		/* Odd in memory */
	Rodd = highpass->GetRows();
    }
    assert(Reven >= Rodd);
    assert(Reven <= Rodd+1);
}

inline LiftChannelC::~LiftChannelC()
{}

#endif // WAILI_LIFTING_INLINE_H
