//
//  	LChannel Class
//
//  $Id: LChannel.h,v 4.3.2.2.2.1 1999/07/20 16:15:44 geert Exp $
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

#ifndef WAILI_LCHANNEL_H
#define WAILI_LCHANNEL_H

#include <assert.h>

#include "Blit.h"


    //  Lifted Channel Abstract Base Class


enum SubBand {
    SubBand_LL = 0, SubBand_LH = 1, SubBand_HL = 2, SubBand_HH = 3,
    SubBand_L = SubBand_LL, SubBand_H = SubBand_LH
};


class LChannelCR;
class LChannelC;
class LChannelR;

class LChannel : public Channel {
    friend class Channel;
    friend class NTChannel;
    friend class LChannelCR;
    friend class LChannelC;
    friend class LChannelR;
    public:
	// Construction/Destruction
	LChannel(const LChannel &channel);
	virtual ~LChannel();

	// Properties
	virtual u_int GetDepth(void) const;
	virtual TransformType GetTransformType(void) const = 0;
	TransformDescriptor *GetTransform(void) const;
	virtual u64 *FullHistogram(PixType &min, PixType &max, u64 &numpixels)
	    const;

	int GetShift(SubBand band);

	// Manipulation
	virtual void Clear(void);
	Channel* &operator[](SubBand band);
	const Channel* &operator[](SubBand band) const;
	virtual LChannel *Clone(void) const = 0;

	// Wavelet Transforms
	NTChannel *IFwt(void);
	virtual NTChannel *IFwt(int x1, int y1, int x2, int y2) const = 0;
	virtual LChannel *PushFwtStepCR(const Wavelet &wavelet);
	virtual LChannel *PushFwtStepC(const Wavelet &wavelet);
	virtual LChannel *PushFwtStepR(const Wavelet &wavelet);
	virtual Channel *PopFwtStep(void);

	// Wavelet Based Operations
	virtual LChannel *Crop(int x1, int y1, int x2, int y2) const;
	virtual void Merge(const Channel &channel);
	virtual void Enhance(f32 m);
	virtual void Enhance(int m, u_int shift);

	// Thresholding
	virtual u64 Threshold(double threshold, int soft = 0);

	// This should be removed later!! The user doesn't need to know
	// what's the internal representation of the channel
	virtual int IsLifted(void) const;

    protected:
	LChannel(const Wavelet &filter, u_int numsubbands,
		 u_int cols = 0, u_int rows = 0);

	virtual void Destroy(void);

	// Internal Wavelet Transform Steps
	virtual NTChannel *IFwtStep(void);
	virtual void Lazy(const NTChannel &source) = 0;
	virtual void ILazy(NTChannel &dest) const = 0;
	virtual void CakeWalk(void) = 0;
	virtual void ICakeWalk(void) = 0;

	// Wavelet Based Operations
	virtual LChannel *Crop_rec(int x1, int y1, int x2, int y2,
				   NTChannel *top, NTChannel *bottom,
				   NTChannel *left, NTChannel *right) const = 0;
	virtual void Merge_rec(const Channel *channel,
			       NTChannel *top, NTChannel *bottom,
			       NTChannel *left, NTChannel *right) = 0;
	virtual Channel *DownScale(u_int s, const Wavelet &wavelet) = 0;

    protected:
	u_int NumSubBands;
	Channel **SubBands;
	const Wavelet *Filter;
	int *Shifts;

    private:
	static const char *rcsid;
};


/////////////////////////////////////////////////////////////////////////////
//
//      Inline Member Functions
//
/////////////////////////////////////////////////////////////////////////////


// inline LChannel::LChannel(const Wavelet &filter)
//     : Channel(), Filter(NULL)
// {
//     ::Clear(SubBands, NumSubBands);  // Is niet correct voor C and R !!!!!!!
//     Filter = filter.Clone();
// }

inline u_int LChannel::GetDepth(void) const
{
    return(1+SubBands[0]->GetDepth());
}

inline int LChannel::GetShift(SubBand band)
{
    assert((u_int)band < NumSubBands);
    return(Shifts[band]);
}

inline Channel* &LChannel::operator[](SubBand band)
{
    assert((u_int)band < NumSubBands);
    assert(SubBands[band] != NULL);
    return(SubBands[band]);
}

inline const Channel* &LChannel::operator[](SubBand band) const
{
    assert((u_int)band < NumSubBands);
    assert(SubBands[band] != NULL);
    return(SubBands[band]);
}

inline LChannel *LChannel::Crop(int x1, int y1, int x2, int y2) const
{
    return(Crop_rec(x1, y1, x2, y2, NULL, NULL, NULL, NULL));
}

inline void LChannel::Merge(const Channel &channel)
{
    Merge_rec(&channel, NULL, NULL, NULL, NULL);
}

inline int LChannel::IsLifted(void) const
{
    return(1);
}


// Include derived classes

#include "LChannelCR.h"
#include "LChannelC.h"
#include "LChannelR.h"

#endif // WAILI_LCHANNEL_H
