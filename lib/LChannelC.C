//
//  	LChannelC Class
//
//  $Id: LChannelC.C,v 4.6.2.1.2.1 1999/07/20 16:15:49 geert Exp $
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


const char *LChannelC::rcsid = "$Id: LChannelC.C,v 4.6.2.1.2.1 1999/07/20 16:15:49 geert Exp $";


// ----------------------------------------------------------------------------
//
//	Lifted Channel Abstract Base Class (lifted Cols)
//
// ----------------------------------------------------------------------------


LChannelC::LChannelC(const Wavelet &filter, u_int cols, u_int rows,
		     int offx, int offy)
    : LChannel(filter, 2, cols, rows)
{
    OffsetX = offx;
    OffsetY = offy;
    int offy_l = GetEven(OffsetY);
    int offy_h = GetOdd(OffsetY);
    u_int rlow = Even(OffsetY)? GetEven(Rows): GetOdd(Rows);
    u_int rhigh = Rows - rlow;
    SubBands[SubBand_L] = new NTChannel(Cols, rlow, OffsetX, offy_l);
    SubBands[SubBand_H] = new NTChannel(Cols, rhigh, OffsetX, offy_h);
    Shifts[SubBand_L] = filter.GetShiftL();
    Shifts[SubBand_H] = filter.GetShiftH();
}

LChannelC::LChannelC(const LChannelC &channel)
    : LChannel(channel)
{
}

void LChannelC::GetMask(u_int &maskx, u_int &masky) const
{
    SubBands[SubBand_L]->GetMask(maskx, masky);
    masky = masky<<1 | 1;
}

void LChannelC::Resize(u_int cols, u_int rows)
{
    Channel::Resize(cols, rows);
    u_int rlow = Even(OffsetY)? GetEven(Rows): GetOdd(Rows);
    u_int rhigh = Rows - rlow;
    if (SubBands[SubBand_L])
	SubBands[SubBand_L]->Resize(Cols, rlow);
    if (SubBands[SubBand_H])
	SubBands[SubBand_H]->Resize(Cols, rhigh);
}

void LChannelC::SetOffsetX(int offx)
{
    OffsetX = offx;
    SubBands[SubBand_L]->SetOffsetX(offx);
    SubBands[SubBand_H]->SetOffsetX(offx);
}

void LChannelC::SetOffsetY(int offy)
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
	SubBands[SubBand_L] = lchannel->SubBands[SubBand_L];
	SubBands[SubBand_H] = lchannel->SubBands[SubBand_H];
	Filter = lchannel->Filter;

	lchannel->SubBands[SubBand_L] = NULL;
	lchannel->SubBands[SubBand_H] = NULL;
	lchannel->Filter = NULL;
	delete lchannel;
    } else {
	OffsetY = offy;
	SubBands[SubBand_L]->SetOffsetY(GetEven(offy));
	SubBands[SubBand_H]->SetOffsetY(GetOdd(offy));
    }
}

void LChannelC::Add(const Channel &channel)
{
    assert(channel.IsLifted());
    assert(((LChannel&)channel).GetTransformType() == TT_Cols);
    assert(channel.GetOffsetX() == OffsetX);
    assert(channel.GetOffsetY() == OffsetY);
    assert(channel.GetRows() == Rows);
    assert(channel.GetCols() == Cols);
    SubBands[SubBand_L]->Add(*((LChannel&)channel)[SubBand_L]);
    SubBands[SubBand_H]->Add(*((LChannel&)channel)[SubBand_H]);
}

void LChannelC::Subtract(const Channel &channel)
{
    assert(channel.IsLifted());
    assert(((LChannel&)channel).GetTransformType() == TT_Cols);
    assert(channel.GetOffsetX() == OffsetX);
    assert(channel.GetOffsetY() == OffsetY);
    assert(channel.GetRows() == Rows);
    assert(channel.GetCols() == Cols);
    SubBands[SubBand_L]->Subtract(*((LChannel&)channel)[SubBand_L]);
    SubBands[SubBand_H]->Subtract(*((LChannel&)channel)[SubBand_H]);
}

LChannelC *LChannelC::Diff(const Channel &channel) const
{
    assert(channel.IsLifted());
    assert(((LChannel&)channel).GetTransformType() == TT_Cols);
    assert(channel.GetOffsetX() == OffsetX);
    assert(channel.GetOffsetY() == OffsetY);
    assert(channel.GetRows() == Rows);
    assert(channel.GetCols() == Cols);
    LChannelC *diff = new LChannelC(*Filter);
    diff->Cols = Cols;
    diff->Rows = Rows;
    diff->OffsetX = OffsetX;
    diff->OffsetY = OffsetY;
    diff->SubBands[SubBand_L] = SubBands[SubBand_L]->
			Diff(*((LChannel&)channel)[SubBand_L]);
    diff->SubBands[SubBand_H] = SubBands[SubBand_H]->
			Diff(*((LChannel&)channel)[SubBand_H]);
    return(diff);
}

void LChannelC::Lazy(const NTChannel &source)
{
    assert(source.Data != NULL);
    const PixType *data = source.Data;

    assert(SubBands[SubBand_L] != NULL);
    assert(SubBands[SubBand_H] != NULL);
    assert(SubBands[SubBand_L]->IsLifted() == 0);
    assert(SubBands[SubBand_H]->IsLifted() == 0);
    assert(((NTChannel *)SubBands[SubBand_L])->Data != NULL);
    assert(((NTChannel *)SubBands[SubBand_H])->Data != NULL);
    PixType *l = ((NTChannel *)SubBands[SubBand_L])->Data;
    PixType *h = ((NTChannel *)SubBands[SubBand_H])->Data;
    u_int low = GetRlow();
    u_int high = GetRhigh();
    if (Even(OffsetY)) {
	for (u_int r = 0; r < high; r++) {
	    Copy(data, l, Cols);
	    data += Cols;
	    l += Cols;
	    Copy(data, h, Cols);
	    data += Cols;
	    h += Cols;
	}
	if (low != high)
	    Copy(data, l, Cols);
    } else {
	for (u_int r = 0; r < low; r++) {
	    Copy(data, h, Cols);
	    data += Cols;
	    h += Cols;
	    Copy(data, l, Cols);
	    data += Cols;
	    l += Cols;
	}
	if (low != high)
	    Copy(data, h, Cols);
    }
}

void LChannelC::ILazy(NTChannel &dest) const
{
    assert(dest.Data != NULL);
    PixType *data = dest.Data;

    assert(SubBands[SubBand_L] != NULL);
    assert(SubBands[SubBand_H] != NULL);
    assert(SubBands[SubBand_L]->IsLifted() == 0);
    assert(SubBands[SubBand_H]->IsLifted() == 0);
    assert(((NTChannel *)SubBands[SubBand_L])->Data != NULL);
    assert(((NTChannel *)SubBands[SubBand_H])->Data != NULL);
    const PixType *l = ((NTChannel *)SubBands[SubBand_L])->Data;
    const PixType *h = ((NTChannel *)SubBands[SubBand_H])->Data;
    u_int low = GetRlow();
    u_int high = GetRhigh();
    if (Even(OffsetY)) {
	for (u_int r = 0; r < high; r++) {
	    Copy(l, data, Cols);
	    data += Cols;
	    l += Cols;
	    Copy(h, data, Cols);
	    data += Cols;
	    h += Cols;
	}
	if (low != high)
	    Copy(l, data, Cols);
    } else {
	for (u_int r = 0; r < low; r++) {
	    Copy(h, data, Cols);
	    data += Cols;
	    h += Cols;
	    Copy(l, data, Cols);
	    data += Cols;
	    l += Cols;
	}
	if (low != high)
	    Copy(h, data, Cols);
    }
}

void LChannelC::CakeWalk(void)
{
    assert(SubBands[SubBand_L]->IsLifted() == 0);
    assert(SubBands[SubBand_H]->IsLifted() == 0);
    LiftChannelC lifting((NTChannel *)SubBands[SubBand_L],
			       (NTChannel *)SubBands[SubBand_H]);
    Filter->CakeWalk(lifting);
}

void LChannelC::ICakeWalk(void)
{
    assert(SubBands[SubBand_L]->IsLifted() == 0);
    assert(SubBands[SubBand_H]->IsLifted() == 0);
    LiftChannelC lifting((NTChannel *)SubBands[SubBand_L],
			       (NTChannel *)SubBands[SubBand_H]);
    Filter->ICakeWalk(lifting);
}

void LChannelC::Zip(const LChannelC &channel)
{
    LChannelC *dest =
	    new LChannelC(*Filter, Cols, Rows<<1, OffsetX, OffsetY<<1);

    LChannelC *subband = new LChannelC(*Filter);
    subband->Cols = Cols;
    subband->Rows = Rows;
    subband->OffsetX = OffsetX;
    subband->OffsetY = OffsetY;

    subband->SubBands[SubBand_L] = SubBands[SubBand_L];
    subband->SubBands[SubBand_H] = channel.SubBands[SubBand_L];
    subband->ILazy(*(NTChannel*)dest->SubBands[SubBand_L]);

    subband->SubBands[SubBand_L] = channel.SubBands[SubBand_H];
    subband->SubBands[SubBand_H] = SubBands[SubBand_H];
    subband->ILazy(*(NTChannel*)dest->SubBands[SubBand_H]);

    subband->SubBands[SubBand_L] = NULL;
    subband->SubBands[SubBand_H] = NULL;
    delete subband;

    Rows = dest->Rows;
    OffsetY = dest->OffsetY;
    SubBands[SubBand_L] = dest->SubBands[SubBand_L];
    SubBands[SubBand_H] = dest->SubBands[SubBand_H];

    dest->SubBands[SubBand_L] = NULL;
    dest->SubBands[SubBand_H] = NULL;
    delete dest;
}

NTChannel *LChannelC::IFwt(int x1, int y1, int x2, int y2) const
{
    int bwstart = Min(Filter->GetHStart(), Filter->GetGStart());
    int bwend = Max(Filter->GetHEnd(), Filter->GetGEnd());
    int bw = Max(-bwstart, bwend);

    // Calculate new coordinates taking account of the needed borders
    int yb1 = Max((int)y1-bw-1, OffsetY);
    int yb2 = Min((int)y2+bw+1, OffsetY+(int)Rows-1);

    LChannelC *lchannel = new LChannelC(*Filter);
    lchannel->Cols = x2-x1+1;
    lchannel->Rows = yb2-yb1+1;
    lchannel->OffsetX = x1;
    lchannel->OffsetY = yb1;

    int yb1_l = GetEven(yb1);
    int yb1_h = GetOdd(yb1);
    int yb2_l = GetOdd(yb2);
    int yb2_h = GetOdd(yb2-1);

    // Crop subimages of all subbands
    if (SubBands[SubBand_L]->IsLifted())
	lchannel->SubBands[SubBand_L] = ((LChannel*)SubBands[SubBand_L])->
					IFwt(x1, yb1_l, x2, yb2_l);
    else
	lchannel->SubBands[SubBand_L] =
			SubBands[SubBand_L]->Crop(x1, yb1_l, x2, yb2_l);

    lchannel->SubBands[SubBand_H] =
		    SubBands[SubBand_H]->Crop(x1, yb1_h, x2, yb2_h);

    // Apply invers transform and crop redundant borders
    NTChannel *ntchannel = lchannel->IFwtStep();
    NTChannel *channel = ntchannel->Crop(x1, y1, x2, y2);
    delete lchannel;
    delete ntchannel;

    return(channel);
}

NTChannel *LChannelC::CropMergeH(LChannelC *channel, int x1, int y1_l, int y1_h,
				 int x2, int y2_l, int y2_h)
{
	assert(x1 <= x2);
	assert(y1_l <= y2_l);
	assert(y1_h <= y2_h);

	// Crop the specified subimages of both subbands
	// None of the subbands is tranformed!!!!!
	NTChannel *channel_l = ((NTChannel*)channel->SubBands[SubBand_L])->
			       Crop(x1, y1_l, x2, y2_l);
	NTChannel *channel_h = ((NTChannel*)channel->SubBands[SubBand_H])->
			       Crop(x1, y1_h, x2, y2_h);

	// Merge high-pass subband in the subband of the current channel
	SubBands[SubBand_H]->Merge(*channel_h);

	delete channel;
	delete channel_h;

	return(channel_l);
}

LChannelC *LChannelC::Crop_rec(int x1, int y1, int x2, int y2,
			       NTChannel *top, NTChannel *bottom,
			       NTChannel *left, NTChannel *right) const
{
    int bwstart = Min(Filter->GetHStart(), Filter->GetGStart());
    int bwend = Max(Filter->GetHEnd(), Filter->GetGEnd());
    int bw = Max(-bwstart, bwend);

    LChannelC *channel;

    int y1_l = GetEven(y1);
    int y1_h = GetOdd(y1);
    int y2_l = GetOdd(y2);
    int y2_h = GetOdd(y2-1);

    // Calculate new coordinates taking account of the needed borders
    int y2_tb = top? y1+(bw<<1)+(int)top->Rows: y1+(bw<<1);
    int y1_bb = bottom? y2-(bw<<1)-(int)bottom->Rows: y2-(bw<<1);

    if (y1_bb-y2_tb > 1) {
	channel = new LChannelC(*Filter);
	channel->Cols = x2-x1+1;
	channel->Rows = y2-y1+1;
	channel->OffsetX = x1;
	channel->OffsetY = y1;

	// Crop the high-pass subband (without borders)
	channel->SubBands[SubBand_H] =
			SubBands[SubBand_H]->Crop(x1, y1_h, x2, y2_h);

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

	LChannelC *ltop = nttop->PushFwtStepC(*Filter);
	delete nttop;

	y2_tb -= bw;
	int y2_tb_l = GetOdd(y2_tb);
	int y2_tb_h = GetOdd(y2_tb-1);

	NTChannel *top_l = channel->CropMergeH(ltop, x1, y1_l, y1_h,
					       x2, y2_tb_l, y2_tb_h);

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

	LChannelC *lbottom = ntbottom->PushFwtStepC(*Filter);
	delete ntbottom;

	y1_bb += bw;
	int y1_bb_l = GetEven(y1_bb);
	int y1_bb_h = GetOdd(y1_bb);

	NTChannel *bottom_l = channel->CropMergeH(lbottom, x1, y1_bb_l, y1_bb_h,
						  x2, y2_l, y2_h);

	// Adjust left border according to the higher resolution level
	NTChannel *left_l = NULL;

	if (left) {
	    LChannelC *lleft = left->PushFwtStepC(*Filter);

	    left_l = (NTChannel*)lleft->SubBands[SubBand_L];
	    lleft->SubBands[SubBand_L] = NULL;

	    channel->SubBands[SubBand_H]->
			Merge(*(NTChannel*)lleft->SubBands[SubBand_H]);

	    delete lleft;
	}

	// Adjust right border according to the higher resolution level
	NTChannel *right_l = NULL;

	if (right) {
	    LChannelC *lright = right->PushFwtStepC(*Filter);

	    right_l = (NTChannel*)lright->SubBands[SubBand_L];
	    lright->SubBands[SubBand_L] = NULL;

	    channel->SubBands[SubBand_H]->
			Merge(*(NTChannel*)lright->SubBands[SubBand_H]);

	    delete lright;
	}

	if (SubBands[SubBand_L]->IsLifted())
	    // Crop coarser level taking account of new borders
	    channel->SubBands[SubBand_L] = ((LChannel*)SubBands[SubBand_L])->
				    Crop_rec(x1, y1_l, x2, y2_l, top_l,
					     bottom_l, left_l, right_l);
	else {
	    // Crop low-pass subband taking account of new borders
	    channel->SubBands[SubBand_L] = SubBands[SubBand_L]->
					   Crop(x1, y1_l, x2, y2_l);

	    channel->SubBands[SubBand_L]->Merge(*top_l);
	    channel->SubBands[SubBand_L]->Merge(*bottom_l);
	    if (left_l)
		channel->SubBands[SubBand_L]->Merge(*left_l);
	    if (right_l)
		channel->SubBands[SubBand_L]->Merge(*right_l);
	}

	delete top_l;
	delete bottom_l;
	if (left_l)
	    delete left_l;
	if (right_l)
	    delete right_l;
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

	channel = (LChannelC*)ntchannel->Fwt(transform, depth);

	delete[] transform;
	delete ntchannel;
    }

    return(channel);
}

void LChannelC::Merge_rec(const Channel *channel,
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
    int x1, y1, x2, y2, y1_tb, y2_tb, y1_bb, y2_bb;

    // Calculate inner and outer border coordinates
    if (top) {
	y1 = top->OffsetY;
	y1_tb = y1-(bw<<1)-1;
	y2_tb = y1+(int)top->Rows-1+(bw<<1);
    } else {
	y1 = ch_y1;
	y1_tb = y1-(bw<<1)-1;
	y2_tb = y1+(bw<<1);
    }
    if (bottom) {
	y2 = bottom->OffsetY+(int)bottom->Rows-1;
	y1_bb = y2-(int)bottom->Rows+1-(bw<<1);
	y2_bb = y2+(bw<<1)+1;
    } else {
	y2 = ch_y2;
	y1_bb = y2-(bw<<1);
	y2_bb = y2+(bw<<1)+1;
    }
    x1 = left? left->OffsetX: ch_x1;
    x2 = right? right->OffsetX+(int)right->Cols-1: ch_x2;

    y1_tb = Max(y1_tb, OffsetY);
    y2_bb = Min(y2_bb, OffsetY+(int)Rows-1);

    if (y1_bb-y2_tb > 1 && channel->IsLifted() &&
		((LChannel*)channel)->GetTransformType() == TT_Cols) {
	// Adjust top border according to the higher resolution level
	NTChannel *nttop = IFwt(x1, y1_tb, x2, y2_tb);
	NTChannel *ntch_top = ((LChannel*)channel)->
			      IFwt(ch_x1, ch_y1, ch_x2, y2_tb);
	nttop->Merge(*ntch_top);
	delete ntch_top;

	if (top)
	    nttop->Merge(*top);
	if (left)
	    CopyRect(left->Data, left->Cols, 0, 0, nttop->Data, nttop->Cols,
		     0, y1-y1_tb, left->Cols, y2_tb-y1+1);
	if (right)
	    CopyRect(right->Data, right->Cols, 0, 0, nttop->Data, nttop->Cols,
		     x2-right->Cols+1-x1, y1-y1_tb, right->Cols, y2_tb-y1+1);

	LChannelC *ltop = nttop->PushFwtStepC(*Filter);
	delete nttop;

	// Adjust bottom border according to the higher resolution level
	NTChannel *ntbottom = IFwt(x1, y1_bb, x2, y2_bb);
	NTChannel *ntch_bottom = ((LChannel*)channel)->
				 IFwt(ch_x1, y1_bb, ch_x2, ch_y2);
	ntbottom->Merge(*ntch_bottom);
	delete ntch_bottom;

	if (bottom)
	    ntbottom->Merge(*bottom);
	if (left)
	    CopyRect(left->Data, left->Cols, 0, y1_bb-y1, ntbottom->Data,
		     ntbottom->Cols, 0, 0, left->Cols, y2-y1_bb+1);
	if (right)
	    CopyRect(right->Data, right->Cols, 0, y1_bb-y1, ntbottom->Data,
		     ntbottom->Cols, x2-right->Cols+1-x1, 0, right->Cols,
		     y2-y1_bb+1);

	LChannelC *lbottom = ntbottom->PushFwtStepC(*Filter);
	delete ntbottom;

	// Adjust left border according to the higher resolution level
	LChannelC *lleft = NULL;

	if (left) {
	    NTChannel *ntleft = IFwt(x1, y1_tb, x1+(int)left->Cols-1, y2_bb);
	    ntleft->Merge(*left);

	    lleft = ntleft->PushFwtStepC(*Filter);
	    delete ntleft;
	}

	// Adjust right border according to the higher resolution level
	LChannelC *lright = NULL;

	if (right) {
	    NTChannel *ntright = IFwt(x2-(int)right->Cols+1, y1_tb, x2, y2_bb);
	    ntright->Merge(*right);

	    lright = ntright->PushFwtStepC(*Filter);
	    delete ntright;
	}

	// Merge the high-pass border-independent (center) subimage part
	SubBands[SubBand_H]->
		Merge(*((LChannelC*)channel)->SubBands[SubBand_H]);

	// Recalculate inner and outer border coordinates
	// (without redundant border)
	y1_tb = Max(y1-bw-1, OffsetY);
	int y1_tb_l = GetEven(y1_tb);
	int y1_tb_h = GetOdd(y1_tb);
	y2_bb = Min(y2+bw+1, OffsetY+(int)Rows-1);
	int y2_bb_l = GetOdd(y2_bb);
	int y2_bb_h = GetOdd(y2_bb-1);
	y2_tb -= bw;
	int y2_tb_l = GetOdd(y2_tb);
	int y2_tb_h = GetOdd(y2_tb-1);
	y1_bb += bw;
	int y1_bb_l = GetEven(y1_bb);
	int y1_bb_h = GetOdd(y1_bb);

	// Merge specified border parts for the high-pass subimage and
	// crop the low-pass parts
	NTChannel *top_l = CropMergeH(ltop, x1, y1_tb_l, y1_tb_h,
				      x2, y2_tb_l, y2_tb_h);
	NTChannel *bottom_l = CropMergeH(lbottom, x1, y1_bb_l, y1_bb_h,
					 x2, y2_bb_l, y2_bb_h);
	NTChannel *left_l = lleft? CropMergeH(lleft, x1, y1_tb_l, y1_tb_h,
					x1+(int)lleft->Cols-1, y2_bb_l, y2_bb_h)
					: (NTChannel*)NULL;
	NTChannel *right_l = lright? CropMergeH(lright, x2-(int)lright->Cols+1,
					y1_tb_l, y1_tb_h, x2, y2_bb_l, y2_bb_h)
					: (NTChannel*)NULL;

	if (SubBands[SubBand_L]->IsLifted())
	    // Merge coarser level taking account of new borders
	    ((LChannel*)SubBands[SubBand_L])->
		    Merge_rec(((LChannelC*)channel)->SubBands[SubBand_L],
			      top_l, bottom_l, left_l, right_l);
	else {
	    // Merge low-pass subband taking account of new borders
	    SubBands[SubBand_L]->
		    Merge(*((LChannelC*)channel)->SubBands[SubBand_L]);
	    SubBands[SubBand_L]->Merge(*top_l);
	    SubBands[SubBand_L]->Merge(*bottom_l);
	    if (lleft)
		SubBands[SubBand_L]->Merge(*left_l);
	    if (lright)
		SubBands[SubBand_L]->Merge(*right_l);
	}

	delete top_l;
	delete bottom_l;
	if (lleft)
	    delete left_l;
	if (lright)
	    delete right_l;
    } else {
	// Merge after applying a full IFwt of the subimage and by
	// reconstructing the original structure. This is only the case
	// for one of the most coarse levels.
	NTChannel* ntchannel = IFwt(x1, y1_tb, x2, y2_bb);
	ntchannel->Merge(*channel);

	if (top)
	    ntchannel->Merge(*top);
	if (bottom)
	    ntchannel->Merge(*bottom);
	if (left)
	    ntchannel->Merge(*left);
	if (right)
	    ntchannel->Merge(*right);

	LChannelC *lchannel = ntchannel->PushFwtStepC(*Filter);
	delete ntchannel;

	y1_tb = Max(y1-bw-1, OffsetY);
	int y1_tb_l = GetEven(y1_tb);
	int y1_tb_h = GetOdd(y1_tb);
	y2_bb = Min(y2+bw+1, OffsetY+(int)Rows-1);
	int y2_bb_l = GetOdd(y2_bb);
	int y2_bb_h = GetOdd(y2_bb-1);

	NTChannel *lchannel_l = CropMergeH(lchannel, x1, y1_tb_l, y1_tb_h,
					   x2, y2_bb_l, y2_bb_h);

	SubBands[SubBand_L]->Merge(*lchannel_l);

	delete lchannel_l;
    }
}

Channel *LChannelC::DownScale(u_int s, const Wavelet &wavelet)
{
    assert(s != 0);

    if (s > 1) {
	Channel *channel;
	if (SubBands[SubBand_L]->IsLifted())
	    if (((LChannel*)SubBands[SubBand_L])->
			GetTransformType() == TT_Rows) {
		channel = ((LChannel*)SubBands[SubBand_L])->SubBands[SubBand_L];
		((LChannel*)SubBands[SubBand_L])->SubBands[SubBand_L] = NULL;
	    } else {
		NTChannel *ntchannel = ((LChannel*)SubBands[SubBand_L])->IFwt();
		LChannelR *lchannel = ntchannel->PushFwtStepR(wavelet);
		channel = lchannel->SubBands[SubBand_L];
		lchannel->SubBands[SubBand_L] = NULL;
		delete ntchannel;
		delete lchannel;
	    }
	else {
	    LChannelR *lchannel = ((NTChannel*)SubBands[SubBand_L])->
				    PushFwtStepR(wavelet);
	    channel = lchannel->SubBands[SubBand_L];
	    lchannel->SubBands[SubBand_L] = NULL;
	    delete lchannel;
	}
	Destroy();
	return(channel->DownScale(GetOdd(s), wavelet));
    } else {
	LChannelC *channel = this->Clone();
	Destroy();
	return(channel);
    }
}

