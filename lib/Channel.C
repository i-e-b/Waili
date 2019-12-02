//
//	Channel Class
//
//  $Id: Channel.C,v 4.4.2.2.2.1 1999/07/20 16:15:49 geert Exp $
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
#include <iostream.h>
#include <math.h>


const char *Channel::rcsid = "$Id: Channel.C,v 4.4.2.2.2.1 1999/07/20 16:15:49 geert Exp $";


// ----------------------------------------------------------------------------
//
//	Channel Abstract Base Class
//
// ----------------------------------------------------------------------------

Channel *Channel::CreateFromDescriptor(u_int cols, u_int rows,
				       const TransformDescriptor transform[],
				       u_int depth, int offsetx, int offsety)
{
    if (depth == 0)
	return new NTChannel(cols, rows, offsetx, offsety);
    else {
	const Wavelet *wavelet = Wavelet::CreateFromID(transform->filter);
	LChannel *ch = NULL;
	switch (transform->type) {
	    case TT_ColsRows:
		ch = new LChannelCR(*wavelet, cols, rows, offsetx, offsety);
		break;
	    case TT_Cols:
		ch = new LChannelC(*wavelet, cols, rows, offsetx, offsety);
		break;
	    case TT_Rows:
		ch = new LChannelR(*wavelet, cols, rows, offsetx, offsety);
		break;
	}
	delete wavelet;
	Channel *ch2 = ch->SubBands[0];
	cols = ch2->GetCols();
	rows = ch2->GetRows();
	offsetx = ch2->GetOffsetX();
	offsety = ch2->GetOffsetY();
	delete ch2;
	ch->SubBands[0] = CreateFromDescriptor(cols, rows, transform+1,
					       depth-1, offsetx, offsety);
	return ch;
    }
}

double Channel::Psnr(const Channel &channel, PixType maxval) const
{
    if (maxval < 0)
	maxval = -maxval;
    if (Cols != channel.GetCols() || Rows != channel.GetRows())
	return(-DBL_MAX);

    u64 ss = 0;
    for (u_int r = 0; r < Rows; r++)
	for (u_int c = 0; c < Cols; c++) {
	    int d = (*this)(c, r)-channel(c, r);
	    ss += d*d;
	}
    if (ss == 0)
	return(DBL_MAX);
    double rmse = sqrt((double)ss/(double)(Cols*Rows));
    double psnr = 20.0*log10(maxval/rmse);
    return(psnr);
}

double Channel::Entropy(void) const
{
    PixType min, max;
    u64 n;
    u64 *histogram = FullHistogram(min, max, n);
    double logn = log(double(n));
    double entropy = 0;
    for (int i = 0; i < max-min+1; i++)
	if (histogram[i]) {
	    double x = double(histogram[i]);
	    entropy -= x*(log(x)-logn);
	}
    entropy /= double(n);
    entropy *= M_LOG2E;
    delete [] histogram;
    return(entropy);
}

Channel *Channel::UpScale(u_int s, const Wavelet &wavelet)
{
    if (s > 1) {
	LChannelCR *channel = new LChannelCR(wavelet);
	channel->Cols = Cols<<1;
	channel->Rows = Rows<<1;
	channel->OffsetX = OffsetX<<1;
	channel->OffsetY = OffsetY<<1;
	channel->SubBands[SubBand_LH] =
		    new NTChannel(Cols, Rows, OffsetX, OffsetY);
	channel->SubBands[SubBand_HL] =
		    new NTChannel(Cols, Rows, OffsetX, OffsetY);
	channel->SubBands[SubBand_HH] =
		    new NTChannel(Cols, Rows, OffsetX, OffsetY);
	((NTChannel*)channel->SubBands[SubBand_LH])->Clear();
	((NTChannel*)channel->SubBands[SubBand_HL])->Clear();
	((NTChannel*)channel->SubBands[SubBand_HH])->Clear();
	channel->SubBands[SubBand_LL] = this->Clone();
	Destroy();
	return(channel->UpScale(GetEven(s), wavelet));
    } else {
	Channel *channel = this->Clone();
	Destroy();
	return(channel);
    }
}

Channel *Channel::Scale(f32 s, const Wavelet &wavelet)
{
    u_int cols = (int)floor(Cols*s);
    u_int rows = (int)floor(Rows*s);
    Channel *channel;

    if (s < 1)
	channel = DownScale((u_int)floor(1/s), wavelet);
    else
	channel = UpScale((u_int)ceil(s), wavelet);

    if (channel->GetCols() != cols || channel->GetRows() != rows) {
	s = (f32)cols / (f32)channel->GetCols();
	if (channel->IsLifted()) {
	    NTChannel *ch = ((LChannel*)channel)->IFwt();
	    delete channel;
	    ch->Interpolate(s);
	    channel = ch;
	    ch = NULL;
	} else
	    ((NTChannel*)channel)->Interpolate(s);
    }

    return(channel);
}


