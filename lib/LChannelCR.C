//
//  	LChannelCR Class
//
//  $Id: LChannelCR.C,v 4.6.2.1.2.1 1999/07/20 16:15:50 geert Exp $
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
#include <waili/Util.h>
#include <stdio.h>
#include <stream.h>
#include <math.h>


const char *LChannelCR::rcsid = "$Id: LChannelCR.C,v 4.6.2.1.2.1 1999/07/20 16:15:50 geert Exp $";


// ----------------------------------------------------------------------------
//
//	Lifted Channel Abstract Base Class (lifted Cols and Rows)
//
// ----------------------------------------------------------------------------


LChannelCR::LChannelCR(const Wavelet &filter, u_int cols, u_int rows,
		       int offx, int offy)
    : LChannel(filter, 4, cols, rows)
{
    OffsetX = offx;
    OffsetY = offy;
    int offx_l = GetEven(OffsetX);
    int offx_h = GetOdd(OffsetX);
    int offy_l = GetEven(OffsetY);
    int offy_h = GetOdd(OffsetY);
    u_int clow = Even(OffsetX)? GetEven(Cols): GetOdd(Cols);
    u_int chigh = Cols - clow;
    u_int rlow = Even(OffsetY)? GetEven(Rows): GetOdd(Rows);
    u_int rhigh = Rows - rlow;
    SubBands[SubBand_LL] = new NTChannel(clow, rlow, offx_l, offy_l);
    SubBands[SubBand_LH] = new NTChannel(chigh, rlow, offx_h, offy_l);
    SubBands[SubBand_HL] = new NTChannel(clow, rhigh, offx_l, offy_h);
    SubBands[SubBand_HH] = new NTChannel(chigh, rhigh, offx_h, offy_h);
    int lshift = filter.GetShiftL();
    int hshift = filter.GetShiftH();
    Shifts[SubBand_LL] = lshift+lshift;
    Shifts[SubBand_LH] = Shifts[SubBand_HL] = hshift+lshift;
    Shifts[SubBand_HH] = hshift+hshift;
}

LChannelCR::LChannelCR(const LChannelCR &channel)
    : LChannel(channel)
{
}

void LChannelCR::GetMask(u_int &maskx, u_int &masky) const
{
    SubBands[SubBand_LL]->GetMask(maskx, masky);
    maskx = maskx<<1 | 1;
    masky = masky<<1 | 1;
}

void LChannelCR::Resize(u_int cols, u_int rows)
{
    Channel::Resize(cols, rows);
    u_int clow = Even(OffsetX)? GetEven(Cols): GetOdd(Cols);
    u_int chigh = Cols - clow;
    u_int rlow = Even(OffsetY)? GetEven(Rows): GetOdd(Rows);
    u_int rhigh = Rows - rlow;
    if (SubBands[SubBand_LL])
	SubBands[SubBand_LL]->Resize(clow, rlow);
    if (SubBands[SubBand_LH])
	SubBands[SubBand_LH]->Resize(chigh, rlow);
    if (SubBands[SubBand_HL])
	SubBands[SubBand_HL]->Resize(clow, rhigh);
    if (SubBands[SubBand_HH])
	SubBands[SubBand_HH]->Resize(chigh, rhigh);
}

void LChannelCR::SetOffsetX(int offx)
{
    if (Even(offx) != Even(OffsetX)) {
	TransformDescriptor *transform = GetTransform();
	u_int depth = GetDepth();

	NTChannel *ntchannel = LChannel::IFwt();
	ntchannel->SetOffsetX(offx);
	LChannel *lchannel = ntchannel->Fwt(transform, depth);
	delete ntchannel;

	Cols = lchannel->Cols;
	Rows = lchannel->Rows;
	OffsetX = lchannel->OffsetX;
	OffsetY = lchannel->OffsetY;
	SubBands[SubBand_LL] = lchannel->SubBands[SubBand_LL];
	SubBands[SubBand_LH] = lchannel->SubBands[SubBand_LH];
	SubBands[SubBand_HL] = lchannel->SubBands[SubBand_HL];
	SubBands[SubBand_HH] = lchannel->SubBands[SubBand_HH];
	Filter = lchannel->Filter;

	lchannel->SubBands[SubBand_LL] = NULL;
	lchannel->SubBands[SubBand_LH] = NULL;
	lchannel->SubBands[SubBand_HL] = NULL;
	lchannel->SubBands[SubBand_HH] = NULL;
	lchannel->Filter = NULL;
	delete lchannel;
    } else {
	OffsetX = offx;
	SubBands[SubBand_LL]->SetOffsetX(GetEven(offx));
	SubBands[SubBand_LH]->SetOffsetX(GetOdd(offx));
	SubBands[SubBand_HL]->SetOffsetX(GetEven(offx));
	SubBands[SubBand_HH]->SetOffsetX(GetOdd(offx));
    }
}

void LChannelCR::SetOffsetY(int offy)
{
    if (Even(offy) != Even(OffsetY)) {
	TransformDescriptor *transform = GetTransform();
	u_int depth = GetDepth();

	NTChannel *ntchannel = LChannel::IFwt();
	ntchannel->SetOffsetY(offy);
	LChannel *lchannel = ntchannel->Fwt(transform, depth);
	delete ntchannel;

	Cols = lchannel->Cols;
	Rows = lchannel->Rows;
	OffsetX = lchannel->OffsetX;
	OffsetY = lchannel->OffsetY;
	SubBands[SubBand_LL] = lchannel->SubBands[SubBand_LL];
	SubBands[SubBand_LH] = lchannel->SubBands[SubBand_LH];
	SubBands[SubBand_HL] = lchannel->SubBands[SubBand_HL];
	SubBands[SubBand_HH] = lchannel->SubBands[SubBand_HH];
	Filter = lchannel->Filter;

	lchannel->SubBands[SubBand_LL] = NULL;
	lchannel->SubBands[SubBand_LH] = NULL;
	lchannel->SubBands[SubBand_HL] = NULL;
	lchannel->SubBands[SubBand_HH] = NULL;
	lchannel->Filter = NULL;
	delete lchannel;
    } else {
	OffsetY = offy;
	SubBands[SubBand_LL]->SetOffsetY(GetEven(offy));
	SubBands[SubBand_LH]->SetOffsetY(GetEven(offy));
	SubBands[SubBand_HL]->SetOffsetY(GetOdd(offy));
	SubBands[SubBand_HH]->SetOffsetY(GetOdd(offy));
    }
}

void LChannelCR::Add(const Channel &channel)
{
    assert(channel.IsLifted());
    assert(((LChannel&)channel).GetTransformType() == TT_ColsRows);
    assert(channel.GetOffsetX() == OffsetX);
    assert(channel.GetOffsetY() == OffsetY);
    assert(channel.GetRows() == Rows);
    assert(channel.GetCols() == Cols);
    SubBands[SubBand_LL]->Add(*((LChannel&)channel)[SubBand_LL]);
    SubBands[SubBand_LH]->Add(*((LChannel&)channel)[SubBand_LH]);
    SubBands[SubBand_HL]->Add(*((LChannel&)channel)[SubBand_HL]);
    SubBands[SubBand_HH]->Add(*((LChannel&)channel)[SubBand_HH]);
}

void LChannelCR::Subtract(const Channel &channel)
{
    assert(channel.IsLifted());
    assert(((LChannel&)channel).GetTransformType() == TT_ColsRows);
    assert(channel.GetOffsetX() == OffsetX);
    assert(channel.GetOffsetY() == OffsetY);
    assert(channel.GetRows() == Rows);
    assert(channel.GetCols() == Cols);
    SubBands[SubBand_LL]->Subtract(*((LChannel&)channel)[SubBand_LL]);
    SubBands[SubBand_LH]->Subtract(*((LChannel&)channel)[SubBand_LH]);
    SubBands[SubBand_HL]->Subtract(*((LChannel&)channel)[SubBand_HL]);
    SubBands[SubBand_HH]->Subtract(*((LChannel&)channel)[SubBand_HH]);
}

LChannelCR *LChannelCR::Diff(const Channel &channel) const
{
    assert(channel.IsLifted());
    assert(((LChannel&)channel).GetTransformType() == TT_ColsRows);
    assert(channel.GetOffsetX() == OffsetX);
    assert(channel.GetOffsetY() == OffsetY);
    assert(channel.GetRows() == Rows);
    assert(channel.GetCols() == Cols);
    LChannelCR *diff = new LChannelCR(*Filter);
    diff->Cols = Cols;
    diff->Rows = Rows;
    diff->OffsetX = OffsetX;
    diff->OffsetY = OffsetY;
    diff->SubBands[SubBand_LL] = SubBands[SubBand_LL]->
			Diff(*((LChannel&)channel)[SubBand_LL]);
    diff->SubBands[SubBand_LH] = SubBands[SubBand_LH]->
			Diff(*((LChannel&)channel)[SubBand_LH]);
    diff->SubBands[SubBand_HL] = SubBands[SubBand_HL]->
			Diff(*((LChannel&)channel)[SubBand_HL]);
    diff->SubBands[SubBand_HH] = SubBands[SubBand_HH]->
			Diff(*((LChannel&)channel)[SubBand_HH]);
    return(diff);
}

void LChannelCR::Lazy(const NTChannel &source)
{
    assert(source.Data != NULL);
    const PixType *data = source.Data;

    assert(SubBands[SubBand_LL] != NULL);
    assert(SubBands[SubBand_LH] != NULL);
    assert(SubBands[SubBand_HL] != NULL);
    assert(SubBands[SubBand_HH] != NULL);
    assert(SubBands[SubBand_LL]->IsLifted() == 0);
    assert(SubBands[SubBand_LH]->IsLifted() == 0);
    assert(SubBands[SubBand_HL]->IsLifted() == 0);
    assert(SubBands[SubBand_HH]->IsLifted() == 0);
    assert(((NTChannel *)SubBands[SubBand_LL])->Data != NULL);
    assert(((NTChannel *)SubBands[SubBand_LH])->Data != NULL);
    assert(((NTChannel *)SubBands[SubBand_HL])->Data != NULL);
    assert(((NTChannel *)SubBands[SubBand_HH])->Data != NULL);
    PixType *ll = ((NTChannel *)SubBands[SubBand_LL])->Data;
    PixType *lh = ((NTChannel *)SubBands[SubBand_LH])->Data;
    PixType *hl = ((NTChannel *)SubBands[SubBand_HL])->Data;
    PixType *hh = ((NTChannel *)SubBands[SubBand_HH])->Data;
    u_int clow = GetClow();
    u_int chigh = GetChigh();
    u_int rlow = GetRlow();
    u_int rhigh = GetRhigh();
    if (Even(OffsetX))
	if (Even(OffsetY)) {
	    for (u_int r = 0; r < rhigh; r++) {
		for (u_int c = 0; c < chigh; c++) {
		    *ll++ = *data++;
		    *lh++ = *data++;
		}
		if (clow != chigh)
		    *ll++ = *data++;
		for (u_int c = 0; c < chigh; c++) {
		    *hl++ = *data++;
		    *hh++ = *data++;
		}
		if (clow != chigh)
		    *hl++ = *data++;
	    }
	    if (rlow != rhigh) {
		for (u_int c = 0; c < chigh; c++) {
		    *ll++ = *data++;
		    *lh++ = *data++;
		}
		if (clow != chigh)
		    *ll++ = *data++;
	    }
	} else {
	    for (u_int r = 0; r < rlow; r++) {
		for (u_int c = 0; c < chigh; c++) {
		    *hl++ = *data++;
		    *hh++ = *data++;
		}
		if (clow != chigh)
		    *hl++ = *data++;
		for (u_int c = 0; c < chigh; c++) {
		    *ll++ = *data++;
		    *lh++ = *data++;
		}
		if (clow != chigh)
		    *ll++ = *data++;
	    }
	    if (rlow != rhigh) {
		for (u_int c = 0; c < chigh; c++) {
		    *hl++ = *data++;
		    *hh++ = *data++;
		}
		if (clow != chigh)
		    *hl++ = *data++;
	    }
	}
    else
	if (Even(OffsetY)) {
	    for (u_int r = 0; r < rhigh; r++) {
		for (u_int c = 0; c < clow; c++) {
		    *lh++ = *data++;
		    *ll++ = *data++;
		}
		if (clow != chigh)
		    *lh++ = *data++;
		for (u_int c = 0; c < clow; c++) {
		    *hh++ = *data++;
		    *hl++ = *data++;
		}
		if (clow != chigh)
		    *hh++ = *data++;
	    }
	    if (rlow != rhigh) {
		for (u_int c = 0; c < clow; c++) {
		    *lh++ = *data++;
		    *ll++ = *data++;
		}
		if (clow != chigh)
		    *lh++ = *data++;
	    }
	} else {
	    for (u_int r = 0; r < rlow; r++) {
		for (u_int c = 0; c < clow; c++) {
		    *hh++ = *data++;
		    *hl++ = *data++;
		}
		if (clow != chigh)
		    *hh++ = *data++;
		for (u_int c = 0; c < clow; c++) {
		    *lh++ = *data++;
		    *ll++ = *data++;
		}
		if (clow != chigh)
		    *lh++ = *data++;
	    }
	    if (rlow != rhigh) {
		for (u_int c = 0; c < clow; c++) {
		    *hh++ = *data++;
		    *hl++ = *data++;
		}
		if (clow != chigh)
		    *hh++ = *data++;
	    }
	}
}

void LChannelCR::ILazy(NTChannel &dest) const
{
    assert(dest.Data != NULL);
    PixType *data = dest.Data;

    assert(SubBands[SubBand_LL] != NULL);
    assert(SubBands[SubBand_LH] != NULL);
    assert(SubBands[SubBand_HL] != NULL);
    assert(SubBands[SubBand_HH] != NULL);
    assert(SubBands[SubBand_LL]->IsLifted() == 0);
    assert(SubBands[SubBand_LH]->IsLifted() == 0);
    assert(SubBands[SubBand_HL]->IsLifted() == 0);
    assert(SubBands[SubBand_HH]->IsLifted() == 0);
    assert(((NTChannel *)SubBands[SubBand_LL])->Data != NULL);
    assert(((NTChannel *)SubBands[SubBand_LH])->Data != NULL);
    assert(((NTChannel *)SubBands[SubBand_HL])->Data != NULL);
    assert(((NTChannel *)SubBands[SubBand_HH])->Data != NULL);
    const PixType *ll = ((NTChannel *)SubBands[SubBand_LL])->Data;
    const PixType *lh = ((NTChannel *)SubBands[SubBand_LH])->Data;
    const PixType *hl = ((NTChannel *)SubBands[SubBand_HL])->Data;
    const PixType *hh = ((NTChannel *)SubBands[SubBand_HH])->Data;
    u_int clow = GetClow();
    u_int chigh = GetChigh();
    u_int rlow = GetRlow();
    u_int rhigh = GetRhigh();
    if (Even(OffsetX))
	if (Even(OffsetY)) {
	    for (u_int r = 0; r < rhigh; r++) {
		for (u_int c = 0; c < chigh; c++) {
		    *data++ = *ll++;
		    *data++ = *lh++;
		}
		if (clow != chigh)
		    *data++ = *ll++;
		for (u_int c = 0; c < chigh; c++) {
		    *data++ = *hl++;
		    *data++ = *hh++;
		}
		if (clow != chigh)
		    *data++ = *hl++;
	    }
	    if (rlow != rhigh) {
		for (u_int c = 0; c < chigh; c++) {
		    *data++ = *ll++;
		    *data++ = *lh++;
		}
		if (clow != chigh)
		    *data++ = *ll++;
	    }
	} else {
	    for (u_int r = 0; r < rlow; r++) {
		for (u_int c = 0; c < chigh; c++) {
		    *data++ = *hl++;
		    *data++ = *hh++;
		}
		if (clow != chigh)
		    *data++ = *hl++;
		for (u_int c = 0; c < chigh; c++) {
		    *data++ = *ll++;
		    *data++ = *lh++;
		}
		if (clow != chigh)
		    *data++ = *ll++;
	    }
	    if (rlow != rhigh) {
		for (u_int c = 0; c < chigh; c++) {
		    *data++ = *hl++;
		    *data++ = *hh++;
		}
		if (clow != chigh)
		    *data++ = *hl++;
	    }
	}
    else
	if (Even(OffsetY)) {
	    for (u_int r = 0; r < rhigh; r++) {
		for (u_int c = 0; c < clow; c++) {
		    *data++ = *lh++;
		    *data++ = *ll++;
		}
		if (clow != chigh)
		    *data++ = *lh++;
		for (u_int c = 0; c < clow; c++) {
		    *data++ = *hh++;
		    *data++ = *hl++;
		}
		if (clow != chigh)
		    *data++ = *hh++;
	    }
	    if (rlow != rhigh) {
		for (u_int c = 0; c < clow; c++) {
		    *data++ = *lh++;
		    *data++ = *ll++;
		}
		if (clow != chigh)
		    *data++ = *lh++;
	    }
	} else {
	    for (u_int r = 0; r < rlow; r++) {
		for (u_int c = 0; c < clow; c++) {
		    *data++ = *hh++;
		    *data++ = *hl++;
		}
		if (clow != chigh)
		    *data++ = *hh++;
		for (u_int c = 0; c < clow; c++) {
		    *data++ = *lh++;
		    *data++ = *ll++;
		}
		if (clow != chigh)
		    *data++ = *lh++;
	    }
	    if (rlow != rhigh) {
		for (u_int c = 0; c < clow; c++) {
		    *data++ = *hh++;
		    *data++ = *hl++;
		}
		if (clow != chigh)
		    *data++ = *hh++;
	    }
	}
}

void LChannelCR::CakeWalk(void)
{
    assert(SubBands[SubBand_LL]->IsLifted() == 0);
    assert(SubBands[SubBand_LH]->IsLifted() == 0);
    assert(SubBands[SubBand_HL]->IsLifted() == 0);
    assert(SubBands[SubBand_HH]->IsLifted() == 0);
    LiftChannelR rowsl((NTChannel *)SubBands[SubBand_LL],
			     (NTChannel *)SubBands[SubBand_LH]);
    LiftChannelR rowsh((NTChannel *)SubBands[SubBand_HL],
			     (NTChannel *)SubBands[SubBand_HH]);
    LiftChannelC colsl((NTChannel *)SubBands[SubBand_LL],
			     (NTChannel *)SubBands[SubBand_HL]);
    LiftChannelC colsh((NTChannel *)SubBands[SubBand_LH],
			     (NTChannel *)SubBands[SubBand_HH]);

    // Transform the Rows
    Filter->CakeWalk(rowsl);
    Filter->CakeWalk(rowsh);

    // Transform the Columns
    Filter->CakeWalk(colsl);
    Filter->CakeWalk(colsh);
}

void LChannelCR::ICakeWalk(void)
{
    assert(SubBands[SubBand_LL]->IsLifted() == 0);
    assert(SubBands[SubBand_LH]->IsLifted() == 0);
    assert(SubBands[SubBand_HL]->IsLifted() == 0);
    assert(SubBands[SubBand_HH]->IsLifted() == 0);
    LiftChannelR rowsl((NTChannel *)SubBands[SubBand_LL],
			     (NTChannel *)SubBands[SubBand_LH]);
    LiftChannelR rowsh((NTChannel *)SubBands[SubBand_HL],
			     (NTChannel *)SubBands[SubBand_HH]);
    LiftChannelC colsl((NTChannel *)SubBands[SubBand_LL],
			     (NTChannel *)SubBands[SubBand_HL]);
    LiftChannelC colsh((NTChannel *)SubBands[SubBand_LH],
			     (NTChannel *)SubBands[SubBand_HH]);

    // Transform the Columns
    Filter->ICakeWalk(colsl);
    Filter->ICakeWalk(colsh);

    // Transform the Rows
    Filter->ICakeWalk(rowsl);
    Filter->ICakeWalk(rowsh);
}

void LChannelCR::Zip(const LChannelCR &channel_lh, const LChannelCR &channel_hl,
		     const LChannelCR &channel_hh)
{
    LChannelCR *dest =
	    new LChannelCR(*Filter, Cols<<1, Rows<<1, OffsetX<<1, OffsetY<<1);

    LChannelCR *subband = new LChannelCR(*Filter);
    subband->Cols = Cols;
    subband->Rows = Rows;
    subband->OffsetX = OffsetX;
    subband->OffsetY = OffsetY;

    subband->SubBands[SubBand_LL] = SubBands[SubBand_LL];
    subband->SubBands[SubBand_LH] = channel_lh.SubBands[SubBand_LL];
    subband->SubBands[SubBand_HL] = channel_hl.SubBands[SubBand_LL];
    subband->SubBands[SubBand_HH] = channel_hh.SubBands[SubBand_LL];
    subband->ILazy(*(NTChannel*)dest->SubBands[SubBand_LL]);

    subband->SubBands[SubBand_LL] = channel_lh.SubBands[SubBand_LH];
    subband->SubBands[SubBand_LH] = SubBands[SubBand_LH];
    subband->SubBands[SubBand_HL] = channel_hh.SubBands[SubBand_LH];
    subband->SubBands[SubBand_HH] = channel_hl.SubBands[SubBand_LH];
    subband->ILazy(*(NTChannel*)dest->SubBands[SubBand_LH]);

    subband->SubBands[SubBand_LL] = channel_hl.SubBands[SubBand_HL];
    subband->SubBands[SubBand_LH] = channel_hh.SubBands[SubBand_HL];
    subband->SubBands[SubBand_HL] = SubBands[SubBand_HL];
    subband->SubBands[SubBand_HH] = channel_lh.SubBands[SubBand_HL];
    subband->ILazy(*(NTChannel*)dest->SubBands[SubBand_HL]);

    subband->SubBands[SubBand_LL] = channel_hh.SubBands[SubBand_HH];
    subband->SubBands[SubBand_LH] = channel_hl.SubBands[SubBand_HH];
    subband->SubBands[SubBand_HL] = channel_lh.SubBands[SubBand_HH];
    subband->SubBands[SubBand_HH] = SubBands[SubBand_HH];
    subband->ILazy(*(NTChannel*)dest->SubBands[SubBand_HH]);

    subband->SubBands[SubBand_LL] = NULL;
    subband->SubBands[SubBand_LH] = NULL;
    subband->SubBands[SubBand_HL] = NULL;
    subband->SubBands[SubBand_HH] = NULL;
    delete subband;

    Cols = dest->Cols;
    Rows = dest->Rows;
    OffsetX = dest->OffsetX;
    OffsetY = dest->OffsetY;
    SubBands[SubBand_LL] = dest->SubBands[SubBand_LL];
    SubBands[SubBand_LH] = dest->SubBands[SubBand_LH];
    SubBands[SubBand_HL] = dest->SubBands[SubBand_HL];
    SubBands[SubBand_HH] = dest->SubBands[SubBand_HH];

    dest->SubBands[SubBand_LL] = NULL;
    dest->SubBands[SubBand_LH] = NULL;
    dest->SubBands[SubBand_HL] = NULL;
    dest->SubBands[SubBand_HH] = NULL;
    delete dest;
}

NTChannel *LChannelCR::IFwt(int x1, int y1, int x2, int y2) const
{
    int bwstart = Min(Filter->GetHStart(), Filter->GetGStart());
    int bwend = Max(Filter->GetHEnd(), Filter->GetGEnd());
    int bw = Max(-bwstart, bwend);

    // Calculate new coordinates taking account of the needed borders
    int xb1 = Max((int)x1-bw-1, OffsetX);
    int yb1 = Max((int)y1-bw-1, OffsetY);
    int xb2 = Min((int)x2+bw+1, OffsetX+(int)Cols-1);
    int yb2 = Min((int)y2+bw+1, OffsetY+(int)Rows-1);

    LChannelCR *lchannel = new LChannelCR(*Filter);
    lchannel->Cols = xb2-xb1+1;
    lchannel->Rows = yb2-yb1+1;
    lchannel->OffsetX = xb1;
    lchannel->OffsetY = yb1;

    int xb1_l = GetEven(xb1);
    int yb1_l = GetEven(yb1);
    int xb1_h = GetOdd(xb1);
    int yb1_h = GetOdd(yb1);
    int xb2_l = GetOdd(xb2);
    int yb2_l = GetOdd(yb2);
    int xb2_h = GetOdd(xb2-1);
    int yb2_h = GetOdd(yb2-1);

    // Crop subimages of all subbands
    if (SubBands[SubBand_LL]->IsLifted())
	lchannel->SubBands[SubBand_LL] = ((LChannel*)SubBands[SubBand_LL])->
					 IFwt(xb1_l, yb1_l, xb2_l, yb2_l);
    else
	lchannel->SubBands[SubBand_LL] =
			SubBands[SubBand_LL]->Crop(xb1_l, yb1_l, xb2_l, yb2_l);

    lchannel->SubBands[SubBand_LH] =
		    SubBands[SubBand_LH]->Crop(xb1_h, yb1_l, xb2_h, yb2_l);
    lchannel->SubBands[SubBand_HL] =
		    SubBands[SubBand_HL]->Crop(xb1_l, yb1_h, xb2_l, yb2_h);
    lchannel->SubBands[SubBand_HH] =
		    SubBands[SubBand_HH]->Crop(xb1_h, yb1_h, xb2_h, yb2_h);

    // Apply invers transform and crop redundant borders
    NTChannel *ntchannel = lchannel->IFwtStep();
    NTChannel *channel = ntchannel->Crop(x1, y1, x2, y2);
    delete lchannel;
    delete ntchannel;

    return(channel);
}

NTChannel *LChannelCR::CropMergeH(LChannelCR *channel,
				  int x1_l, int x1_h, int y1_l, int y1_h,
				  int x2_l, int x2_h, int y2_l, int y2_h)
{
	assert(x1_l <= x2_l);
	assert(x1_h <= x2_h);
	assert(y1_l <= y2_l);
	assert(y1_h <= y2_h);

	// Crop the specified subimages of all subbands
	// None of the subbands is tranformed!!!!!
	NTChannel *channel_ll = ((NTChannel*)channel->SubBands[SubBand_LL])->
				Crop(x1_l, y1_l, x2_l, y2_l);
	NTChannel *channel_lh = ((NTChannel*)channel->SubBands[SubBand_LH])->
				Crop(x1_h, y1_l, x2_h, y2_l);
	NTChannel *channel_hl = ((NTChannel*)channel->SubBands[SubBand_HL])->
				Crop(x1_l, y1_h, x2_l, y2_h);
	NTChannel *channel_hh = ((NTChannel*)channel->SubBands[SubBand_HH])->
				Crop(x1_h, y1_h, x2_h, y2_h);

	// Merge high-pass subbands in the subbands of the current channel
	SubBands[SubBand_LH]->Merge(*channel_lh);
	SubBands[SubBand_HL]->Merge(*channel_hl);
	SubBands[SubBand_HH]->Merge(*channel_hh);

	delete channel;
	delete channel_lh;
	delete channel_hl;
	delete channel_hh;

	return(channel_ll);
}

LChannelCR *LChannelCR::Crop_rec(int x1, int y1, int x2, int y2,
				 NTChannel *top, NTChannel *bottom,
				 NTChannel *left, NTChannel *right) const
{
    int bwstart = Min(Filter->GetHStart(), Filter->GetGStart());
    int bwend = Max(Filter->GetHEnd(), Filter->GetGEnd());
    int bw = Max(-bwstart, bwend);

    LChannelCR *channel;

    int x1_l = GetEven(x1);
    int y1_l = GetEven(y1);
    int x1_h = GetOdd(x1);
    int y1_h = GetOdd(y1);
    int x2_l = GetOdd(x2);
    int y2_l = GetOdd(y2);
    int x2_h = GetOdd(x2-1);
    int y2_h = GetOdd(y2-1);

    // Calculate new coordinates taking account of the needed borders
    int y2_tb = top? y1+(bw<<1)+(int)top->Rows: y1+(bw<<1);
    int y1_bb = bottom? y2-(bw<<1)-(int)bottom->Rows: y2-(bw<<1);
    int x2_lb = left? x1+(bw<<1)+(int)left->Cols: x1+(bw<<1);
    int x1_rb = right? x2-(bw<<1)-(int)right->Cols: x2-(bw<<1);

    if (y1_bb-y2_tb > 1 && x1_rb-x2_lb > 1) {
	channel = new LChannelCR(*Filter);
	channel->Cols = x2-x1+1;
	channel->Rows = y2-y1+1;
	channel->OffsetX = x1;
	channel->OffsetY = y1;

	// Crop all high-pass subbands (without borders)
	channel->SubBands[SubBand_LH] =
			SubBands[SubBand_LH]->Crop(x1_h, y1_l, x2_h, y2_l);
	channel->SubBands[SubBand_HL] =
			SubBands[SubBand_HL]->Crop(x1_l, y1_h, x2_l, y2_h);
	channel->SubBands[SubBand_HH] =
			SubBands[SubBand_HH]->Crop(x1_h, y1_h, x2_h, y2_h);

	// Adjust top border according to the higher resolution level
	NTChannel *nttop = IFwt(x1, y1, x2, y2_tb);

	if (top)
	    nttop->Merge(*top);
	if (left)
	    CopyRect(left->Data, left->Cols, 0, 0, nttop->Data, nttop->Cols,
		     0, 0, left->Cols, nttop->Rows);
	if (right)
	    CopyRect(right->Data, right->Cols, 0, 0, nttop->Data, nttop->Cols,
		     nttop->Cols-right->Cols, 0, right->Cols, nttop->Rows);

	LChannelCR *ltop = nttop->PushFwtStepCR(*Filter);
	delete nttop;

	y2_tb -= bw;
	int y2_tb_l = GetOdd(y2_tb);
	int y2_tb_h = GetOdd(y2_tb-1);

	NTChannel *top_ll = channel->CropMergeH(ltop, x1_l, x1_h, y1_l, y1_h,
					    x2_l, x2_h, y2_tb_l, y2_tb_h);

	// Adjust bottom border according to the higher resolution level
	NTChannel *ntbottom = IFwt(x1, y1_bb, x2, y2);

	if (bottom)
	    ntbottom->Merge(*bottom);
	if (left)
	    CopyRect(left->Data, left->Cols, 0, left->Rows-ntbottom->Rows,
		     ntbottom->Data, ntbottom->Cols, 0, 0, left->Cols,
		     ntbottom->Rows);
	if (right)
	    CopyRect(right->Data, right->Cols, 0, right->Rows-ntbottom->Rows,
		     ntbottom->Data, ntbottom->Cols, ntbottom->Cols-right->Cols,
		     0, right->Cols, ntbottom->Rows);

	LChannelCR *lbottom = ntbottom->PushFwtStepCR(*Filter);
	delete ntbottom;

	y1_bb += bw;
	int y1_bb_l = GetEven(y1_bb);
	int y1_bb_h = GetOdd(y1_bb);

	NTChannel *bottom_ll = channel->CropMergeH(lbottom, x1_l, x1_h, y1_bb_l,
					    y1_bb_h, x2_l, x2_h, y2_l, y2_h);

	// Adjust left border according to the higher resolution level
	NTChannel *ntleft = IFwt(x1, y1, x2_lb, y2);

	if (left)
	    ntleft->Merge(*left);
	if (top)
	    CopyRect(top->Data, top->Cols, 0, 0, ntleft->Data, ntleft->Cols,
		     0, 0, ntleft->Cols, top->Rows);
	if (bottom)
	    CopyRect(bottom->Data, bottom->Cols, 0, 0, ntleft->Data,
		     ntleft->Cols, 0, ntleft->Rows-bottom->Rows,
		     ntleft->Cols, bottom->Rows);

	LChannelCR *lleft = ntleft->PushFwtStepCR(*Filter);
	delete ntleft;

	x2_lb -= bw;
	int x2_lb_l = GetOdd(x2_lb);
	int x2_lb_h = GetOdd(x2_lb-1);

	NTChannel *left_ll = channel->CropMergeH(lleft, x1_l, x1_h, y1_l, y1_h,
					    x2_lb_l, x2_lb_h, y2_l, y2_h);

	// Adjust right border according to the higher resolution level
	NTChannel *ntright = IFwt(x1_rb, y1, x2, y2);

	if (right)
	    ntright->Merge(*right);
	if (top)
	    CopyRect(top->Data, top->Cols, top->Cols-ntright->Cols, 0,
		     ntright->Data, ntright->Cols, 0, 0,
		     ntright->Cols, top->Rows);
	if (bottom)
	    CopyRect(bottom->Data, bottom->Cols, bottom->Cols-ntright->Cols, 0,
		     ntright->Data, ntright->Cols, 0,
		     ntright->Rows-bottom->Rows, ntright->Cols, bottom->Rows);

	LChannelCR *lright = ntright->PushFwtStepCR(*Filter);
	delete ntright;

	x1_rb += bw;
	int x1_rb_l = GetEven(x1_rb);
	int x1_rb_h = GetOdd(x1_rb);

	NTChannel *right_ll = channel->CropMergeH(lright, x1_rb_l, x1_rb_h,
					y1_l, y1_h, x2_l, x2_h, y2_l, y2_h);

	if (SubBands[SubBand_LL]->IsLifted())
	    // Crop coarser level taking account of new borders
	    channel->SubBands[SubBand_LL] = ((LChannel*)SubBands[SubBand_LL])->
				    Crop_rec(x1_l, y1_l, x2_l, y2_l, top_ll,
					     bottom_ll, left_ll, right_ll);
	else {
	    // Crop low-pass subband taking account of new borders
	    channel->SubBands[SubBand_LL] = SubBands[SubBand_LL]->
					    Crop(x1_l, y1_l, x2_l, y2_l);

	    channel->SubBands[SubBand_LL]->Merge(*top_ll);
	    channel->SubBands[SubBand_LL]->Merge(*bottom_ll);
	    channel->SubBands[SubBand_LL]->Merge(*left_ll);
	    channel->SubBands[SubBand_LL]->Merge(*right_ll);
	}

	delete top_ll;
	delete bottom_ll;
	delete left_ll;
	delete right_ll;
    } else {
	// Crop after applying a full IFwt of the subimage and by
	// reconstructing the original structure. This is only the case
	// for one of the most coarse levels.
	TransformDescriptor *transform = GetTransform();
	u_int depth = GetDepth();

	NTChannel* ntchannel = IFwt(x1, y1, x2, y2);

	if (top)
	    ntchannel->Merge(*top);
	if (bottom)
	    ntchannel->Merge(*bottom);
	if (left)
	    ntchannel->Merge(*left);
	if (right)
	    ntchannel->Merge(*right);

	channel = (LChannelCR*)ntchannel->Fwt(transform, depth);

	delete[] transform;
	delete ntchannel;
    }

    return(channel);
}

void LChannelCR::Merge_rec(const Channel *channel,
			   NTChannel *top, NTChannel *bottom,
			   NTChannel *left, NTChannel *right)
{
    int bwstart = Min(Filter->GetHStart(), Filter->GetGStart());
    int bwend = Max(Filter->GetHEnd(), Filter->GetGEnd());
    int bw = Max(-bwstart, bwend);

    int ch_x1 = channel->GetOffsetX();
    int ch_y1 = channel->GetOffsetY();
    int ch_x2 = ch_x1+(int)channel->GetCols()-1;
    int ch_y2 = ch_y1+(int)channel->GetRows()-1;
    int x1, y1, x2, y2, y1_tb, y2_tb, y1_bb, y2_bb, x1_lb, x2_lb, x1_rb, x2_rb;

    // Calculate inner and outer border coordinates
    if (top) {
	y1 = top->OffsetY;
	y1_tb = y1-(bw<<1)-1;
	y2_tb = y1+(int)top->Rows+(bw<<1);
    } else {
	y1 = ch_y1;
	y1_tb = y1-(bw<<1)-1;
	y2_tb = y1+(bw<<1);
    }
    if (bottom) {
	y2 = bottom->OffsetY+(int)bottom->Rows-1;
	y1_bb = y2-(int)bottom->Rows-(bw<<1);
	y2_bb = y2+(bw<<1)+1;
    } else {
	y2 = ch_y2;
	y1_bb = y2-(bw<<1);
	y2_bb = y2+(bw<<1)+1;
    }
    if (left) {
	x1 = left->OffsetX;
	x1_lb = x1-(bw<<1)-1;
	x2_lb = x1+(int)left->Cols+(bw<<1);
    } else {
	x1 = ch_x1;
	x1_lb = x1-(bw<<1)-1;
	x2_lb = x1+(bw<<1);
    }
    if (right) {
	x2 = right->OffsetX+(int)right->Cols-1;
	x1_rb = x2-(int)right->Cols-(bw<<1);
	x2_rb = x2+(bw<<1)+1;
    } else {
	x2 = ch_x2;
	x1_rb = x2-(bw<<1);
	x2_rb = x2+(bw<<1)+1;
    }

    y1_tb = Max(y1_tb, OffsetY);
    y2_bb = Min(y2_bb, OffsetY+(int)Rows-1);
    x1_lb = Max(x1_lb, OffsetX);
    x2_rb = Min(x2_rb, OffsetX+(int)Cols-1);

    if (y1_bb-y2_tb > 1 && x1_rb-x2_lb > 1 && channel->IsLifted() &&
		((LChannel*)channel)->GetTransformType() == TT_ColsRows) {
	// Adjust top border according to the higher resolution level
	NTChannel *nttop = IFwt(x1_lb, y1_tb, x2_rb, y2_tb);
	NTChannel *ntch_top = ((LChannel*)channel)->
			      IFwt(ch_x1, ch_y1, ch_x2, y2_tb);
	nttop->Merge(*ntch_top);
	delete ntch_top;

	if (top)
	    nttop->Merge(*top);
	if (left)
	    CopyRect(left->Data, left->Cols, 0, 0, nttop->Data, nttop->Cols,
		     x1-x1_lb, y1-y1_tb, left->Cols, y2_tb-y1+1);
	if (right)
	    CopyRect(right->Data, right->Cols, 0, 0, nttop->Data, nttop->Cols,
		     x2-right->Cols+1-x1_lb, y1-y1_tb, right->Cols, y2_tb-y1+1);

	LChannelCR *ltop = nttop->PushFwtStepCR(*Filter);
	delete nttop;

	// Adjust bottom border according to the higher resolution level
	NTChannel *ntbottom = IFwt(x1_lb, y1_bb, x2_rb, y2_bb);
	NTChannel *ntch_bottom = ((LChannel*)channel)->
				 IFwt(ch_x1, y1_bb, ch_x2, ch_y2);
	ntbottom->Merge(*ntch_bottom);
	delete ntch_bottom;

	if (bottom)
	    ntbottom->Merge(*bottom);
	if (left)
	    CopyRect(left->Data, left->Cols, 0, y1_bb-y1, ntbottom->Data,
		     ntbottom->Cols, x1-x1_lb, 0, left->Cols, y2-y1_bb+1);
	if (right)
	    CopyRect(right->Data, right->Cols, 0, y1_bb-y1, ntbottom->Data,
		     ntbottom->Cols, x2-right->Cols+1-x1_lb, 0, right->Cols,
		     y2-y1_bb+1);

	LChannelCR *lbottom = ntbottom->PushFwtStepCR(*Filter);
	delete ntbottom;

	// Adjust left border according to the higher resolution level
	NTChannel *ntleft = IFwt(x1_lb, y1_tb, x2_lb, y2_bb);
	NTChannel *ntch_left = ((LChannel*)channel)->
			       IFwt(ch_x1, ch_y1, x2_lb, ch_y2);
	ntleft->Merge(*ntch_left);
	delete ntch_left;

	if (left)
	    ntleft->Merge(*left);
	if (top)
	    CopyRect(top->Data, top->Cols, 0, 0, ntleft->Data, ntleft->Cols,
		     x1-x1_lb, y1-y1_tb, x2_lb-x1+1, top->Rows);
	if (bottom)
	    CopyRect(bottom->Data, bottom->Cols, 0, 0, ntleft->Data,
		     ntleft->Cols, x1-x1_lb, y2-bottom->Rows+1-y1_tb,
		     x2_lb-x1+1, bottom->Rows);

	LChannelCR *lleft = ntleft->PushFwtStepCR(*Filter);
	delete ntleft;

	// Adjust right border according to the higher resolution level
	NTChannel *ntright = IFwt(x1_rb, y1_tb, x2_rb, y2_bb);
	NTChannel *ntch_right = ((LChannel*)channel)->
				IFwt(x1_rb, ch_y1, ch_x2, ch_y2);
	ntright->Merge(*ntch_right);
	delete ntch_right;

	if (right)
	    ntright->Merge(*right);
	if (top)
	    CopyRect(top->Data, top->Cols, x1_rb-x1, 0, ntright->Data,
		     ntright->Cols, 0, y1-y1_tb, x2-x1_rb+1, top->Rows);
	if (bottom)
	    CopyRect(bottom->Data, bottom->Cols, x1_rb-x1, 0, ntright->Data,
		     ntright->Cols, 0, y2-bottom->Rows+1-y1_tb, x2-x1_rb+1,
		     bottom->Rows);

	LChannelCR *lright = ntright->PushFwtStepCR(*Filter);
	delete ntright;

	// Merge the high-pass border-independent (center) subimage parts
	SubBands[SubBand_LH]->
		Merge(*((LChannelCR*)channel)->SubBands[SubBand_LH]);
	SubBands[SubBand_HL]->
		Merge(*((LChannelCR*)channel)->SubBands[SubBand_HL]);
	SubBands[SubBand_HH]->
		Merge(*((LChannelCR*)channel)->SubBands[SubBand_HH]);

	// Recalculate inner and outer border coordinates
	// (without redundant border)
	y1_tb = Max(y1-bw-1, OffsetY);
	int y1_tb_l = GetEven(y1_tb);
	int y1_tb_h = GetOdd(y1_tb);
	y2_bb = Min(y2+bw+1, OffsetY+(int)Rows-1);
	int y2_bb_l = GetOdd(y2_bb);
	int y2_bb_h = GetOdd(y2_bb-1);
	x1_lb = Max(x1-bw-1, OffsetX);
	int x1_lb_l = GetEven(x1_lb);
	int x1_lb_h = GetOdd(x1_lb);
	x2_rb = Min(x2+bw+1, OffsetX+(int)Cols-1);
	int x2_rb_l = GetOdd(x2_rb);
	int x2_rb_h = GetOdd(x2_rb-1);
	y2_tb -= bw;
	int y2_tb_l = GetOdd(y2_tb);
	int y2_tb_h = GetOdd(y2_tb-1);
	y1_bb += bw;
	int y1_bb_l = GetEven(y1_bb);
	int y1_bb_h = GetOdd(y1_bb);
	x2_lb -= bw;
	int x2_lb_l = GetOdd(x2_lb);
	int x2_lb_h = GetOdd(x2_lb-1);
	x1_rb += bw;
	int x1_rb_l = GetEven(x1_rb);
	int x1_rb_h = GetOdd(x1_rb);

	// Merge specified border parts for the high-pass subimages and
	// crop the low-pass parts
	NTChannel *top_ll = CropMergeH(ltop, x1_lb_l, x1_lb_h, y1_tb_l,
				y1_tb_h, x2_rb_l, x2_rb_h, y2_tb_l, y2_tb_h);
	NTChannel *bottom_ll = CropMergeH(lbottom, x1_lb_l, x1_lb_h, y1_bb_l,
				y1_bb_h, x2_rb_l, x2_rb_h, y2_bb_l, y2_bb_h);
	NTChannel *left_ll = CropMergeH(lleft, x1_lb_l, x1_lb_h, y1_tb_l,
				y1_tb_h, x2_lb_l, x2_lb_h, y2_bb_l, y2_bb_h);
	NTChannel *right_ll = CropMergeH(lright, x1_rb_l, x1_rb_h, y1_tb_l,
				y1_tb_h, x2_rb_l, x2_rb_h, y2_bb_l, y2_bb_h);

	if (SubBands[SubBand_LL]->IsLifted())
	    // Merge coarser level taking account of new borders
	    ((LChannel*)SubBands[SubBand_LL])->
		    Merge_rec(((LChannelCR*)channel)->SubBands[SubBand_LL],
			      top_ll, bottom_ll, left_ll, right_ll);
	else {
	    // Merge low-pass subband taking account of new borders
	    SubBands[SubBand_LL]->
		    Merge(*((LChannelCR*)channel)->SubBands[SubBand_LL]);
	    SubBands[SubBand_LL]->Merge(*top_ll);
	    SubBands[SubBand_LL]->Merge(*bottom_ll);
	    SubBands[SubBand_LL]->Merge(*left_ll);
	    SubBands[SubBand_LL]->Merge(*right_ll);
	}

	delete top_ll;
	delete bottom_ll;
	delete left_ll;
	delete right_ll;
    } else {
	// Merge after applying a full IFwt of the subimage and by
	// reconstructing the original structure. This is only the case
	// for one of the most coarse levels.
	NTChannel* ntchannel = IFwt(x1_lb, y1_tb, x2_rb, y2_bb);
	ntchannel->Merge(*channel);

	if (top)
	    ntchannel->Merge(*top);
	if (bottom)
	    ntchannel->Merge(*bottom);
	if (left)
	    ntchannel->Merge(*left);
	if (right)
	    ntchannel->Merge(*right);

	LChannelCR *lchannel = ntchannel->PushFwtStepCR(*Filter);
	delete ntchannel;

	y1_tb = Max(y1-bw-1, OffsetY);
	int y1_tb_l = GetEven(y1_tb);
	int y1_tb_h = GetOdd(y1_tb);
	y2_bb = Min(y2+bw+1, OffsetY+(int)Rows-1);
	int y2_bb_l = GetOdd(y2_bb);
	int y2_bb_h = GetOdd(y2_bb-1);
	x1_lb = Max(x1-bw-1, OffsetX);
	int x1_lb_l = GetEven(x1_lb);
	int x1_lb_h = GetOdd(x1_lb);
	x2_rb = Min(x2+bw+1, OffsetX+(int)Cols-1);
	int x2_rb_l = GetOdd(x2_rb);
	int x2_rb_h = GetOdd(x2_rb-1);

	NTChannel *lchannel_ll = CropMergeH(lchannel, x1_lb_l, x1_lb_h, y1_tb_l,
				 y1_tb_h, x2_rb_l, x2_rb_h, y2_bb_l, y2_bb_h);

	SubBands[SubBand_LL]->Merge(*lchannel_ll);

	delete lchannel_ll;
    }
}

Channel *LChannelCR::DownScale(u_int s, const Wavelet &wavelet)
{
    assert(s != 0);

    if (s > 1) {
	Channel *channel;
	channel = SubBands[SubBand_LL];
	SubBands[SubBand_LL] = NULL;
	Destroy();
	return(channel->DownScale(GetOdd(s), wavelet));
    } else {
	LChannelCR *channel = this->Clone();
	Destroy();
	return(channel);
    }
}



