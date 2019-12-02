//
//	Integer Wavelet Classes
//
//  $Id: Wavelet.h,v 4.1.2.4.2.1 1999/07/20 16:15:45 geert Exp $
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

#ifndef WAILI_WAVELET_H
#define WAILI_WAVELET_H

#include <math.h>
#include <limits.h>

#include "Lifting.h"


    //  Optimization

#define LIFTING_OPTIMIZE_STEPS	1	// Use optimized lifting steps



// ----------------------------------------------------------------------------
//
//	Integer Wavelet Abstract Base Class
//
// ----------------------------------------------------------------------------


class Wavelet {
    public:
	virtual Wavelet *Clone(void) const = 0;
	// Biorthogonal Cohen-Daubechies-Feauveau
	static Wavelet *CreateCDF(u_int np, u_int d);

	virtual void CakeWalk(Lifting &lifting) const = 0;
	virtual void ICakeWalk(Lifting &lifting) const = 0;

	// Filter Length
	int GetGStart() const;
	int GetGEnd() const;
	int GetHStart() const;
	int GetHEnd() const;

	// Normalization
	int GetShiftL(void) const;
	int GetShiftH(void) const;

	// Unique private IDs for different Wavelets
	u8 GetID(void) const;
	static Wavelet *CreateFromID(u8 id);

    protected:
	int GStart, GEnd, HStart, HEnd;
	// Normalization
	int ShiftL, ShiftH;

	// Unique private IDs
	enum Wavelet_IDs {
	    ID_Lazy = 0,
	    ID_CDF_1_1 = 1, ID_CDF_1_3 = 2, ID_CDF_1_5 = 3,
	    ID_CDF_2_2 = 4, ID_CDF_2_4 = 5, ID_CDF_2_6 = 6,
	    ID_CDF_4_2 = 7, ID_CDF_4_4 = 8, ID_CDF_4_6 = 9,
	    ID_CRF_13_7 = 100, ID_SWE_13_7 = 101
	};
	u8 ID;

    private:
	static const char *rcsid;
};


// ----------------------------------------------------------------------------


    //  Lazy Wavelet
    //  Equivalent to Biorthogonal Cohen-Daubechies-Feauveau (0, 0)

class Wavelet_Lazy : public Wavelet {
    public:
	Wavelet_Lazy();
	Wavelet_Lazy *Clone(void) const;

	// Lifting steps on Channels
	void CakeWalk(Lifting &) const {};
	void ICakeWalk(Lifting &) const {};

    private:
	static const char *rcsid;
};


// ----------------------------------------------------------------------------


    //  Biorthogonal Cohen-Daubechies-Feauveau (1, x)

class LiftCoef_CDF_1_x {
    protected:
	static const s16 D1b[2];
	static const u16 D1a;
};

class LiftCoef_CDF_1_1 : public LiftCoef_CDF_1_x {
    protected:
	static const s16 P2b[2];
	static const u16 P2a;
};

class LiftCoef_CDF_1_3 : public LiftCoef_CDF_1_x {
    protected:
	static const s16 P2b[4];
	static const u16 P2a;
};

class LiftCoef_CDF_1_5 : public LiftCoef_CDF_1_x {
    protected:
	static const s16 P2b[6];
	static const u16 P2a;
};


class Wavelet_CDF_1_x : public Wavelet {
    protected:
	Wavelet_CDF_1_x();

    private:
	static const char *rcsid;
};

class Wavelet_CDF_1_1 : public Wavelet_CDF_1_x,
			public LiftCoef_CDF_1_1 {
    public:
	Wavelet_CDF_1_1();
	virtual Wavelet_CDF_1_1 *Clone(void) const;

	// Lifting steps on Channels
	virtual void CakeWalk(Lifting &lifting) const;
	virtual void ICakeWalk(Lifting &lifting) const;
};

class Wavelet_CDF_1_3 : public Wavelet_CDF_1_x,
			public LiftCoef_CDF_1_3 {
    public:
	Wavelet_CDF_1_3();
	virtual Wavelet_CDF_1_3 *Clone(void) const;

	// Lifting steps on Channels
	virtual void CakeWalk(Lifting &lifting) const;
	virtual void ICakeWalk(Lifting &lifting) const;
};

class Wavelet_CDF_1_5 : public Wavelet_CDF_1_x,
			public LiftCoef_CDF_1_5 {
    public:
	Wavelet_CDF_1_5();
	virtual Wavelet_CDF_1_5 *Clone(void) const;

	// Lifting steps on Channels
	virtual void CakeWalk(Lifting &lifting) const;
	virtual void ICakeWalk(Lifting &lifting) const;
};


// ----------------------------------------------------------------------------


    //  Biorthogonal Cohen-Daubechies-Feauveau (2, x)

class LiftCoef_CDF_2_x {
    protected:
	static const s16 D1b[2];
	static const u16 D1a;
};

class LiftCoef_CDF_2_2 : public LiftCoef_CDF_2_x {
    protected:
	static const s16 P2b[2];
	static const u16 P2a;
};

class LiftCoef_CDF_2_4 : public LiftCoef_CDF_2_x {
    protected:
	static const s16 P2b[4];
	static const u16 P2a;
};

class LiftCoef_CDF_2_6 : public LiftCoef_CDF_2_x {
    protected:
	static const s16 P2b[6];
	static const u16 P2a;
};


class Wavelet_CDF_2_x : public Wavelet {
    protected:
	Wavelet_CDF_2_x();

    private:
	static const char *rcsid;
};

class Wavelet_CDF_2_2 : public Wavelet_CDF_2_x,
			public LiftCoef_CDF_2_2 {
    public:
	Wavelet_CDF_2_2();
	virtual Wavelet_CDF_2_2 *Clone(void) const;

	// Lifting steps on Channels
	virtual void CakeWalk(Lifting &lifting) const;
	virtual void ICakeWalk(Lifting &lifting) const;
};

class Wavelet_CDF_2_4 : public Wavelet_CDF_2_x,
			public LiftCoef_CDF_2_4 {
    public:
	Wavelet_CDF_2_4();
	virtual Wavelet_CDF_2_4 *Clone(void) const;

	// Lifting steps on Channels
	virtual void CakeWalk(Lifting &lifting) const;
	virtual void ICakeWalk(Lifting &lifting) const;
};

class Wavelet_CDF_2_6 : public Wavelet_CDF_2_x,
			public LiftCoef_CDF_2_6 {
    public:
	Wavelet_CDF_2_6();
	virtual Wavelet_CDF_2_6 *Clone(void) const;

	// Lifting steps on Channels
	virtual void CakeWalk(Lifting &lifting) const;
	virtual void ICakeWalk(Lifting &lifting) const;
};


// ----------------------------------------------------------------------------


    //  Biorthogonal Cohen-Daubechies-Feauveau (4, x)

class LiftCoef_CDF_4_x {
    protected:
	static const s16 P1b[2];
	static const u16 P1a1;
	static const u16 P1a2;
	static const s16 D2b[2];
	static const u16 D2a;
};

class LiftCoef_CDF_4_2 : public LiftCoef_CDF_4_x {
    protected:
	static const s16 P3b[2];
	static const u16 P3a;
};

class LiftCoef_CDF_4_4 : public LiftCoef_CDF_4_x {
    protected:
	static const s16 P3b[4];
	static const u16 P3a;
};

class LiftCoef_CDF_4_6 : public LiftCoef_CDF_4_x {
    protected:
	static const s16 P3b[6];
	static const u16 P3a;
};


class Wavelet_CDF_4_x : public Wavelet {
    protected:
	Wavelet_CDF_4_x();

    private:
	static const char *rcsid;
};

class Wavelet_CDF_4_2 : public Wavelet_CDF_4_x,
			public LiftCoef_CDF_4_2 {
    public:
	Wavelet_CDF_4_2();
	virtual Wavelet_CDF_4_2 *Clone(void) const;

	// Lifting steps on Channels
	virtual void CakeWalk(Lifting &lifting) const;
	virtual void ICakeWalk(Lifting &lifting) const;
};

class Wavelet_CDF_4_4 : public Wavelet_CDF_4_x,
			public LiftCoef_CDF_4_4 {
    public:
	Wavelet_CDF_4_4();
	virtual Wavelet_CDF_4_4 *Clone(void) const;

	// Lifting steps on Channels
	virtual void CakeWalk(Lifting &lifting) const;
	virtual void ICakeWalk(Lifting &lifting) const;
};

class Wavelet_CDF_4_6 : public Wavelet_CDF_4_x,
			public LiftCoef_CDF_4_6 {
    public:
	Wavelet_CDF_4_6();
	virtual Wavelet_CDF_4_6 *Clone(void) const;

	// Lifting steps on Channels
	virtual void CakeWalk(Lifting &lifting) const;
	virtual void ICakeWalk(Lifting &lifting) const;
};


// ----------------------------------------------------------------------------


    //  JPEG 2000

class Wavelet_CRF_13_7 : public Wavelet {
    public:
	Wavelet_CRF_13_7();
	virtual Wavelet_CRF_13_7 *Clone(void) const;

	// Lifting steps on Channels
	virtual void CakeWalk(Lifting &lifting) const;
	virtual void ICakeWalk(Lifting &lifting) const;

    protected:
	static const s16 D1b[4];
	static const u16 D1a;
	static const s16 P2b[4];
	static const u16 P2a;

    private:
	static const char *rcsid;
};

class Wavelet_SWE_13_7 : public Wavelet {
    public:
	Wavelet_SWE_13_7();
	virtual Wavelet_SWE_13_7 *Clone(void) const;

	// Lifting steps on Channels
	virtual void CakeWalk(Lifting &lifting) const;
	virtual void ICakeWalk(Lifting &lifting) const;

    protected:
	static const s16 D1b[4];
	static const u16 D1a;
	static const s16 P2b[4];
	static const u16 P2a;

    private:
	static const char *rcsid;
};


/////////////////////////////////////////////////////////////////////////////
//
//      Inline Member Functions
//
/////////////////////////////////////////////////////////////////////////////


inline int Wavelet::GetGStart(void) const
{
    return(GStart);
}

inline int Wavelet::GetGEnd(void) const
{
    return(GEnd);
}

inline int Wavelet::GetHStart(void) const
{
    return(HStart);
}

inline int Wavelet::GetHEnd(void) const
{
    return(HEnd);
}

inline int Wavelet::GetShiftL(void) const
{
    return(ShiftL);
}

inline int Wavelet::GetShiftH(void) const
{
    return(ShiftH);
}

inline u8 Wavelet::GetID(void) const
{
    return ID;
}

inline Wavelet_Lazy::Wavelet_Lazy()
{
    HStart = 0;
    HEnd = 0;
    GStart = 1;
    GEnd = 1;
    ShiftH = 0;
    ShiftL = 0;
}

inline Wavelet_Lazy *Wavelet_Lazy::Clone(void) const
{
    return(new Wavelet_Lazy(*this));
}

inline Wavelet_CDF_1_x::Wavelet_CDF_1_x()
{
    GStart = 0;
    GEnd = 1;
    ShiftL = 1;
    ShiftH = -1;
}

inline Wavelet_CDF_2_x::Wavelet_CDF_2_x()
{
    GStart = 0;
    GEnd = 2;
    ShiftL = 1;
    ShiftH = -1;
}

inline Wavelet_CDF_4_x::Wavelet_CDF_4_x()
{
    GStart = -1;
    GEnd = 3;
    ShiftL = 1;
    ShiftH = -1;
}

inline Wavelet_CDF_1_1::Wavelet_CDF_1_1()
{
    HStart = 0;
    HEnd = 1;
    ID = ID_CDF_1_1;
}

inline Wavelet_CDF_1_1 *Wavelet_CDF_1_1::Clone(void) const
{
    return(new Wavelet_CDF_1_1(*this));
}

inline Wavelet_CDF_1_3::Wavelet_CDF_1_3()
{
    HStart = -2;
    HEnd = 3;
    ID = ID_CDF_1_3;
}

inline Wavelet_CDF_1_3 *Wavelet_CDF_1_3::Clone(void) const
{
    return(new Wavelet_CDF_1_3(*this));
}

inline Wavelet_CDF_1_5::Wavelet_CDF_1_5()
{
    HStart = -4;
    HEnd = 5;
    ID = ID_CDF_1_5;
}

inline Wavelet_CDF_1_5 *Wavelet_CDF_1_5::Clone(void) const
{
    return(new Wavelet_CDF_1_5(*this));
}

inline Wavelet_CDF_2_2::Wavelet_CDF_2_2()
{
    HStart = -2;
    HEnd = 2;
    ID = ID_CDF_2_2;
}

inline Wavelet_CDF_2_2 *Wavelet_CDF_2_2::Clone(void) const
{
    return(new Wavelet_CDF_2_2(*this));
}

inline Wavelet_CDF_2_4::Wavelet_CDF_2_4()
{
    HStart = -4;
    HEnd = 4;
    ID = ID_CDF_2_4;
}

inline Wavelet_CDF_2_4 *Wavelet_CDF_2_4::Clone(void) const
{
    return(new Wavelet_CDF_2_4(*this));
}

inline Wavelet_CDF_2_6::Wavelet_CDF_2_6()
{
    HStart = -6;
    HEnd = 6;
    ID = ID_CDF_2_6;
}

inline Wavelet_CDF_2_6 *Wavelet_CDF_2_6::Clone(void) const
{
    return(new Wavelet_CDF_2_6(*this));
}

inline Wavelet_CDF_4_2::Wavelet_CDF_4_2()
{
    HStart = -3;
    HEnd = 3;
    ID = ID_CDF_4_2;
}

inline Wavelet_CDF_4_2 *Wavelet_CDF_4_2::Clone(void) const
{
    return(new Wavelet_CDF_4_2(*this));
}

inline Wavelet_CDF_4_4::Wavelet_CDF_4_4()
{
    HStart = -5;
    HEnd = 5;
    ID = ID_CDF_4_4;
}

inline Wavelet_CDF_4_4 *Wavelet_CDF_4_4::Clone(void) const
{
    return(new Wavelet_CDF_4_4(*this));
}

inline Wavelet_CDF_4_6::Wavelet_CDF_4_6()
{
    HStart = -7;
    HEnd = 7;
    ID = ID_CDF_4_6;
}

inline Wavelet_CDF_4_6 *Wavelet_CDF_4_6::Clone(void) const
{
    return(new Wavelet_CDF_4_6(*this));
}


inline Wavelet_CRF_13_7::Wavelet_CRF_13_7()
{
    GStart = -4;
    GEnd = 2;
    HStart = -6;
    HEnd = 6;
    ID = ID_CRF_13_7;
    ShiftL = 1;
    ShiftH = 1;
}

inline Wavelet_CRF_13_7 *Wavelet_CRF_13_7::Clone(void) const
{
    return new Wavelet_CRF_13_7(*this);
}

inline Wavelet_SWE_13_7::Wavelet_SWE_13_7()
{
    GStart = -4;
    GEnd = 2;
    HStart = -6;
    HEnd = 6;
    ID = ID_SWE_13_7;
    ShiftL = 1;
    ShiftH = 1;
}

inline Wavelet_SWE_13_7 *Wavelet_SWE_13_7::Clone(void) const
{
    return new Wavelet_SWE_13_7(*this);
}


#endif // WAILI_WAVELET_H
