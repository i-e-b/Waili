//
//	Image Class
//
//  $Id: Image.h,v 4.6.2.3.2.1 1999/07/20 16:15:44 geert Exp $
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

#ifndef WAILI_IMAGE_H
#define WAILI_IMAGE_H

#include <assert.h>
#include <stream.h>

#include "Channel.h"
#include "Util.h"


    //  Image Types and File Types

enum ImageType {
    IT_Unknown, IT_Mono, IT_CIEY, IT_CIEL, IT_RGB, IT_CIEXYZ, IT_CIELab,
    IT_YUV, IT_YUVr
};
enum ImageFormat {
    IF_AUTO, IF_PNMASCII, IF_PNMRAW
#ifdef SUPPORT_TIFF
    , IF_TIFF
#endif // SUPPORT_TIFF
};


    //  Image Class

class Image {
    public:
	// Construction/Destruction
	Image();
	Image(u_int channels);
	Image(u_int cols, u_int rows, u_int channels = 1);
	Image(const u_int cols[], const u_int rows[], u_int channels);
	Image(const Channel &channel, u_int channels = 1);
	Image(const Channel *channel[], u_int channels);
	Image(const Image &im);
	Image(u_int cols, u_int rows, const TransformDescriptor transform[],
	      u_int depth, u_int channels = 1);
	~Image();

	// Import/Export of Images
	ImageType Import(const char *filename, ImageFormat type = IF_AUTO);
	void Export(const char *filename, ImageFormat type = IF_AUTO);

	//  Image Conversion
	void Convert(ImageType from, ImageType to);

	// Properties
	u_int GetChannels(void) const;
	u_int GetCols(u_int channel) const;
	u_int GetRows(u_int channel) const;
	int GetOffsetX(void) const;
	int GetOffsetY(void) const;

	// Manipulation
	Channel*& operator[](u_int channel);
	const Channel*& operator[](u_int channel) const;
	PixType& operator()(u_int c, u_int r, u_int ch = 0);
	const PixType operator()(u_int c, u_int r, u_int ch = 0) const;

	void Clear(void);
	void Resize(u_int cols, u_int rows);
	void Resize(u_int cols, u_int rows, u_int channels);
	Image& operator=(const Image &im);
	Image *Clone(void) const;

	void SetOffsetX(int offx);
	void SetOffsetY(int offy);

	Image *Crop(int x1, int y1, int x2, int y2) const;
	void Merge(const Image &im);
	void Add(const Image &im);
	void Subtract(const Image &im);
	Image *Diff(const Image &im) const;

	void InsertChannel(const Channel &data, u_int ch);
	void DeleteChannel(u_int channel);

	// Wavelet Transforms
	void Fwt(const TransformDescriptor transform[], u_int depth);
	void IFwt(void);

	void RedundantFwt(const Wavelet &wavelet, TransformType type);

	// Wavelet Based Operations
	void Scale(f32 scale, const Wavelet &wavelet);

    private:
	//  Low-level Image Import/Export
	ImageType ImportPNM(const char *filename);
	ImageType ImportTIFF(const char *filename);
	void ExportPNM(const char *filename, int raw);

    protected:
	u_int NumChannels;
	Channel **Channels;

    private:
	static const char *rcsid;
};


/////////////////////////////////////////////////////////////////////////////
//
//      Inline Member Functions
//
/////////////////////////////////////////////////////////////////////////////


inline Image::Image()
    : NumChannels(0), Channels(NULL)
{}

inline Image::Image(u_int channels)
    : NumChannels(channels), Channels(NULL)
{
    Channels = new (Channel *)[NumChannels];
    ::Clear(Channels, NumChannels);
}


inline Channel*& Image::operator[](u_int channel)
{
#ifdef BOUNDS_CHECK
    assert(channel < NumChannels);
#endif // BOUNDS_CHECK
    return(Channels[channel]);
}

inline const Channel*& Image::operator[](u_int channel) const
{
#ifdef BOUNDS_CHECK
    assert(channel < NumChannels);
#endif // BOUNDS_CHECK
    return(Channels[channel]);
}

inline u_int Image::GetChannels(void) const
{
    return(NumChannels);
}

inline u_int Image::GetCols(u_int channel) const
{
#ifdef BOUNDS_CHECK
    assert(channel < NumChannels);
#endif // BOUNDS_CHECK
    assert(Channels[channel] != NULL);
    return(Channels[channel]->GetCols());
}

inline u_int Image::GetRows(u_int channel) const
{
#ifdef BOUNDS_CHECK
    assert(channel < NumChannels);
#endif // BOUNDS_CHECK
    assert(Channels[channel] != NULL);
    return(Channels[channel]->GetRows());
}

inline int Image::GetOffsetX(void) const
{
    assert(Channels[0] != NULL);
    return(Channels[0]->GetOffsetX());
}

inline int Image::GetOffsetY(void) const
{
    assert(Channels[0] != NULL);
    return(Channels[0]->GetOffsetY());
}

inline PixType& Image::operator()(u_int c, u_int r, u_int ch)
{
    return((*(*this)[ch])(c, r));
}

inline const PixType Image::operator()(u_int c, u_int r, u_int ch) const
{
    return((*(*this)[ch])(c, r));
}

inline void Image::Clear(void)
{
    for (u_int ch = 0; ch < NumChannels; ch++)
	(*this)[ch]->Clear();
}

inline void Image::Resize(u_int cols, u_int rows)
{
    for (u_int ch = 0; ch < NumChannels; ch++)
	(*this)[ch]->Resize(cols, rows);
}

inline Image *Image::Clone(void) const
{
    return(new Image(*this));
}

inline void Image::SetOffsetX(int offx)
{
    for (u_int ch = 0; ch < NumChannels; ch++)
	(*this)[ch]->SetOffsetX(offx);
}

inline void Image::SetOffsetY(int offy)
{
    for (u_int ch = 0; ch < NumChannels; ch++)
	(*this)[ch]->SetOffsetY(offy);
}

inline Image *Image::Crop(int x1, int y1, int x2, int y2) const
{
    Image *im = new Image(NumChannels);
    for (u_int ch = 0; ch < NumChannels; ch++)
	(*im)[ch] = (*this)[ch]->Crop(x1, y1, x2, y2);
    return(im);
}

inline void Image::Merge(const Image &im)
{
    assert(im.GetChannels() == NumChannels);
    for (u_int ch = 0; ch < NumChannels; ch++)
	(*this)[ch]->Merge(*im[ch]);
}

inline void Image::Add(const Image &im)
{
    assert(im.GetChannels() == NumChannels);
    for (u_int ch = 0; ch < NumChannels; ch++)
	(*this)[ch]->Add(*im[ch]);
}

inline void Image::Subtract(const Image &im)
{
    assert(im.GetChannels() == NumChannels);
    for (u_int ch = 0; ch < NumChannels; ch++)
	(*this)[ch]->Subtract(*im[ch]);
}

inline Image *Image::Diff(const Image &im) const
{
    assert(im.GetChannels() == NumChannels);
    Image *diff = new Image(NumChannels);
    for (u_int ch = 0; ch < NumChannels; ch++)
	(*diff)[ch] = (*this)[ch]->Diff(*im[ch]);
    return(diff);
}


#endif // WAILI_IMAGE_H
