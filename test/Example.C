//
//	Simple image compression example
//
//  $Id: Example.C,v 4.0.2.1.2.1 1999/07/20 16:16:19 geert Exp $
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

#ifndef NULL
#define NULL	0
#endif

#include <waili/Image.h>

int main(void)
{
    const char infile[] = "image.pgm";
    const char outfile[] = "result.pgm";
    double threshold = 20.0;
    Image image;

    // Read the image
    image.Import(infile);

    // Transform the image using the Cohen-Daubechies-Feauveau
    // (2, 2) biorthogonal wavelets
    Wavelet *wavelet = Wavelet::CreateCDF(2, 2);
    u8 id = wavelet->GetID();
    delete wavelet;
    TransformDescriptor transform[] = {
	{ TT_ColsRows, id },
	{ TT_ColsRows, id },
	{ TT_ColsRows, id },
	{ TT_ColsRows, id },
	{ TT_ColsRows, id },
	{ TT_ColsRows, id },
	{ TT_ColsRows, id },
	{ TT_ColsRows, id }
    };
    image.Fwt(transform, sizeof(transform)/sizeof(*transform));

    // Zero all entries smaller than the threshold
    for (u_int ch = 0; ch < image.GetChannels(); ch++)
	image[ch]->Threshold(threshold);

    // Inverse wavelet transform
    image.IFwt();

    // Write the reconstructed image to a file
    image.Export(outfile);
    return(0);
}
