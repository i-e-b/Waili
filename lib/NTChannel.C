//
//  	NTChannel Class
//
//  $Id: NTChannel.C,v 4.12.2.3.2.1 1999/07/20 16:15:51 geert Exp $
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


const char *NTChannel::rcsid = "$Id: NTChannel.C,v 4.12.2.3.2.1 1999/07/20 16:15:51 geert Exp $";


// ----------------------------------------------------------------------------
//
//	Non-Transformed Channel Class
//
// ----------------------------------------------------------------------------


NTChannel::NTChannel(u_int cols, u_int rows, int offx, int offy)
    : Channel(cols, rows, offx, offy), Data(NULL)
{
    Data = new PixType[Cols*Rows];
}

NTChannel::NTChannel(const NTChannel &channel)
    : Channel(channel.Cols, channel.Rows, channel.OffsetX, channel.OffsetY),
      Data(NULL)
{
    Data = new PixType[Cols*Rows];
    Copy(channel.Data, Data, Cols*Rows);
}

NTChannel::~NTChannel()
{
    delete[] Data;
}

void NTChannel::Clear(void)
{
    ::Clear(Data, Cols*Rows);
}

void NTChannel::Resize(u_int cols, u_int rows)
{
    PixType *data = new PixType[cols*rows];
    Cols = cols;
    Rows = rows;
    delete[] Data;
    Data = data;
}

NTChannel *NTChannel::Crop(int x1, int y1, int x2, int y2) const
{
    assert(x1 >= OffsetX);
    assert(y1 >= OffsetY);
    assert(x2 < OffsetX+(int)Cols);
    assert(y2 < OffsetY+(int)Rows);
    assert(x2 >= x1);
    assert(y2 >= y1);
    u_int cols = x2-x1+1;
    u_int rows = y2-y1+1;
    NTChannel *ntch = new NTChannel(cols, rows, x1, y1);
    CopyRect(Data, Cols, x1-OffsetX, y1-OffsetY, ntch->Data, cols, 0, 0,
	     cols, rows);
    return(ntch);
}

void NTChannel::Merge(const Channel &channel)
{
    int x = channel.GetOffsetX()-OffsetX;
    int y = channel.GetOffsetY()-OffsetY;
    assert(x+channel.GetCols() <= Cols);
    assert(y+channel.GetRows() <= Rows);
    Channel *ch = channel.Clone();
    NTChannel *ntch;
    if (ch->IsLifted()) {
	ntch = ((LChannel*)ch)->IFwt();
	delete ch;
    }
    else
	ntch = (NTChannel*)ch;
    CopyRect(ntch->Data, ntch->Cols, 0, 0, Data, Cols, x, y,
	     ntch->Cols, ntch->Rows);
    delete ntch;
}

void NTChannel::Add(const Channel &channel)
{
    assert(!channel.IsLifted());
    assert(channel.GetRows() == Rows);
    assert(channel.GetCols() == Cols);
    for (u_int r = 0; r < Rows; r++)
	for (u_int c = 0; c < Cols; c++)
	    (*this)(c, r) += channel(c, r);
}

void NTChannel::Subtract(const Channel &channel)
{
    assert(!channel.IsLifted());
    assert(channel.GetRows() == Rows);
    assert(channel.GetCols() == Cols);
    for (u_int r = 0; r < Rows; r++)
	for (u_int c = 0; c < Cols; c++)
	    (*this)(c, r) -= channel(c, r);
}

NTChannel *NTChannel::Diff(const Channel &channel) const
{
    assert(!channel.IsLifted());
    assert(channel.GetRows() == Rows);
    assert(channel.GetCols() == Cols);
    assert(channel.GetOffsetX() == OffsetX);
    assert(channel.GetOffsetY() == OffsetY);
    NTChannel *diff = new NTChannel(Cols, Rows, OffsetX, OffsetY);
    for (u_int r = 0; r < Rows; r++)
	for (u_int c = 0; c < Cols; c++)
	    (*diff)(c, r) = (*this)(c, r) == channel(c, r)? -128: 127;
    return(diff);
}

s32 *NTChannel::Correlate(const NTChannel &channel, u_int diff) const
{
    s32 *cor = new s32[Cols*Rows];
    NTChannel *scaled = channel.DupliScale((u_int)pow(2.0, (double)diff));

    int dx = scaled->OffsetX-OffsetX;
    int dy = scaled->OffsetY-OffsetY;
    for (u_int j = 0; j < Rows; j++)
	if ((int)j < dy || j > dy+scaled->Rows-1)
	    for (u_int i = 0; i < Cols; i++)
		cor[j*Cols+i] = 0;
	else
	    for (u_int i = 0; i < Cols; i++)
		if ((int)i < dx || i > dx+scaled->Cols-1)
		    cor[j*Cols+i] = 0;
		else
		    cor[j*Cols+i] = (*this)(i,j)*(*scaled)(i-dx,j-dy);
    delete scaled;
    return(cor);
}

void NTChannel::Enhance(f32 m)
{
    for (u_int r = 0; r < Rows; r++)
       for (u_int c = 0; c < Cols; c++)
	   (*this)(c, r) = PixType(m*(*this)(c, r));
}

void NTChannel::Enhance(int m, u_int shift)
{
    for (u_int r = 0; r < Rows; r++)
       for (u_int c = 0; c < Cols; c++)
	   (*this)(c, r) = PixType(int(m*(*this)(c, r))>>shift);
}

LChannelCR *NTChannel::PushFwtStepCR(const Wavelet &wavelet)
{
    if (Cols < 2 || Rows < 2)
	return(NULL);

    LChannelCR *lifted = new LChannelCR(wavelet, Cols, Rows, OffsetX, OffsetY);
    lifted->Lazy(*this);
    Destroy();
    lifted->CakeWalk();
    return(lifted);
}

LChannelC *NTChannel::PushFwtStepC(const Wavelet &wavelet)
{
    if (Rows < 2)
	return(NULL);

    LChannelC *lifted = new LChannelC(wavelet, Cols, Rows, OffsetX, OffsetY);
    lifted->Lazy(*this);
    Destroy();
    lifted->CakeWalk();
    return(lifted);
}

LChannelR *NTChannel::PushFwtStepR(const Wavelet &wavelet)
{
    if (Cols < 2)
	return(NULL);

    LChannelR *lifted = new LChannelR(wavelet, Cols, Rows, OffsetX, OffsetY);
    lifted->Lazy(*this);
    Destroy();
    lifted->CakeWalk();
    return(lifted);
}

LChannel *NTChannel::Fwt(const TransformDescriptor transform[], u_int depth)
{
    LChannel *channel = NULL;
    if (depth) {
	const Wavelet *wavelet = Wavelet::CreateFromID(transform->filter);
	switch (transform->type) {
	    case TT_ColsRows:
		channel = PushFwtStepCR(*wavelet);
		break;
	    case TT_Cols:
		channel = PushFwtStepC(*wavelet);
		break;
	    case TT_Rows:
		channel = PushFwtStepR(*wavelet);
		break;
	}
	delete wavelet;
	if (channel) {
	    Destroy();
	    assert(!channel->SubBands[0]->IsLifted());
	    NTChannel *ch1 = (NTChannel *)channel->SubBands[0];
	    LChannel *ch2 = ch1->Fwt(++transform, --depth);
	    if (ch2) {
		delete channel->SubBands[0];
		channel->SubBands[0] = ch2;
	    }
	}
    }
    return(channel);
}

LChannelCR *NTChannel::RedundantFwtCR(const Wavelet &wavelet)
{
    if (Cols < 2 || Rows < 2)
	return(NULL);


    LChannelCR *lifted = new LChannelCR(wavelet, Cols, Rows, OffsetX, OffsetY);
    lifted->Lazy(*this);
    LChannelCR *red_lh = new LChannelCR(wavelet, Cols, Rows, OffsetX+1,
					OffsetY);
    red_lh->Lazy(*this);
    LChannelCR *red_hl = new LChannelCR(wavelet, Cols, Rows, OffsetX,
					OffsetY+1);
    red_hl->Lazy(*this);
    LChannelCR *red_hh = new LChannelCR(wavelet, Cols, Rows, OffsetX+1,
					OffsetY+1);
    red_hh->Lazy(*this);
    Destroy();
    lifted->CakeWalk();
    red_lh->CakeWalk();
    red_hl->CakeWalk();
    red_hh->CakeWalk();
    lifted->Zip(*red_lh, *red_hl, *red_hh);
    delete lifted->SubBands[SubBand_LL];
    lifted->SubBands[SubBand_LL] = this->Clone();
    return(lifted);
}

LChannelC *NTChannel::RedundantFwtC(const Wavelet &wavelet)
{
    if (Rows < 2)
	return(NULL);

    LChannelC *lifted = new LChannelC(wavelet, Cols, Rows, OffsetX, OffsetY);
    lifted->Lazy(*this);
    LChannelC *redundant = new LChannelC(wavelet, Cols, Rows, OffsetX,
					 OffsetY+1);
    redundant->Lazy(*this);
    Destroy();
    lifted->CakeWalk();
    redundant->CakeWalk();
    lifted->Zip(*redundant);
    return(lifted);
}

LChannelR *NTChannel::RedundantFwtR(const Wavelet &wavelet)
{
    if (Cols < 2)
	return(NULL);

    LChannelR *lifted = new LChannelR(wavelet, Cols, Rows, OffsetX, OffsetY);
    lifted->Lazy(*this);
    LChannelR *redundant = new LChannelR(wavelet, Cols, Rows, OffsetX+1,
					 OffsetY);
    redundant->Lazy(*this);
    Destroy();
    lifted->CakeWalk();
    redundant->CakeWalk();
    lifted->Zip(*redundant);
    return(lifted);
}

NTChannel *NTChannel::DownScale(u_int s, const Wavelet &wavelet)
{
    if (s > 1)
	if (Cols > 1 && Rows > 1) {
	    LChannelCR *channel = PushFwtStepCR(wavelet);
	    ((NTChannel*)channel->SubBands[SubBand_LH])->Destroy();
	    ((NTChannel*)channel->SubBands[SubBand_HL])->Destroy();
	    ((NTChannel*)channel->SubBands[SubBand_HH])->Destroy();
	    return(((NTChannel*)channel->SubBands[SubBand_LL])->
		    DownScale(GetOdd(s), wavelet));
	}
	else
	    return(NULL);
    else {
	NTChannel *channel = this->Clone();
	Destroy();
	return(channel);
    }
}

NTChannel *NTChannel::DupliScale(u_int s) const
{
    u_int cols = Cols*s;
    NTChannel *channel = new NTChannel(cols, Rows*s,
				       OffsetX*(int)s, OffsetY*(int)s);

    PixType *src, *dst;
    src = dst = channel->Data;

    u_int js = 0;
    for (u_int j = 0; j < Rows; j++) {
	u_int is = 0;
	for (u_int i = 0; i < Cols; i++) {
	    for (u_int p = 0; p < s; p++)
		(*channel)(is+p, js) = (*this)(i, j);
	    is += s;
	}
	for (u_int q = 1; q < s; q++) {
	    dst += cols;
	    Copy(src, dst, cols);
	}
	src = dst += cols;
	js += s;
    }
    return(channel);
}

void NTChannel::Interpolate(f32 s)
{
    u_int cols = (int)floor(Cols*s);
    u_int rows = (int)floor(Rows*s);
    f32 step, x = 0;

    NTChannel *channel = new NTChannel(cols, Rows, OffsetX, OffsetY);
    step = (f32)(Cols-1)/(f32)(cols-1);
    for (u_int c = 0; c < cols; c++) {
	u_int x1 = (int)floor(x);
	u_int x2 = Min(x1+1, Cols-1);
	f32 dx = x-(f32)x1;
	for (u_int r = 0; r < Rows; r++)
	    (*channel)(c, r) = Interpolate((*this)(x1, r), (*this)(x2, r), dx);
	x += step;
    }

    step = (f32)(Rows-1)/(f32)(rows-1);
    x = 0;
    u_int oldRows = Rows;
    this->Resize(cols, rows);
    for (u_int r = 0; r < rows; r++) {
	u_int x1 = (int)floor(x);
	u_int x2 = Min(x1+1, oldRows-1);
	f32 dx = x-(f32)x1;
	for (u_int c = 0; c < cols; c++)
	    (*this)(c, r) = Interpolate((*channel)(c, x1), (*channel)(c, x2),
					dx);
	x += step;
    }

    delete channel;
}

void NTChannel::Destroy(void)
{
    delete[] Data;
    Data = 0;
    Cols = Rows = OffsetX = OffsetY = 0;
}

void NTChannel::GetMinMax(PixType &min, PixType &max, u_int smoothing = 0) const
{
    if (smoothing != 0)
	Die("%s: Smoothing isn't implemented yet!\n", __FUNCTION__);
    min = max = (*this)(0, 0);
    for (u_int r = 0; r < Rows; r++)
	for (u_int c = 0; c < Cols; c++)
	    if ((*this)(c, r) < min)
		min = (*this)(c, r);
	    else if ((*this)(c, r) > max)
		max = (*this)(c, r);
}

u64 *NTChannel::Histogram(PixType min, PixType max) const
{
    u64 *histogram = new u64[max-min+1];
    ::Clear(histogram, max-min+1);
    u64 *hz = histogram-min;
    for (u_int r = 0; r < Rows; r++)
       for (u_int c = 0; c < Cols; c++) {
	   PixType val = (*this)(c, r);
	   if (val < min)
	       val = min;
	   else if (val > max)
	       val = max;
	   hz[val]++;
       }
    return(histogram);
}

u64 *NTChannel::FullHistogram(PixType &min, PixType &max, u64 &numpixels) const
{
    GetMinMax(min, max);
    u64 *histogram = Histogram(min, max);
    numpixels = Cols*Rows;
    return(histogram);
}


u64 NTChannel::ThresholdHard(u_int threshold)
{
    u64 cnt = 0;

    for (u_int r = 0; r < Rows; r++)
	for (u_int c = 0; c < Cols; c++)
	    if ((u_int)Abs((*this)(c, r)) < threshold) {
		(*this)(c, r) = 0;
		cnt++;
	    }
    return(cnt);
}

u64 NTChannel::ThresholdSoft(u_int threshold)
{
    u64 cnt = 0;

    for (u_int r = 0; r < Rows; r++)
	for (u_int c = 0; c < Cols; c++)
	    if ((u_int)Abs((*this)(c, r)) < threshold) {
		(*this)(c, r) = 0;
		cnt++;
	    } else if ((*this)(c, r) > 0)
		(*this)(c, r) -= threshold;
	    else
		(*this)(c, r) += threshold;
    return(cnt);
}


    //  Generalized Cross Validation

u_int NTChannel::OptimalGCVThreshold(void) const
{
    PixType min, max;
    u_int threshold, first, last, u, v;
    double minimum, ffirst, flast, fu, fv;

    GetMinMax(min, max);
    last = Max(Abs(min), Abs(max));

    if (last > 20)
	last /= 5;
    if (!last)
	last = 1;
    first = u_int(last/50);
    if (!first)
	first = 1;

    u_int f0 = 1;
    u_int f1 = 1;
    u_int f2 = 2;
    while (last-first > f2) {
	f0 = f1;
	f1 = f2;
	f2 = f0+f1;
    }
    ffirst = GCV(first);
    flast = GCV(last);
    u = first+f0;
    fu = GCV(u);
    v = first+f1;
    fv = GCV(v);

    while (last-first > 3) {
	if (fu > fv) {
	    first = u;
	    ffirst = fu;
	    u = v;
	    fu = fv;
	    v = first+last-u;
	    if (v <= u)
		if (last > u+1)
		    v = (u+last)/2;
		else
		    v = last;
	    fv = GCV(v);
	} else {
	    last = v;
	    flast = fv;
	    v = u;
	    fv = fu;
	    u = first+last-v;
	    if (u >= v)
		if (first < v-1)
		    u = (first+v)/2;
		else
		    u = first;
	    fu = GCV(u);
	}
    }
    threshold = first;
    minimum = ffirst;
    if (fu < minimum) {
	threshold = u;
	minimum = fu;
    }
    if (fv < minimum) {
	threshold = v;
	minimum = fv;
    }
    if (flast < minimum) {
	threshold = last;
	minimum = flast;
    }

    return(threshold);
}

double NTChannel::GCV(u_int threshold) const
{
    u_int cnt = 0;
    u64 quadsum = 0;
    u_int quadthresh = threshold*threshold;
    double gcv;

    for (u_int r = 0; r < Rows; r++)
	for (u_int c = 0; c < Cols; c++)
	    if ((u_int)Abs((*this)(c, r)) < threshold) {
		cnt++;
		quadsum += (*this)(c, r)*(*this)(c, r);
	    } else
		quadsum += quadthresh;
    gcv = (double)Cols*(double)Rows*(double)quadsum/((double)cnt*(double)cnt);
    return(gcv);
}


