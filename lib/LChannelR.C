//
//  	LChannelR Class
//
//  $Id: LChannelR.C,v 4.6.2.1.2.1 1999/07/20 16:15:51 geert Exp $
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


const char *LChannelR::rcsid = "$Id: LChannelR.C,v 4.6.2.1.2.1 1999/07/20 16:15:51 geert Exp $";


// ----------------------------------------------------------------------------
//
//	Lifted Channel Abstract Base Class (lifted Rows)
//
// ----------------------------------------------------------------------------


LChannelR::LChannelR(const Wavelet &filter, u_int cols, u_int rows,
		     int offx, int offy)
    : LChannel(filter, 2, cols, rows)
{
    OffsetX = offx;
    OffsetY = offy;
    int offx_l = GetEven(OffsetX);
    int offx_h = GetOdd(OffsetX);
    u_int clow = Even(OffsetX)? GetEven(Cols): GetOdd(Cols);
    u_int chigh = Cols - clow;
    SubBands[SubBand_L] = new NTChannel(clow, Rows, offx_l, OffsetY);
    SubBands[SubBand_H] = new NTChannel(chigh, Rows, offx_h, OffsetY);
    Shifts[SubBand_L] = filter.GetShiftL();
    Shifts[SubBand_H] = filter.GetShiftH();
}

LChannelR::LChannelR(const LChannelR &channel)
    : LChannel(channel)
{
}

void LChannelR::GetMask(u_int &maskx, u_int &masky) const
{
    SubBands[SubBand_L]->GetMask(maskx, masky);
    maskx = maskx<<1 | 1;
}

void LChannelR::Resize(u_int cols, u_int rows)
{
    Channel::Resize(cols, rows);
    u_int clow = Even(OffsetX)? GetEven(Cols): GetOdd(Cols);
    u_int chigh = Cols - clow;
    if (SubBands[SubBand_L])
	SubBands[SubBand_L]->Resize(clow, Rows);
    if (SubBands[SubBand_H])
	SubBands[SubBand_H]->Resize(chigh, Rows);
}

void LChannelR::SetOffsetX(int offx)
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
	SubBands[SubBand_L] = lchannel->SubBands[SubBand_L];
	SubBands[SubBand_H] = lchannel->SubBands[SubBand_H];
	Filter = lchannel->Filter;

	lchannel->SubBands[SubBand_L] = NULL;
	lchannel->SubBands[SubBand_H] = NULL;
	lchannel->Filter = NULL;
	delete lchannel;
    } else {
	OffsetX = offx;
	SubBands[SubBand_L]->SetOffsetX(GetEven(offx));
	SubBands[SubBand_H]->SetOffsetX(GetOdd(offx));
    }
}

void LChannelR::SetOffsetY(int offy)
{
    OffsetY = offy;
    SubBands[SubBand_L]->SetOffsetY(offy);
    SubBands[SubBand_H]->SetOffsetY(offy);
}

void LChannelR::Add(const Channel &channel)
{
    assert(channel.IsLifted());
    assert(((LChannel&)channel).GetTransformType() == TT_Rows);
    assert(channel.GetOffsetX() == OffsetX);
    assert(channel.GetOffsetY() == OffsetY);
    assert(channel.GetRows() == Rows);
    assert(channel.GetCols() == Cols);
    SubBands[SubBand_L]->Add(*((LChannel&)channel)[SubBand_L]);
    SubBands[SubBand_H]->Add(*((LChannel&)channel)[SubBand_H]);
}

void LChannelR::Subtract(const Channel &channel)
{
    assert(channel.IsLifted());
    assert(((LChannel&)channel).GetTransformType() == TT_Rows);
    assert(channel.GetOffsetX() == OffsetX);
    assert(channel.GetOffsetY() == OffsetY);
    assert(channel.GetRows() == Rows);
    assert(channel.GetCols() == Cols);
    SubBands[SubBand_L]->Subtract(*((LChannel&)channel)[SubBand_L]);
    SubBands[SubBand_H]->Subtract(*((LChannel&)channel)[SubBand_H]);
}

LChannelR *LChannelR::Diff(const Channel &channel) const
{
    assert(channel.IsLifted());
    assert(((LChannel&)channel).GetTransformType() == TT_Rows);
    assert(channel.GetOffsetX() == OffsetX);
    assert(channel.GetOffsetY() == OffsetY);
    assert(channel.GetRows() == Rows);
    assert(channel.GetCols() == Cols);
    LChannelR *diff = new LChannelR(*Filter);
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

void LChannelR::Lazy(const NTChannel &source)
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
    u_int low = GetClow();
    u_int high = GetChigh();
    if (Even(OffsetX))
	for (u_int r = 0; r < Rows; r++) {
	    for (u_int c = 0; c < high; c++) {
		*l++ = *data++;
		*h++ = *data++;
	    }
	    if (low != high)
		*l++ = *data++;
	}
    else
	for (u_int r = 0; r < Rows; r++) {
	    for (u_int c = 0; c < low; c++) {
		*h++ = *data++;
		*l++ = *data++;
	    }
	    if (low != high)
		*h++ = *data++;
	}
}

void LChannelR::ILazy(NTChannel &dest) const
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
    u_int low = GetClow();
    u_int high = GetChigh();
    if (Even(OffsetX))
	for (u_int r = 0; r < Rows; r++) {
	    for (u_int c = 0; c < high; c++) {
		*data++ = *l++;
		*data++ = *h++;
	    }
	    if (low != high)
		*data++ = *l++;
	}
    else
	for (u_int r = 0; r < Rows; r++) {
	    for (u_int c = 0; c < low; c++) {
		*data++ = *h++;
		*data++ = *l++;
	    }
	    if (low != high)
		*data++ = *h++;
	}
}

void LChannelR::CakeWalk(void)
{
    assert(SubBands[SubBand_L]->IsLifted() == 0);
    assert(SubBands[SubBand_H]->IsLifted() == 0);
    LiftChannelR lifting((NTChannel *)SubBands[SubBand_L],
			       (NTChannel *)SubBands[SubBand_H]);
    Filter->CakeWalk(lifting);
}

void LChannelR::ICakeWalk(void)
{
    assert(SubBands[SubBand_L]->IsLifted() == 0);
    assert(SubBands[SubBand_H]->IsLifted() == 0);
    LiftChannelR lifting((NTChannel *)SubBands[SubBand_L],
			       (NTChannel *)SubBands[SubBand_H]);
    Filter->ICakeWalk(lifting);
}

void LChannelR::Zip(const LChannelR &channel)
{
    LChannelR *dest =
	    new LChannelR(*Filter, Cols<<1, Rows, OffsetX<<1, OffsetY);

    LChannelR *subband = new LChannelR(*Filter);
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

    Cols = dest->Cols;
    OffsetX = dest->OffsetX;
    SubBands[SubBand_L] = dest->SubBands[SubBand_L];
    SubBands[SubBand_H] = dest->SubBands[SubBand_H];

    dest->SubBands[SubBand_L] = NULL;
    dest->SubBands[SubBand_H] = NULL;
    delete dest;
}

NTChannel *LChannelR::IFwt(int x1, int y1, int x2, int y2) const
{
    int bwstart = Min(Filter->GetHStart(), Filter->GetGStart());
    int bwend = Max(Filter->GetHEnd(), Filter->GetGEnd());
    int bw = Max(-bwstart, bwend);

    // Calculate new coordinates taking account of the needed borders
    int xb1 = Max((int)x1-bw-1, OffsetX);
    int xb2 = Min((int)x2+bw+1, OffsetX+(int)Cols-1);

    LChannelR *lchannel = new LChannelR(*Filter);
    lchannel->Cols = xb2-xb1+1;
    lchannel->Rows = y2-y1+1;
    lchannel->OffsetX = xb1;
    lchannel->OffsetY = y1;

    int xb1_l = GetEven(xb1);
    int xb1_h = GetOdd(xb1);
    int xb2_l = GetOdd(xb2);
    int xb2_h = GetOdd(xb2-1);

    // Crop subimages of all subbands
    if (SubBands[SubBand_L]->IsLifted())
	lchannel->SubBands[SubBand_L] = ((LChannel*)SubBands[SubBand_L])->
					IFwt(xb1_l, y1, xb2_l, y2);
    else
	lchannel->SubBands[SubBand_L] =
			SubBands[SubBand_L]->Crop(xb1_l, y1, xb2_l, y2);

    lchannel->SubBands[SubBand_H] =
		    SubBands[SubBand_H]->Crop(xb1_h, y1, xb2_h, y2);

    // Apply invers transform and crop redundant borders
    NTChannel *ntchannel = lchannel->IFwtStep();
    NTChannel *channel = ntchannel->Crop(x1, y1, x2, y2);
    delete lchannel;
    delete ntchannel;

    return(channel);
}

NTChannel *LChannelR::CropMergeH(LChannelR *channel, int x1_l, int x1_h, int y1,
				 int x2_l, int x2_h, int y2)
{
	assert(x1_l <= x2_l);
	assert(x1_h <= x2_h);
	assert(y1 <= y2);

	// Crop the specified subimages of both subbands
	// None of the subbands is tranformed!!!!!
	NTChannel *channel_l = ((NTChannel*)channel->SubBands[SubBand_L])->
				Crop(x1_l, y1, x2_l, y2);
	NTChannel *channel_h = ((NTChannel*)channel->SubBands[SubBand_H])->
				Crop(x1_h, y1, x2_h, y2);

	// Merge high-pass subband in the subband of the current channel
	SubBands[SubBand_H]->Merge(*channel_h);

	delete channel;
	delete channel_h;

	return(channel_l);
}

LChannelR *LChannelR::Crop_rec(int x1, int y1, int x2, int y2,
			       NTChannel *top, NTChannel *bottom,
			       NTChannel *left, NTChannel *right) const
{
    int bwstart = Min(Filter->GetHStart(), Filter->GetGStart());
    int bwend = Max(Filter->GetHEnd(), Filter->GetGEnd());
    int bw = Max(-bwstart, bwend);

    LChannelR *channel;

    int x1_l = GetEven(x1);
    int x1_h = GetOdd(x1);
    int x2_l = GetOdd(x2);
    int x2_h = GetOdd(x2-1);

    // Calculate new coordinates taking account of the needed borders
    int x2_lb = left? x1+(bw<<1)+(int)left->Cols: x1+(bw<<1);
    int x1_rb = right? x2-(bw<<1)-(int)right->Cols: x2-(bw<<1);

    if (x1_rb-x2_lb > 1) {
	channel = new LChannelR(*Filter);
	channel->Cols = x2-x1+1;
	channel->Rows = y2-y1+1;
	channel->OffsetX = x1;
	channel->OffsetY = y1;

	// Crop the high-pass subband (without borders)
	channel->SubBands[SubBand_H] =
			SubBands[SubBand_H]->Crop(x1_h, y1, x2_h, y2);

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

	LChannelR *lleft = ntleft->PushFwtStepR(*Filter);
	delete ntleft;

	x2_lb -= bw;
	int x2_lb_l = GetOdd(x2_lb);
	int x2_lb_h = GetOdd(x2_lb-1);

	NTChannel *left_l = channel->CropMergeH(lleft, x1_l, x1_h, y1,
						x2_lb_l, x2_lb_h, y2);

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

	LChannelR *lright = ntright->PushFwtStepR(*Filter);
	delete ntright;

	x1_rb += bw;
	int x1_rb_l = GetEven(x1_rb);
	int x1_rb_h = GetOdd(x1_rb);

	NTChannel *right_l = channel->CropMergeH(lright, x1_rb_l, x1_rb_h, y1,
						 x2_l, x2_h, y2);

	// Adjust top border according to the higher resolution level
	NTChannel *top_l = NULL;

	if (top) {
	    LChannelR *ltop = top->PushFwtStepR(*Filter);

	    top_l = (NTChannel*)ltop->SubBands[SubBand_L];
	    ltop->SubBands[SubBand_L] = NULL;

	    channel->SubBands[SubBand_H]->
			Merge(*(NTChannel*)ltop->SubBands[SubBand_H]);

	    delete ltop;
	}

	// Adjust bottom border according to the higher resolution level
	NTChannel *bottom_l = NULL;

	if (bottom) {
	    LChannelR *lbottom = bottom->PushFwtStepR(*Filter);

	    bottom_l = (NTChannel*)lbottom->SubBands[SubBand_L];
	    lbottom->SubBands[SubBand_L] = NULL;

	    channel->SubBands[SubBand_H]->
			Merge(*(NTChannel*)lbottom->SubBands[SubBand_H]);

	    delete lbottom;
	}

	if (SubBands[SubBand_L]->IsLifted())
	    // Crop coarser level taking account of new borders
	    channel->SubBands[SubBand_L] = ((LChannel*)SubBands[SubBand_L])->
				    Crop_rec(x1_l, y1, x2_l, y2, top_l,
					     bottom_l, left_l, right_l);
	else {
	    // Crop low-pass subband taking account of new borders
	    channel->SubBands[SubBand_L] = SubBands[SubBand_L]->
					   Crop(x1_l, y1, x2_l, y2);

	    channel->SubBands[SubBand_L]->Merge(*left_l);
	    channel->SubBands[SubBand_L]->Merge(*right_l);
	    if (top_l)
		channel->SubBands[SubBand_L]->Merge(*top_l);
	    if (bottom_l)
		channel->SubBands[SubBand_L]->Merge(*bottom_l);
	}

	delete left_l;
	delete right_l;
	if (top_l)
	    delete top_l;
	if (bottom_l)
	    delete bottom_l;
    }
    else {
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

	channel = (LChannelR*)ntchannel->Fwt(transform, depth);

	delete[] transform;
	delete ntchannel;
    }

    return(channel);
}

void LChannelR::Merge_rec(const Channel *channel,
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
    int x1, y1, x2, y2, x1_lb, x2_lb, x1_rb, x2_rb;

    // Calculate inner and outer border coordinates
    y1 = top? top->OffsetY: ch_y1;
    y2 = bottom? bottom->OffsetY+(int)bottom->Rows-1: ch_y2;
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

    x1_lb = Max(x1_lb, OffsetX);
    x2_rb = Min(x2_rb, OffsetX+(int)Cols-1);

    if (x1_rb-x2_lb > 1 && channel->IsLifted() &&
		((LChannel*)channel)->GetTransformType() == TT_Rows) {
	// Adjust left border according to the higher resolution level
	NTChannel *ntleft = IFwt(x1_lb, y1, x2_lb, y2);
	NTChannel *ntch_left = ((LChannel*)channel)->
			       IFwt(ch_x1, ch_y1, x2_lb, ch_y2);
	ntleft->Merge(*ntch_left);
	delete ntch_left;

	if (left)
	    ntleft->Merge(*left);
	if (top)
	    CopyRect(top->Data, top->Cols, 0, 0, ntleft->Data, ntleft->Cols,
		     x1-x1_lb, 0, x2_lb-x1+1, top->Rows);
	if (bottom)
	    CopyRect(bottom->Data, bottom->Cols, 0, 0, ntleft->Data,
		     ntleft->Cols, x1-x1_lb, y2-bottom->Rows+1-y1,
		     x2_lb-x1+1, bottom->Rows);

	LChannelR *lleft = ntleft->PushFwtStepR(*Filter);
	delete ntleft;

	// Adjust right border according to the higher resolution level
	NTChannel *ntright = IFwt(x1_rb, y1, x2_rb, y2);
	NTChannel *ntch_right = ((LChannel*)channel)->
				IFwt(x1_rb, ch_y1, ch_x2, ch_y2);
	ntright->Merge(*ntch_right);
	delete ntch_right;

	if (right)
	    ntright->Merge(*right);
	if (top)
	    CopyRect(top->Data, top->Cols, x1_rb-x1, 0, ntright->Data,
		     ntright->Cols, 0, 0, x2-x1_rb+1, top->Rows);
	if (bottom)
	    CopyRect(bottom->Data, bottom->Cols, x1_rb-x1, 0, ntright->Data,
		     ntright->Cols, 0, y2-bottom->Rows+1-y1, x2-x1_rb+1,
		     bottom->Rows);

	LChannelR *lright = ntright->PushFwtStepR(*Filter);
	delete ntright;

	// Adjust top border according to the higher resolution level
	LChannelR *ltop = NULL;

	if (top) {
	    NTChannel *nttop = IFwt(x1_lb, y1, x2_rb, y1+(int)top->Rows-1);
	    nttop->Merge(*top);

	    ltop = nttop->PushFwtStepR(*Filter);
	    delete nttop;
	}

	// Adjust bottom border according to the higher resolution level
	LChannelR *lbottom = NULL;

	if (bottom) {
	    NTChannel *ntbottom =
			    IFwt(x1_lb, y2-(int)bottom->Rows+1, x2_rb, y2);
	    ntbottom->Merge(*bottom);

	    lbottom = ntbottom->PushFwtStepR(*Filter);
	    delete ntbottom;
	}

	// Merge the high-pass border-independent (center) subimage part
	SubBands[SubBand_H]->
		Merge(*((LChannelR*)channel)->SubBands[SubBand_H]);

	// Recalculate inner and outer border coordinates
	// (without redundant border)
	x1_lb = Max(x1-bw-1, OffsetX);
	int x1_lb_l = GetEven(x1_lb);
	int x1_lb_h = GetOdd(x1_lb);
	x2_rb = Min(x2+bw+1, OffsetX+(int)Cols-1);
	int x2_rb_l = GetOdd(x2_rb);
	int x2_rb_h = GetOdd(x2_rb-1);
	x2_lb -= bw;
	int x2_lb_l = GetOdd(x2_lb);
	int x2_lb_h = GetOdd(x2_lb-1);
	x1_rb += bw;
	int x1_rb_l = GetEven(x1_rb);
	int x1_rb_h = GetOdd(x1_rb);

	// Merge specified border parts for the high-pass subimage and
	// crop the low-pass parts
	NTChannel *left_l = CropMergeH(lleft, x1_lb_l, x1_lb_h, y1,
				       x2_lb_l, x2_lb_h, y2);
	NTChannel *right_l = CropMergeH(lright, x1_rb_l, x1_rb_h, y1,
					x2_rb_l, x2_rb_h, y2);
	NTChannel *top_l = ltop? CropMergeH(ltop, x1_lb_l, x1_lb_h, y1,
				x2_rb_l, x2_rb_h, y1+(int)ltop->Rows-1)
				: (NTChannel*)NULL;
	NTChannel *bottom_l = lbottom? CropMergeH(lbottom, x1_lb_l, x1_lb_h,
				y2-(int)lbottom->Rows+1, x2_rb_l, x2_rb_h, y2)
				: (NTChannel*)NULL;

	if (SubBands[SubBand_L]->IsLifted())
	    // Merge coarser level taking account of new borders
	    ((LChannel*)SubBands[SubBand_L])->
		    Merge_rec(((LChannelR*)channel)->SubBands[SubBand_L],
			      top_l, bottom_l, left_l, right_l);
	else {
	    // Merge low-pass subband taking account of new borders
	    SubBands[SubBand_L]->
		    Merge(*((LChannelR*)channel)->SubBands[SubBand_L]);
	    SubBands[SubBand_L]->Merge(*left_l);
	    SubBands[SubBand_L]->Merge(*right_l);
	    if (ltop)
		SubBands[SubBand_L]->Merge(*top_l);
	    if (lbottom)
		SubBands[SubBand_L]->Merge(*bottom_l);
	}

	delete left_l;
	delete right_l;
	if (ltop)
	    delete top_l;
	if (lbottom)
	    delete bottom_l;
    } else {
	// Merge after applying a full IFwt of the subimage and by
	// reconstructing the original structure. This is only the case
	// for one of the most coarse levels.
	NTChannel* ntchannel = IFwt(x1_lb, y1, x2_rb, y2);
	ntchannel->Merge(*channel);

	if (top)
	    ntchannel->Merge(*top);
	if (bottom)
	    ntchannel->Merge(*bottom);
	if (left)
	    ntchannel->Merge(*left);
	if (right)
	    ntchannel->Merge(*right);

	LChannelR *lchannel = ntchannel->PushFwtStepR(*Filter);
	delete ntchannel;

	x1_lb = Max(x1-bw-1, OffsetX);
	int x1_lb_l = GetEven(x1_lb);
	int x1_lb_h = GetOdd(x1_lb);
	x2_rb = Min(x2+bw+1, OffsetX+(int)Cols-1);
	int x2_rb_l = GetOdd(x2_rb);
	int x2_rb_h = GetOdd(x2_rb-1);

	NTChannel *lchannel_l = CropMergeH(lchannel, x1_lb_l, x1_lb_h, y1,
					   x2_rb_l, x2_rb_h, y2);

	SubBands[SubBand_L]->Merge(*lchannel_l);

	delete lchannel_l;
    }
}

Channel *LChannelR::DownScale(u_int s, const Wavelet &wavelet)
{
    assert(s != 0);

    if (s > 1) {
	Channel *channel;
	if (SubBands[SubBand_L]->IsLifted())
	    if (((LChannel*)SubBands[SubBand_L])->
			    GetTransformType() == TT_Cols) {
		channel = ((LChannel*)SubBands[SubBand_L])->SubBands[SubBand_L];
		((LChannel*)SubBands[SubBand_L])->SubBands[SubBand_L] = NULL;
	    } else {
		NTChannel *ntchannel = ((LChannel*)SubBands[SubBand_L])->IFwt();
		LChannelC *lchannel = ntchannel->PushFwtStepC(wavelet);
		channel = lchannel->SubBands[SubBand_L];
		lchannel->SubBands[SubBand_L] = NULL;
		delete ntchannel;
		delete lchannel;
	    }
	else {
	    LChannelC *lchannel = ((NTChannel*)SubBands[SubBand_L])->
				    PushFwtStepC(wavelet);
	    channel = lchannel->SubBands[SubBand_L];
	    lchannel->SubBands[SubBand_L] = NULL;
	    delete lchannel;
	}
	Destroy();
	return(channel->DownScale(GetOdd(s), wavelet));
    } else {
	LChannelR *channel = this->Clone();
	Destroy();
	return(channel);
    }
}


