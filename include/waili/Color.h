//
//	Color Spaces
//
//  $Id: Color.h,v 4.0.2.1.2.1 1999/07/20 16:15:44 geert Exp $
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

#ifndef WAILI_COLOR_H
#define WAILI_COLOR_H

#include "Types.h"


class Color_RGB8;
class Color_XYZ8;
class Color_LAB8;

class Color_RGB {
    public:
	Color_RGB() {}
	Color_RGB(const Color_RGB8 &rgb8);

	f32 r, g, b;
};

class Color_XYZ {
    public:
	Color_XYZ() {}
	Color_XYZ(const Color_XYZ8 &xyz8);

	f32 x, y, z;
};

class Color_LAB {
    public:
	Color_LAB() {}
	Color_LAB(const Color_LAB8 &lab8);

	f32 l, a, b;
};

class Color_RGB8 {
    public:
	Color_RGB8() {}
	Color_RGB8(const Color_RGB &rgb);

	u8 r, g, b;
};

class Color_XYZ8 {
    public:
	Color_XYZ8() {}
	Color_XYZ8(const Color_XYZ &xyz);

	u8 x, y, z;
};

class Color_LAB8 {
    public:
	Color_LAB8() {}
	Color_LAB8(const Color_LAB &lab);

	u8 l;
	s8 a, b;
};

class Color_CIEY {
    public:
	Color_CIEY() {}

	f32 y;
};

class Color_CIEL {
    public:
	Color_CIEL() {}

	f32 l;
};

class Color_RGB16 {
    public:
	Color_RGB16() {}

	s16 r, g, b;
};

class Color_YUVr16 {
    public:
	Color_YUVr16() {}

	s16 yr, ur, vr;
};


class ColorSpace {
    public:
	ColorSpace(void);

	void SetPrimChroma(const f32 chroma[6], int immediate = 1);
	void GetPrimChroma(f32 chroma[6]) const;
	void SetWhitePoint(const f32 wp[2], int immediate = 1);
	void GetWhitePoint(f32 wp[2]) const;

	//  Color Conversions (f32)
	void Convert(const Color_RGB &rgb, Color_XYZ &xyz) const;
	void Convert(const Color_XYZ &xyz, Color_RGB &rgb) const;
	void Convert(const Color_XYZ &xyz, Color_LAB &lab) const;
	void Convert(const Color_LAB &lab, Color_XYZ &xyz) const;
	void Convert(const Color_RGB &rgb, Color_LAB &lab) const;
	void Convert(const Color_LAB &lab, Color_RGB &rgb) const;

	//  Color to Gray Conversions (f32)
	void Convert(const Color_RGB &rgb, Color_CIEY &y) const;
	void Convert(const Color_XYZ &xyz, Color_CIEY &y) const;
	void Convert(const Color_RGB &rgb, Color_CIEL &l) const;
	void Convert(const Color_XYZ &xyz, Color_CIEL &l) const;

	//  Gray Conversions (f32)
	static void Convert(const Color_CIEY &y, Color_CIEL &l);
	static void Convert(const Color_CIEL &l, Color_CIEY &y);

	//  Color Conversions (8 bit)
	void Convert(const Color_RGB8 &rgb, Color_XYZ8 &xyz) const;
	void Convert(const Color_XYZ8 &xyz, Color_RGB8 &rgb) const;
	void Convert(const Color_XYZ8 &xyz, Color_LAB8 &lab) const;
	void Convert(const Color_LAB8 &lab, Color_XYZ8 &xyz) const;
	void Convert(const Color_RGB8 &rgb, Color_LAB8 &lab) const;
	void Convert(const Color_LAB8 &lab, Color_RGB8 &rgb) const;

	// Color Conversions (16 bit)
	static void Convert(const Color_RGB16 &rgb, Color_YUVr16 &yuvr);
	static void Convert(const Color_YUVr16 &yuvr, Color_RGB16 &rgb);

    protected:
	void CalcConvMat(void);

    private:
	Color_XYZ PrimChroma[3];
	Color_XYZ WhitePoint;
	f32 RGB2XYZ[3][3];
	f32 XYZ2RGB[3][3];

    private:
	static const char *rcsid;
};


// ----------------------------------------------------------------------------


    //  Inline Class Member Functions

inline void ColorSpace::Convert(const Color_RGB &rgb, Color_LAB &lab) const
{
    Color_XYZ xyz;

    Convert(rgb, xyz);
    Convert(xyz, lab);
}

inline void ColorSpace::Convert(const Color_LAB &lab, Color_RGB &rgb) const
{
    Color_XYZ xyz;

    Convert(lab, xyz);
    Convert(xyz, rgb);
}

inline void ColorSpace::Convert(const Color_XYZ &xyz, Color_CIEY &y) const
{
    y.y = xyz.y;
}

inline void ColorSpace::Convert(const Color_RGB &rgb, Color_CIEL &l) const
{
    Color_CIEY y;

    Convert(rgb, y);
    Convert(y, l);
}

inline void ColorSpace::Convert(const Color_XYZ &xyz, Color_CIEL &l) const
{
    Color_CIEY y;

    Convert(xyz, y);
    Convert(y, l);
}

#endif // WAILI_COLOR_H
