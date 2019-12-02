//
//  	NTChannel Class
//
//  $Id: NTChannel.h,v 4.8.2.1.2.1 1999/07/20 16:15:45 geert Exp $
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

#ifndef WAILI_NTCHANNEL_H
#define WAILI_NTCHANNEL_H

#include <assert.h>

#include "Blit.h"


    //  Non-Transformed Channel Class


class NTChannel : public Channel {
    friend class Channel;
    friend class LChannel;
    friend class LChannelCR;
    friend class LChannelC;
    friend class LChannelR;
    friend class LiftChannelR;
    friend class LiftChannelC;
    public:
	// Construction/Destruction
	NTChannel();
	NTChannel(u_int cols, u_int rows, int offx = 0, int offy = 0);
	NTChannel(const NTChannel &channel);
	virtual ~NTChannel();

	// Properties
	virtual void GetMask(u_int &maskx, u_int &masky) const;
	virtual u_int GetDepth(void) const;
	void GetMinMax(PixType &min, PixType &max, u_int smoothing = 0) const;
	u64 *Histogram(PixType min, PixType max) const;
	virtual u64 *FullHistogram(PixType &min, PixType &max, u64 &numpixels)
	    const;

	// Manipulation
	virtual PixType &operator()(u_int c, u_int r);
	virtual PixType operator()(u_int c, u_int r) const;
	virtual void Clear(void);
	virtual void Resize(u_int cols, u_int rows);
	virtual NTChannel *Clone(void) const;
	virtual void SetOffsetX(int offx);
	virtual void SetOffsetY(int offy);

	virtual NTChannel *Crop(int x1, int y1, int x2, int y2) const;
	virtual void Merge(const Channel &channel);
	virtual void Add(const Channel &channel);
	virtual void Subtract(const Channel &channel);
	virtual NTChannel *Diff(const Channel &channel) const;
	s32 *Correlate(const NTChannel &channel, u_int diff) const;

	virtual void Enhance(f32 m);
	virtual void Enhance(int m, u_int shift);

	// Wavelet Transforms
	virtual LChannel *Fwt(const TransformDescriptor transform[],
			      u_int depth);
	virtual LChannelCR *PushFwtStepCR(const Wavelet &wavelet);
	virtual LChannelC *PushFwtStepC(const Wavelet &wavelet);
	virtual LChannelR *PushFwtStepR(const Wavelet &wavelet);
	virtual Channel *PopFwtStep(void);

	virtual LChannelCR *RedundantFwtCR(const Wavelet &wavelet);
	virtual LChannelC *RedundantFwtC(const Wavelet &wavelet);
	virtual LChannelR *RedundantFwtR(const Wavelet &wavelet);

	// Thresholding
	virtual u64 Threshold(double threshold, int soft = 0);
	u64 ThresholdHard(u_int threshold);
	u64 ThresholdSoft(u_int threshold);
	u_int OptimalGCVThreshold(void) const;

	// This should be removed later!! The user doesn't need to know
	// what's the internal representation of the channel
	virtual int IsLifted(void) const;

	// Scaling using a duplication scheme
	NTChannel *DupliScale(u_int s) const;

    protected:
	virtual void Destroy(void);

	// Wavelet Based Operations
	virtual NTChannel *DownScale(u_int s, const Wavelet &wavelet);

	// Scaling using a linear interpolation scheme
	void Interpolate(f32 s);

	// Generalized Cross Validation
	double GCV(u_int threshold) const;

	static PixType Interpolate(PixType y1, PixType y2, f32 x);

    protected:
	PixType *Data;

    private:
	static const char *rcsid;
};


/////////////////////////////////////////////////////////////////////////////
//
//      Inline Member Functions
//
/////////////////////////////////////////////////////////////////////////////


inline NTChannel::NTChannel()
    : Channel(), Data(NULL)
{}

inline void NTChannel::GetMask(u_int &maskx, u_int &masky) const
{
    maskx = masky = 0;
}

inline u_int NTChannel::GetDepth(void) const
{
    return(0);
}

inline PixType &NTChannel::operator()(u_int c, u_int r)
{
#ifdef BOUNDS_CHECK
    assert(Data != NULL);
    assert(c < Cols);
    assert(r < Rows);
#endif // BOUNDS_CHECK
    return(Data[r*Cols+c]);
}

inline PixType NTChannel::operator()(u_int c, u_int r) const
{
#ifdef BOUNDS_CHECK
    assert(Data != NULL);
    assert(c < Cols);
    assert(r < Rows);
#endif // BOUNDS_CHECK
    return(Data[r*Cols+c]);
}

inline NTChannel *NTChannel::Clone(void) const
{
    return(new NTChannel(*this));
}

inline void NTChannel::SetOffsetX(int offx)
{
    OffsetX = offx;
}

inline void NTChannel::SetOffsetY(int offy)
{
    OffsetY = offy;
}

inline Channel *NTChannel::PopFwtStep(void)
{
    return(NULL);
}

inline u64 NTChannel::Threshold(double threshold, int soft)
{
    u_int ithresh = u_int(fabs(threshold)+0.5);
    return(soft ? ThresholdSoft(ithresh) : ThresholdHard(ithresh));
}

inline int NTChannel::IsLifted(void) const
{
    return(0);
}

inline PixType NTChannel::Interpolate(PixType y1, PixType y2, f32 x)
{
    return((PixType)rint((y2-y1)*x)+y1);
}


/////////////////////////////////////////////////////////////////////////////
//
//      Inline Member Functions for Class Lifting !!!!!!
//
/////////////////////////////////////////////////////////////////////////////

#include "Lifting.inline.h"

#endif // WAILI_NTCHANNEL_H
