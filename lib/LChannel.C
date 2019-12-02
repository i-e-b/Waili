//
//  	LChannel Class
//
//  $Id: LChannel.C,v 4.5.2.2.2.1 1999/07/20 16:15:49 geert Exp $
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
#include <stdio.h>
#include <stream.h>
#include <math.h>

#include <waili/Util.h>


const char *LChannel::rcsid = "$Id: LChannel.C,v 4.5.2.2.2.1 1999/07/20 16:15:49 geert Exp $";


// ----------------------------------------------------------------------------
//
//	Lifted Channel Abstract Base Class
//
// ----------------------------------------------------------------------------


LChannel::LChannel(const Wavelet &filter, u_int numsubbands,
		   u_int cols, u_int rows)
    : Channel(cols, rows), NumSubBands(0), SubBands(NULL), Filter(NULL),
      Shifts(NULL)
{
    if (numsubbands) {
	SubBands = new Channel*[numsubbands];
	::Clear(SubBands, numsubbands);
	Shifts = new int[numsubbands];
	::Clear(Shifts, numsubbands);
    }
    NumSubBands = numsubbands;
    Filter = filter.Clone();
}

LChannel::LChannel(const LChannel &channel)
    : Channel(channel.Cols, channel.Rows, channel.OffsetX, channel.OffsetY),
      NumSubBands(0), SubBands(NULL), Filter(NULL), Shifts(NULL)
{
    u_int numsubbands = channel.NumSubBands;
    if (numsubbands) {
	SubBands = new Channel*[numsubbands];
	Shifts = new int[numsubbands];
	for (u_int i = 0; i < numsubbands; i++) {
	    if (channel.SubBands[i])
		SubBands[i] = channel.SubBands[i]->Clone();
	    else
		SubBands[i] = NULL;
	    Shifts[i] = channel.Shifts[i];
	}
    }
    NumSubBands = numsubbands;
    Filter = channel.Filter->Clone();
}

LChannel::~LChannel()
{
    delete Filter;
    if (SubBands) {
	for (u_int i = 0; i < NumSubBands; i++)
	    delete SubBands[i];
	delete[] SubBands;
    }
}

TransformDescriptor *LChannel::GetTransform(void) const
{
    u_int depth = GetDepth();
    TransformDescriptor *transform = new TransformDescriptor[depth];
    const LChannel *lchannel = this;
    for (u_int level = 0; level < depth; level++) {
	transform[level].type = lchannel->GetTransformType();
	transform[level].filter = lchannel->Filter->GetID();
	if (lchannel->SubBands[0]->IsLifted())
	    lchannel = (LChannel*)lchannel->SubBands[0];
    }
    return(transform);
}

u64 *LChannel::FullHistogram(PixType &min, PixType &max, u64 &numpixels) const
{
    numpixels = 0;
    PixType minima[NumSubBands], maxima[NumSubBands];
    u64 *histograms[NumSubBands];
    for (u_int i = 0; i < NumSubBands; i++) {
	u64 *hist = NULL;
	if (SubBands[i]) {
	    u64 n;
	    if ((hist = SubBands[i]->FullHistogram(minima[i], maxima[i], n))) {
		if (!numpixels) {
		    min = minima[i];
		    max = maxima[i];
		} else {
		    min = Min(min, minima[i]);
		    max = Max(max, maxima[i]);
		}
		numpixels += n;
	    }
	}
	histograms[i] = hist;
    }
    if (!numpixels)
	return(NULL);

    u64 *histogram = new u64[max-min+1];
    ::Clear(histogram, max-min+1);
    u64 *hz = histogram-min;
    for (u_int i = 0; i < NumSubBands; i++)
	if (histograms[i]) {
	    u64 *hz2 = histograms[i]-minima[i];
	    for (PixType j = Max(min, minima[i]); j <= Min(max, maxima[i]);
		 j++)
		hz[j] += hz2[j];
	    delete [] histograms[i];
	}
    return(histogram);
}


void LChannel::Clear(void)
{
    for (u_int i = 0; i < NumSubBands; i++)
	if (SubBands[i])
	    SubBands[i]->Clear();
}

void LChannel::Enhance(f32 m)
{
    for (u_int i = 1; i < NumSubBands; i++)
	if (SubBands[i])
	    SubBands[i]->Enhance(m);
}

void LChannel::Enhance(int m, u_int shift)
{
    for (u_int i = 1; i < NumSubBands; i++)
	if (SubBands[i])
	    SubBands[i]->Enhance(m, shift);
}

u64 LChannel::Threshold(double threshold, int soft)
{
    u64 cnt = 0;

    for (u_int i = 0; i < NumSubBands; i++)
	if (SubBands[i]) {
	    double subthresh = threshold;
	    int shift = Shifts[i];
	    if (shift > 0) {
		while (shift >= 2) {
		    subthresh *= 0.5;
		    shift -= 2;
		}
		if (shift)
		subthresh *= M_SQRT1_2;
	    } else if (shift < 0) {
		while (shift <= -2) {
		    subthresh *= 2;
		    shift += 2;
		}
		if (shift)
		    subthresh *= M_SQRT2;
	    }
	    cnt += SubBands[i]->Threshold(subthresh, soft);
	}
    return(cnt);
}

NTChannel *LChannel::IFwtStep(void)
{
    NTChannel *dest = new NTChannel(Cols, Rows, GetOffsetX(), GetOffsetY());
    ICakeWalk();
    ILazy(*dest);
    Destroy();
    return(dest);
}

NTChannel *LChannel::IFwt(void)
{
    if (SubBands[0]->IsLifted()) {
	NTChannel *dest = ((LChannel *)SubBands[0])->IFwt();
	delete SubBands[0];
	SubBands[0] = dest;
    }
    return(IFwtStep());
}

LChannel *LChannel::PushFwtStepCR(const Wavelet &wavelet)
{
    Channel *channel = NULL;
    if (SubBands[0] && (channel = SubBands[0]->PushFwtStepCR(wavelet))) {
	if (channel != SubBands[0]) {
	    delete SubBands[0];
	    SubBands[0] = channel;
	}
	channel = this;
    }
    return((LChannel *)channel);
}

LChannel *LChannel::PushFwtStepC(const Wavelet &wavelet)
{
    Channel *channel = NULL;
    if (SubBands[0] && (channel = SubBands[0]->PushFwtStepC(wavelet))) {
	if (channel != SubBands[0]) {
	    delete SubBands[0];
	    SubBands[0] = channel;
	}
	channel = this;
    }
    return((LChannel *)channel);
}

LChannel *LChannel::PushFwtStepR(const Wavelet &wavelet)
{
    Channel *channel = NULL;
    if (SubBands[0] && (channel = SubBands[0]->PushFwtStepR(wavelet))) {
	if (channel != SubBands[0]) {
	    delete SubBands[0];
	    SubBands[0] = channel;
	}
	channel = this;
    }
    return((LChannel *)channel);
}

Channel *LChannel::PopFwtStep(void)
{
    Channel *channel = NULL;
    if (SubBands[0])
	if (SubBands[0]->IsLifted()) {
	    channel = SubBands[0]->PopFwtStep();
	    if (channel != SubBands[0]) {
		delete SubBands[0];
		SubBands[0] = channel;
	    }
	    channel = this;
	} else {
	    NTChannel *dest =
		    new NTChannel(Cols, Rows, GetOffsetX(), GetOffsetY());
	    ICakeWalk();
	    ILazy(*dest);
	    Destroy();
	    channel = dest;
	}
    return(channel);
}

void LChannel::Destroy(void)
{
    for (u_int i = 0; i < NumSubBands; i++) {
	delete SubBands[i];
	SubBands[i] = NULL;
    }
    delete Filter;
    Filter = NULL;
    Cols = Rows = OffsetX = OffsetY = 0;
}


