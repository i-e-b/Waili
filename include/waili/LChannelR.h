//
//  	LChannelR Class
//
//  $Id: LChannelR.h,v 4.3.2.1.2.1 1999/07/20 16:15:44 geert Exp $
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

#ifndef WAILI_LCHANNELR_H
#define WAILI_LCHANNELR_H

#include <assert.h>

#include "Blit.h"


    // 	Lifted Channel Class (lifted Rows)


class LChannelR : public LChannel {
    friend class NTChannel;
    public:
	// Construction/Destruction
	LChannelR(const Wavelet &filter);
	LChannelR(const Wavelet &filter, u_int cols, u_int rows,
		  int offx, int offy);
	LChannelR(const LChannelR &channel);

	// Properties
	virtual void GetMask(u_int &maskx, u_int &masky) const;
	virtual TransformType GetTransformType(void) const;
	u_int GetClow(void) const;
	u_int GetChigh(void) const;

	// Manipulation
	virtual PixType &operator()(u_int c, u_int r);
	virtual PixType operator()(u_int c, u_int r) const;
	virtual void Resize(u_int cols, u_int rows);
	virtual LChannelR *Clone(void) const;
	virtual void SetOffsetX(int offx);
	virtual void SetOffsetY(int offy);

	virtual void Add(const Channel &channel);
	virtual void Subtract(const Channel &channel);
	virtual LChannelR *Diff(const Channel &channel) const;

	// Wavelet Transforms
	virtual NTChannel *IFwt(int x1, int y1, int x2, int y2) const;

    protected:
	// Internal Wavelet Transform Steps
	virtual void Lazy(const NTChannel &source);
	virtual void ILazy(NTChannel &dest) const;
	virtual void CakeWalk(void);
	virtual void ICakeWalk(void);

	void Zip(const LChannelR &channel);

	// Wavelet Based Operations
	virtual LChannelR *Crop_rec(int x1, int y1, int x2, int y2,
				    NTChannel *top, NTChannel *bottom,
				    NTChannel *left, NTChannel *right) const;
	virtual void Merge_rec(const Channel *channel,
			       NTChannel *top, NTChannel *bottom,
			       NTChannel *left, NTChannel *right);
	virtual Channel *DownScale(u_int s, const Wavelet &wavelet);

    private:
	NTChannel *CropMergeH(LChannelR *channel, int x1_l, int x1_h, int y1,
			      int x2_l, int x2_h, int y2);

	static const char *rcsid;
};


/////////////////////////////////////////////////////////////////////////////
//
//      Inline Member Functions
//
/////////////////////////////////////////////////////////////////////////////


inline LChannelR::LChannelR(const Wavelet &filter)
    : LChannel(filter, 2)
{
    Shifts[SubBand_L] = filter.GetShiftL();
    Shifts[SubBand_H] = filter.GetShiftH();
}

inline TransformType LChannelR::GetTransformType(void) const
{
    return(TT_Rows);
}

inline u_int LChannelR::GetClow(void) const
{
    assert(SubBands[SubBand_L] != NULL);
    return(SubBands[SubBand_L]->GetCols());
}

inline u_int LChannelR::GetChigh(void) const
{
    assert(SubBands[SubBand_H] != NULL);
    return(SubBands[SubBand_H]->GetCols());
}

inline PixType &LChannelR::operator()(u_int c, u_int r)
{
    Channel *channel;
#ifdef BOUNDS_CHECK
    assert(c < Cols);
    assert(r < Rows);
#endif // BOUNDS_CHECK
    if (c < GetClow())
	channel = SubBands[0];
    else {
	c -= GetClow();
	channel = SubBands[1];
    }
#ifdef BOUNDS_CHECK
    assert(channel != NULL);
#endif // BOUNDS_CHECK
    return((*channel)(c, r));
}

inline PixType LChannelR::operator()(u_int c, u_int r) const
{
    Channel *channel;
#ifdef BOUNDS_CHECK
    assert(c < Cols);
    assert(r < Rows);
#endif // BOUNDS_CHECK
    if (c < GetClow())
	channel = SubBands[0];
    else {
	c -= GetClow();
	channel = SubBands[1];
    }
#ifdef BOUNDS_CHECK
    assert(channel != NULL);
#endif // BOUNDS_CHECK
    return((*channel)(c, r));
}

inline LChannelR *LChannelR::Clone(void) const
{
    return(new LChannelR(*this));
}


#endif // WAILI_LCHANNELR_H
