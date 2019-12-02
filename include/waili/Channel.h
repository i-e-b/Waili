//
//  	Channel Class
//
//  $Id: Channel.h,v 4.5.2.3.2.1 1999/07/20 16:15:43 geert Exp $
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

#ifndef WAILI_CHANNEL_H
#define WAILI_CHANNEL_H

#include <assert.h>

#include "Wavelet.h"
#include "Blit.h"


enum TransformType {
    TT_ColsRows, TT_Cols, TT_Rows
};

struct TransformDescriptor {
    u8 type;	// TransformType
    u8 filter;	// Wavelet_IDs
};



    //  Channel Abstract Base Class

class LChannel;
// class LChannelCR;

class Channel {
    friend class LChannelCR;
    friend class LChannelC;
    friend class LChannelR;
    public:
	// Construction/Destruction
	Channel();
	Channel(u_int cols, u_int rows, int offx = 0, int offy = 0);
	Channel(const Channel &channel);
	virtual ~Channel();

	static Channel *CreateFromDescriptor(u_int cols, u_int rows,
	    const TransformDescriptor transform[], u_int depth,
	    int offsetx = 0, int offsety = 0);

	// Properties
	u_int GetCols(void) const;
	u_int GetRows(void) const;
	int GetOffsetX(void) const;
	int GetOffsetY(void) const;
	virtual void GetMask(u_int &maskx, u_int &masky) const = 0;
	virtual u_int GetDepth(void) const = 0;
	virtual double Psnr(const Channel &channel, PixType maxval = 255)
	    const;
	virtual u64 *FullHistogram(PixType &min, PixType &max, u64 &numpixels)
	    const = 0;
	double Entropy(void) const;

	// Manipulation
	virtual PixType &operator()(u_int c, u_int r) = 0;
	virtual PixType operator()(u_int c, u_int r) const = 0;
	virtual void Clear(void) = 0;
	virtual void Resize(u_int cols, u_int rows);
	virtual Channel *Clone(void) const = 0;
	virtual void SetOffsetX(int offx) = 0;
	virtual void SetOffsetY(int offy) = 0;

	virtual Channel *Crop(int x1, int y1, int x2, int y2) const = 0;
	virtual void Merge(const Channel &channel) = 0;
	virtual void Add(const Channel &channel) = 0;
	virtual void Subtract(const Channel &channel) = 0;
	virtual Channel *Diff(const Channel &channel) const = 0;
	virtual void Enhance(f32 m) = 0;
	virtual void Enhance(int m, u_int shift) = 0;

	// Wavelet Transforms
	virtual LChannel *PushFwtStepCR(const Wavelet &wavelet) = 0;
	virtual LChannel *PushFwtStepC(const Wavelet &wavelet) = 0;
	virtual LChannel *PushFwtStepR(const Wavelet &wavelet) = 0;
	virtual Channel *PopFwtStep(void) = 0;

	// Thresholding
	virtual u64 Threshold(double threshold, int soft = 0) = 0;

	// This should be removed later!! The user doesn't need to know
	// what's the internal representation of the channel
	virtual int IsLifted(void) const = 0;

	// Wavelet Based Operations
	Channel *Scale(f32 s, const Wavelet &wavelet);

    protected:
	virtual void Destroy(void) = 0;

	// Wavelet Based Operations
	virtual Channel *DownScale(u_int s, const Wavelet &wavelet) = 0;
	Channel *UpScale(u_int s, const Wavelet &wavelet);

	static int GetEven(int len);
	static int GetOdd(int len);

    protected:
	u_int Cols, Rows;
	int OffsetX, OffsetY;

    private:
	static const char *rcsid;
};


/////////////////////////////////////////////////////////////////////////////
//
// 	Inline Member Functions
//
/////////////////////////////////////////////////////////////////////////////


inline Channel::Channel()
    : Cols(0), Rows(0), OffsetX(0), OffsetY(0)
{}

inline Channel::Channel(u_int cols, u_int rows, int offx, int offy)
    : Cols(cols), Rows(rows), OffsetX(offx), OffsetY(offy)
{}

inline Channel::Channel(const Channel &channel)
    : Cols(channel.Cols), Rows(channel.Rows), OffsetX(channel.OffsetX),
      OffsetY(channel.OffsetY)
{}

inline Channel::~Channel()
{}

inline u_int Channel::GetCols(void) const
{
    return(Cols);
}

inline u_int Channel::GetRows(void) const
{
    return(Rows);
}

inline int Channel::GetOffsetX(void) const
{
    return(OffsetX);
}

inline int Channel::GetOffsetY(void) const
{
    return(OffsetY);
}

inline void Channel::Resize(u_int cols, u_int rows)
{
    Cols = cols;
    Rows = rows;
}

inline int Channel::GetEven(int len)
{
    return((len+(len>=0))/2);
}

inline int Channel::GetOdd(int len)
{
    return((len-(len<0))/2);
}


// Include derived classes

#include "LChannel.h"
#include "NTChannel.h"

#endif // WAILI_CHANNEL_H
