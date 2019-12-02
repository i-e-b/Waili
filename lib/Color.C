//
//	Color Spaces
//
//  $Id: Color.C,v 4.0.2.1.2.1 1999/07/20 16:15:49 geert Exp $
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

#include <math.h>

#include <waili/Color.h>
#include <waili/Util.h>


const char *ColorSpace::rcsid = "$Id: Color.C,v 4.0.2.1.2.1 1999/07/20 16:15:49 geert Exp $";


static f32 DefaultPrimChroma[6] = {
    0.6400, 0.3300,
    0.3000, 0.6000,
    0.1500, 0.0600
};

static f32 DefaultWhitePoint[2] = {
    0.3127, 0.3290
};


ColorSpace::ColorSpace(void)
{
    SetPrimChroma(DefaultPrimChroma, 0);
    SetWhitePoint(DefaultWhitePoint, 1);
}


void ColorSpace::SetPrimChroma(const f32 chroma[6], int immediate)
{
    PrimChroma[0].x = chroma[0];
    PrimChroma[0].y = chroma[1];
    PrimChroma[0].z = 1.0-PrimChroma[0].x-PrimChroma[0].y;
    PrimChroma[1].x = chroma[2];
    PrimChroma[1].y = chroma[3];
    PrimChroma[1].z = 1.0-PrimChroma[1].x-PrimChroma[1].y;
    PrimChroma[2].x = chroma[4];
    PrimChroma[2].y = chroma[5];
    PrimChroma[2].z = 1.0-PrimChroma[2].x-PrimChroma[2].y;

    if (immediate)
	CalcConvMat();
}


void ColorSpace::GetPrimChroma(f32 chroma[6]) const
{
    chroma[0] = PrimChroma[0].x;
    chroma[1] = PrimChroma[0].y;
    chroma[2] = PrimChroma[1].x;
    chroma[3] = PrimChroma[1].y;
    chroma[4] = PrimChroma[2].x;
    chroma[5] = PrimChroma[2].y;
}


void ColorSpace::SetWhitePoint(const f32 wp[2], int immediate)
{
    WhitePoint.x = wp[0]/wp[1];
    WhitePoint.y = 1.0;
    WhitePoint.z = (1.0-wp[0]-wp[1])/wp[1];

    if (immediate)
	CalcConvMat();
}


void ColorSpace::GetWhitePoint(f32 wp[2]) const
{
    wp[1] = 1.0/(1.0+WhitePoint.x+WhitePoint.z);
    wp[0] = WhitePoint.x*wp[1];
}


void ColorSpace::CalcConvMat(void)
{
    f32 mat[3][3], white[3], scale[3], c;
    int ind[3], t;
    u_int i, j, k;

    // Normalized white point
    white[0] = WhitePoint.x;
    white[1] = WhitePoint.y;
    white[2] = WhitePoint.z;

    for (i = 0; i < 3; i++) {
	mat[0][i] = PrimChroma[i].x;
	mat[1][i] = PrimChroma[i].y;
	mat[2][i] = PrimChroma[i].z;
	ind[i] = i;
    }

    // Gaussian elimination with partial pivoting
    for (i = 0; i < 2; i++) {
	for (j = i+1; j < 3; j++)
	    if (fabs(mat[ind[j]][i]) > fabs(mat[ind[i]][i])) {
		t = ind[j];
		ind[j] = ind[i];
		ind[i] = t;
	    }
	if (fabs(mat[ind[i]][i]) < EPS)
	    Die("%s: Matrix is singular\n", __FUNCTION__);

	for (j = i+1; j < 3; j++) {
	    c = mat[ind[j]][i]/mat[ind[i]][i];
	    for (k = i+1; k < 3; k++)
		mat[ind[j]][k] -= c*mat[ind[i]][k];
	    white[ind[j]] -= c*white[ind[i]];
	}
    }
    if (fabs(mat[ind[2]][2]) < EPS)
	Die("%s: Matrix is singular\n", __FUNCTION__);

    // Back substitution to solve for scale
    scale[ind[2]] = white[ind[2]]/mat[ind[2]][2];
    scale[ind[1]] = (white[ind[1]]-(mat[ind[1]][2]*scale[ind[2]]))/
		    mat[ind[1]][1];
    scale[ind[0]] = (white[ind[0]]-(mat[ind[0]][1]*scale[ind[1]])-
		     (mat[ind[0]][2]*scale[ind[2]]))/mat[ind[0]][0];

    // Build matrix RGB->XYZ
    for (i = 0; i < 3; i++) {
	RGB2XYZ[0][i] = scale[i]*PrimChroma[i].x;
	RGB2XYZ[1][i] = scale[i]*PrimChroma[i].y;
	RGB2XYZ[2][i] = scale[i]*PrimChroma[i].z;
    }

    for (i = 0; i < 3; i++)
	for (j = 0; j < 3; j++) {
	    mat[i][j] = RGB2XYZ[i][j];
	    XYZ2RGB[i][j] = i == j ? 1.0 : 0.0;
	}

    for (i = 0; i < 3; i++) {
	for (j = i+1, k = i; j < 3; j++)
	    if (fabs(mat[j][i]) > fabs(mat[k][i]))
		k = j;

	// Check for singularity
	if (fabs(mat[k][i]) < EPS)
	    Die("%s: Matrix is singular\n", __FUNCTION__);

	// Pivot - switch rows k and i
	if (k != i)
	    for (j = 0; j < 3; j++) {
		c = mat[i][j];
		mat[i][j] = mat[k][j];
		mat[k][j] = c;
		c = XYZ2RGB[i][j];
		XYZ2RGB[i][j] = XYZ2RGB[k][j];
		XYZ2RGB[k][j] = c;
	    }

	// Normalize the row - make the diagonal 1
	c = 1.0/mat[i][i];
	for (j = 0; j < 3; j++) {
	    if (j != i)
		mat[i][j] *= c;
	    else
		mat[i][j] = 1.0;
	    XYZ2RGB[i][j] *= c;
	}

	// Zero the non-diagonal terms in this column
	for (j = 0; j < 3; j++)
	    if (j != i) {
		c = -mat[j][i];
		for (k = 0; k < 3; k++) {
		    mat[j][k] += c*mat[i][k];
		    XYZ2RGB[j][k] += c*XYZ2RGB[i][k];
		}
	    }
    }
}


    //  Color Conversions

void ColorSpace::Convert(const Color_RGB &rgb, Color_XYZ &xyz) const
{
    xyz.x = RGB2XYZ[0][0]*rgb.r+RGB2XYZ[0][1]*rgb.g+RGB2XYZ[0][2]*rgb.b;
    xyz.y = RGB2XYZ[1][0]*rgb.r+RGB2XYZ[1][1]*rgb.g+RGB2XYZ[1][2]*rgb.b;
    xyz.z = RGB2XYZ[2][0]*rgb.r+RGB2XYZ[2][1]*rgb.g+RGB2XYZ[2][2]*rgb.b;
}


void ColorSpace::Convert(const Color_XYZ &xyz, Color_RGB &rgb) const
{
    rgb.r = XYZ2RGB[0][0]*xyz.x+XYZ2RGB[0][1]*xyz.y+XYZ2RGB[0][2]*xyz.z;
    rgb.g = XYZ2RGB[1][0]*xyz.x+XYZ2RGB[1][1]*xyz.y+XYZ2RGB[1][2]*xyz.z;
    rgb.b = XYZ2RGB[2][0]*xyz.x+XYZ2RGB[2][1]*xyz.y+XYZ2RGB[2][2]*xyz.z;
}


#define DARK_XYZ_VALUE	(0.008856)
#define DARK_L_VALUE	(7.999625)
#define DARK_AB_VALUE	(0.206893)

void ColorSpace::Convert(const Color_XYZ &xyz, Color_LAB &lab) const
{
    f32 x, y, z;

    x = xyz.x/WhitePoint.x;
    y = xyz.y;
    z = xyz.z/WhitePoint.z;

    if (y <= DARK_XYZ_VALUE) {
	lab.l = 903.3*y;
	y = pow(7.787*y+16.0/116.0, 1.0/3.0);
    } else {
	y = pow(y, 1.0/3.0);
	lab.l = 116.0*y-16;
    }
#if 0
    if (x <= DARK_XYZ_VALUE)
	x = 7.787*x+16.0/116.0;
    if (z <= DARK_XYZ_VALUE)
	z = 7.787*z+16.0/116.0;
#endif
    x = pow(x, 1.0/3.0);
    z = pow(z, 1.0/3.0);
    lab.a = 500*(x-y);
    lab.b = 200*(y-z);
}


void ColorSpace::Convert(const Color_LAB &lab, Color_XYZ &xyz) const
{
    f32 x, y, z, t;

    if (lab.l <= DARK_L_VALUE) {
	y = lab.l/903.3;
	t = pow(7.787*y+16.0/116.0, 1.0/3.0);
    } else {
	t = (lab.l+16.0)/116.0;
	y = pow(t, 3.0);
    }
    x = pow(lab.a/500.0+t, 3.0);
    z = pow(t-lab.b/200.0, 3.0);
#if 0
    if (x <= DARK_AB_VALUE)
	x = (x-16.0/116.0)/7.787;
    if (y <= DARK_AB_VALUE)
	y = (y-16.0/116.0)/7.787;
#endif

    xyz.x = x*WhitePoint.x;
    xyz.y = y;
    xyz.z = z*WhitePoint.z;
}


    //  Color to Gray Conversions

void ColorSpace::Convert(const Color_RGB &rgb, Color_CIEY &y) const
{
    y.y = RGB2XYZ[1][0]*rgb.r+RGB2XYZ[1][1]*rgb.g+RGB2XYZ[1][2]*rgb.b;
}


    //  Gray Conversions

void ColorSpace::Convert(const Color_CIEY &y, Color_CIEL &l)
{
    if (y.y <= DARK_XYZ_VALUE)
	l.l = 903.3*y.y;
    else
	l.l = 116.0*pow(y.y, 1.0/3.0)-16;
}


void ColorSpace::Convert(const Color_CIEL &l, Color_CIEY &y)
{
    if (l.l <= DARK_L_VALUE)
	y.y = l.l/903.3;
    else
	y.y = pow((l.l+16.0)/116.0, 3.0);
}


    //  Color Conversions (16 bit)

void ColorSpace::Convert(const Color_RGB16 &rgb, Color_YUVr16 &yuvr)
{
    yuvr.yr = (rgb.r+2*rgb.g+rgb.b)>>2;
    yuvr.ur = rgb.r-rgb.g;
    yuvr.vr = rgb.b-rgb.g;
}

void ColorSpace::Convert(const Color_YUVr16 &yuvr, Color_RGB16 &rgb)
{
    rgb.g = yuvr.yr-((yuvr.ur+yuvr.vr)>>2);
    rgb.r = yuvr.ur+rgb.g;
    rgb.b = yuvr.vr+rgb.g;
}
