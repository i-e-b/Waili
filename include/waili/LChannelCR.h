//
//  	LChannelCR Class
//
//  $Id: LChannelCR.h,v 4.3.2.1.2.1 1999/07/20 16:15:44 geert Exp $
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

#ifndef WAILI_LCHANNELCR_H
#define WAILI_LCHANNELCR_H

#include <assert.h>

#include "Blit.h"


    // 	Lifted Channel Class (lifted Cols and Rows)


class LChannelCR : public LChannel {
    friend class NTChannel;
    friend class LChannelC;
    friend class LChannelR;
    public:
	// Construction/Destruction
	LChannelCR(const Wavelet &filter);
	LChannelCR(const Wavelet &filter, u_int cols, u_int rows,
		   int offx, int offy);
	LChannelCR(const LChannelCR &channel);

	// Properties
	virtual void GetMask(u_int &maskx, u_int &masky) const;
	virtual TransformType GetTransformType(void) const;
	u_int GetClow(void) const;
	u_int GetChigh(void) const;
	u_int GetRlow(void) const;
	u_int GetRhigh(void) const;

	// Manipulation
	virtual PixType &operator()(u_int c, u_int r);
	virtual PixType operator()(u_int c, u_int r) const;
	virtual void Resize(u_int cols, u_int rows);
	virtual LChannelCR *Clone(void) const;
	virtual void SetOffsetX(int offx);
	virtual void SetOffsetY(int offy);

	virtual void Add(const Channel &channel);
	virtual void Subtract(const Channel &channel);
	virtual LChannelCR *Diff(const Channel &channel) const;

	// Wavelet Transforms
	virtual NTChannel *IFwt(int x1, int y1, int x2, int y2) const;

    protected:
	// Internal Wavelet Transform Steps
	virtual void Lazy(const NTChannel &source);
	virtual void ILazy(NTChannel &dest) const;
	virtual void CakeWalk(void);
	virtual void ICakeWalk(void);

	void Zip(const LChannelCR &channel_lh, const LChannelCR &channel_hl,
		 const LChannelCR &channel_hh);

	// Wavelet Based Operations
	virtual LChannelCR *Crop_rec(int x1, int y1, int x2, int y2,
				     NTChannel *top, NTChannel *bottom,
				     NTChannel *left, NTChannel *right) const;
	virtual void Merge_rec(const Channel *channel,
			       NTChannel *top, NTChannel *bottom,
			       NTChannel *left, NTChannel *right);
	virtual Channel *DownScale(u_int s, const Wavelet &wavelet);

    private:
	NTChannel *CropMergeH(LChannelCR *channel, int x1_l, int x1_h, int y1_l,
			      int y1_h, int x2_l, int x2_h, int y2_l, int y2_h);

	static const char *rcsid;
};


/////////////////////////////////////////////////////////////////////////////
//
//      Inline Member Functions
//
/////////////////////////////////////////////////////////////////////////////


inline LChannelCR::LChannelCR(const Wavelet &filter)
    : LChannel(filter, 4)
{
    int lshift = filter.GetShiftL();
    int hshift = filter.GetShiftH();
    Shifts[SubBand_LL] = lshift+lshift;
    Shifts[SubBand_LH] = Shifts[SubBand_HL] = hshift+lshift;
    Shifts[SubBand_HH] = hshift+hshift;
}

inline TransformType LChannelCR::GetTransformType(void) const
{
    return(TT_ColsRows);
}

inline u_int LChannelCR::GetClow(void) const
{
    assert(SubBands[SubBand_LL] != NULL);
    return(SubBands[SubBand_LL]->GetCols());
}

inline u_int LChannelCR::GetChigh(void) const
{
    assert(SubBands[SubBand_HH] != NULL);
    return(SubBands[SubBand_HH]->GetCols());
}

inline u_int LChannelCR::GetRlow(void) const
{
    assert(SubBands[SubBand_LL] != NULL);
    return(SubBands[SubBand_LL]->GetRows());
}

inline u_int LChannelCR::GetRhigh(void) const
{
    assert(SubBands[SubBand_HH] != NULL);
    return(SubBands[SubBand_HH]->GetRows());
}

inline PixType &LChannelCR::operator()(u_int c, u_int r)
{
    u_int band = SubBand_LL;
    Channel *channel;
#ifdef BOUNDS_CHECK
    assert(c < Cols);
    assert(r < Rows);
#endif // BOUNDS_CHECK
    if (r >= GetRlow()) {
	r -= GetRlow();
	band += SubBand_HL;
    }
    if (c >= GetClow()) {
	c -= GetClow();
	band += SubBand_LH;
    }
    channel = SubBands[band];
#ifdef BOUNDS_CHECK
    assert(channel != NULL);
#endif // BOUNDS_CHECK
    return((*channel)(c, r));
}

inline PixType LChannelCR::operator()(u_int c, u_int r) const
{
    u_int band = SubBand_LL;
    Channel *channel;
#ifdef BOUNDS_CHECK
    assert(c < Cols);
    assert(r < Rows);
#endif // BOUNDS_CHECK
    if (r >= GetRlow()) {
	r -= GetRlow();
	band += SubBand_HL;
    }
    if (c >= GetClow()) {
	c -= GetClow();
	band += SubBand_LH;
    }
    channel = SubBands[band];
#ifdef BOUNDS_CHECK
    assert(channel != NULL);
#endif // BOUNDS_CHECK
    return((*channel)(c, r));
}

inline LChannelCR *LChannelCR::Clone(void) const
{
    return(new LChannelCR(*this));
}


#endif // WAILI_LCHANNELCR_H
