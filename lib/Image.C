//
//	Image Class
//
//  $Id: Image.C,v 4.4.2.4.2.1 1999/07/20 16:15:49 geert Exp $
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

#include <string.h>

#include <waili/Image.h>
#include <waili/Util.h>
#include <waili/Blit.h>

#include <waili/Timer.h>
#include <waili/Storage.h>
#include <waili/Color.h>

#include <errno.h>

#ifdef SUPPORT_TIFF
#include <tiffio.h>
#endif // SUPPORT_TIFF


const char *Image::rcsid = "$Id: Image.C,v 4.4.2.4.2.1 1999/07/20 16:15:49 geert Exp $";


Image::Image(u_int cols, u_int rows, u_int channels)
    : NumChannels(channels), Channels(NULL)
{
    Channels = new (Channel *)[NumChannels];
    for (u_int ch = 0; ch < NumChannels; ch++)
	Channels[ch] = new NTChannel(cols, rows);
}

Image::Image(const u_int cols[], const u_int rows[], u_int channels)
    : NumChannels(channels), Channels(NULL)
{
    Channels = new (Channel *)[NumChannels];
    for (u_int ch = 0; ch < NumChannels; ch++)
	Channels[ch] = new NTChannel(cols[ch], rows[ch]);
}

Image::Image(const Channel &channel, u_int channels = 1)
    : NumChannels(channels), Channels(NULL)
{
    Channels = new (Channel *)[NumChannels];
    for (u_int ch = 0; ch < NumChannels; ch++)
	Channels[ch] = channel.Clone();
}

Image::Image(const Channel *channel[], u_int channels)
    : NumChannels(channels), Channels(NULL)
{
    Channels = new (Channel *)[NumChannels];
    for (u_int ch = 0; ch < NumChannels; ch++)
	Channels[ch] = channel[ch]->Clone();
}

Image::Image(const Image &im)
    : NumChannels(im.NumChannels), Channels(NULL)
{
    Channels = new (Channel *)[NumChannels];
    for (u_int ch = 0; ch < NumChannels; ch++)
	Channels[ch] = im.Channels[ch]->Clone();
}

Image::Image(u_int cols, u_int rows, const TransformDescriptor transform[],
	     u_int depth, u_int channels)
    : NumChannels(channels), Channels(NULL)
{
    Channels = new (Channel *)[NumChannels];
    for (u_int ch = 0; ch < NumChannels; ch++)
	Channels[ch] = Channel::CreateFromDescriptor(cols, rows, transform,
						     depth);
}

Image::~Image()
{
    if (Channels)
	for (u_int ch = 0; ch < NumChannels; ch++)
	    delete Channels[ch];
    delete[] Channels;
}


// ----------------------------------------------------------------------------
//
//	Generic Routines
//
// ----------------------------------------------------------------------------


    //  Import/Export of Graphics Files


ImageType Image::Import(const char *filename, ImageFormat type)
{
    ImageType res = IT_Unknown;

    switch (type) {
	case IF_AUTO:
#ifdef SUPPORT_TIFF
	    if (strstr(filename, "tif"))
		res = ImportTIFF(filename);
	    else
#endif // SUPPORT_TIFF
		res = ImportPNM(filename);
	    break;
	case IF_PNMASCII:
	case IF_PNMRAW:
	    res = ImportPNM(filename);
	    break;
#ifdef SUPPORT_TIFF
	case IF_TIFF:
	    res = ImportTIFF(filename);
	    break;
#endif // SUPPORT_TIFF
    }
    return(res);
}


void Image::Export(const char *filename, ImageFormat type)
{
    switch (type) {
	case IF_AUTO:
#ifdef SUPPORT_TIFF
	    if (strstr(filename, "tif"))
		Die("%s: Export to TIFF isn't implemented yet!\n",
		    __FUNCTION__);
	    else
#endif // SUPPORT_TIFF
		ExportPNM(filename, 1);
	    break;
	case IF_PNMASCII:
	    ExportPNM(filename, 0);
	    break;
	case IF_PNMRAW:
	    ExportPNM(filename, 1);
	    break;
#ifdef SUPPORT_TIFF
	case IF_TIFF:
	    Die("%s: Export to TIFF isn't implemented yet!\n", __FUNCTION__);
	    break;
#endif // SUPPORT_TIFF
    }
}


    //  Static Routines

static u8 GetRealChar(Stream &stream);
static u_int GetNumber(Stream &stream);


    //  Import an Image from a File in the PNM (PGM or PPM) Format

ImageType Image::ImportPNM(const char *filename)
{
    ImageType res;
    Stream stream(filename, "r");
    u_int cols, rows, channels;
    u_int c, r, ch;
    u8 d, magic;
    int raw = 0;

    stream.Read(magic);
    if (magic != 'P')
	Die("%s: Not a PNM file\n", __FUNCTION__);
    stream.Read(magic);
    switch (magic) {
	case '1':	// PBM ASCII
	case '4':	// PBM Raw
	    Die("%s: PBM is not supported\n", __FUNCTION__);
	    break;

	case '5':	// PGM Raw
	    raw = 1;
	case '2':	// PGM ASCII
	    channels = 1;
	    res = IT_CIEY;
	    break;

	case '6':	// PPM Raw
	    raw = 1;
	case '3':	// PPM ASCII
	    channels = 3;
	    res = IT_RGB;
	    break;

	default:
	    Die("%s: Not a PNM file\n", __FUNCTION__);
	    break;
    }
    cols = GetNumber(stream);
    rows = GetNumber(stream);
    GetNumber(stream);
    Resize(cols, rows, channels);
    if (raw)
	for (r = 0; r < rows; r++)
	    for (c = 0; c < cols; c++)
		for (ch = 0; ch < channels; ch++) {
		    stream.Read(d);
		    (*this)(c, r, ch) = (PixType)(d-128);
		}
    else
	for (r = 0; r < rows; r++)
	    for (c = 0; c < cols; c++)
		for (ch = 0; ch < channels; ch++)
		    (*this)(c, r, ch) = (PixType)(GetNumber(stream)-128);
    return(res);
}


#ifdef SUPPORT_TIFF

    //  Import an Image from a File in the TIFF Format

ImageType Image::ImportTIFF(const char *filename)
{
    ImageType res;
    TIFF *tif;
    u_short bps, spp, photomet;
    u_int tiffmaxval, sign, bitsleft;
    u_int cols, rows, channels;
    u_int r, c, ch;
    int sample;
    u_char *buf, *p;

    if (!(tif = TIFFOpen(filename, "r")))
	Die("%s: TIFFOpen() failed: %s\n", __FUNCTION__, strerror(errno));

    if (!TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bps))
	bps = 1;
    if (!TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &spp))
	spp = 1;
    if (!TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &photomet))
	Die("%s: Can't get photometric\n", __FUNCTION__);

    switch (spp) {
	case 1:
	case 3:
	case 4:
	    break;

	default:
	    Die("%s: cannot handle %d samples per pixel\n", __FUNCTION__, spp);
    }
    channels = spp;
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &cols);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &rows);

    tiffmaxval = (1<<bps)-1;
    sign = 1<<(bps-1);

    if (tiffmaxval == 1 && spp == 1)
	res = IT_Mono;
    else
	switch (photomet) {
	    case PHOTOMETRIC_MINISBLACK:
	    case PHOTOMETRIC_MINISWHITE:
		res = IT_CIEY;
		break;

	    case PHOTOMETRIC_RGB:
#if 0
		f32 *tmp;
		if (TIFFGetField(tif, TIFFTAG_PRIMARYCHROMATICITIES, &tmp))
		    Color.SetPrimChroma(tmp);
		if (TIFFGetField(tif, TIFFTAG_WHITEPOINT, &tmp))
		    Color.SetWhitePoint(tmp);
#endif
		res = IT_RGB;
		break;

	    case PHOTOMETRIC_CIELAB:
		switch (channels) {
		    case 1:
			res = IT_CIEL;
			break;

		    case 3:
		    case 4:
			res = IT_CIELab;
			break;

		    default:
			Die("%s: cannot handle CIELab with %d samples per "
			    "pixel\n", __FUNCTION__, spp);
		}
		break;

	    default:
		Die("%s: Unsupported photometric %d\n", __FUNCTION__,
		    photomet);
	}

    Resize(cols, rows, channels);
    buf = new u_char[TIFFScanlineSize(tif)];

#define NEXTSAMPLE \
    { \
	if ( bitsleft == 0 ) { \
	    ++p; \
	    bitsleft = 8; \
	} \
	bitsleft -= bps; \
	sample = (*p>>bitsleft) & tiffmaxval; \
    }

    for (r = 0; r < rows; r++) {
	if (TIFFReadScanline(tif, buf, r, 0) < 0)
	    Die("%s: TIFFReadScanline() failed: %s\n", __FUNCTION__,
		strerror(errno));
	p = buf;
	bitsleft = 8;
	switch (photomet) {
	    case PHOTOMETRIC_MINISBLACK:
	    case PHOTOMETRIC_RGB:
		for (c = 0; c < cols; c++)
		    for (ch = 0; ch < channels; ch++) {
			NEXTSAMPLE
			(*this)(c, r, ch) = (PixType)(sample-128);
		    }
		break;

	    case PHOTOMETRIC_CIELAB:
		for (c = 0; c < cols; c++) {
		    NEXTSAMPLE		// L*
		    (*this)(c, r, 0) = (PixType)(sample-128);
		    if (channels == 1)
			continue;
		    NEXTSAMPLE		// a*
		    if (sample & sign)
			sample -= 2*sign;
		    (*this)(c, r, 2) = (PixType)(sample-128);
		    NEXTSAMPLE		// b*
		    if (sample & sign)
			sample -= 2*sign;
		    (*this)(c, r, 2) = (PixType)(sample-128);
		    if (channels == 4)
			NEXTSAMPLE	// Alpha
		}
		break;

	    case PHOTOMETRIC_MINISWHITE:
		for (c = 0; c < cols; c++) {
		    NEXTSAMPLE
		    sample = tiffmaxval - sample;
		    (*this)(c, r, 0) = (PixType)(sample-128);
		}
		break;
	}
    }
    delete[] buf;

    TIFFClose(tif);
    return(res);
}
#endif // SUPPORT_TIFF


    //  Export an Image to a File in the PNM (PGM or PPM) Format

void Image::ExportPNM(const char *filename, int raw)
{
    Stream stream(filename, "w");
    u8 magic, d;
    PixType val;
    u_int cols = GetCols(0);
    u_int rows = GetRows(0);
    u_int channels = GetChannels();
    for (u_int ch = 1; ch < channels; ch++)
	if (cols != GetCols(ch) || rows != GetRows(ch))
	    Die("%s: All channels must have the same size\n", __FUNCTION__);

    switch (channels) {
	case 1:
	    magic = '2';
	    break;

	case 3:
	    magic = '3';
	    break;

	default:
	    Die("%s: Unsupported number of channels %d\n", __FUNCTION__,
		channels);
    }
    if (raw)
	magic += 3;

    stream.Printf("P%c\n%d %d\n255", magic, cols, rows);
    if (raw) {
	stream.Write((u8)'\n');
	for (u_int r = 0; r < rows; r++)
	    for (u_int c = 0; c < cols; c++)
		for (u_int ch = 0; ch < channels; ch++) {
		    val = (*this)(c, r, ch)+128;
		    if (val < 0)
			d = 0;
		    else if (val > 255)
			d = 255;
		    else
			d = (u8)val;
		    stream.Write(d);
		}
    } else {
	u_int l = 70;
	for (u_int r = 0; r < rows; r++)
	    for (u_int c = 0; c < cols; c++)
		for (u_int ch = 0; ch < channels; ch++) {
		    val = (*this)(c, r, ch)+128;
		    if (val < 0)
			d = 0;
		    else if (val > 255)
			d = 255;
		    else
			d = (u8)val;
		    l += 4;
		    if (l >= 70) {
			stream.Printf("\n%4d", (u_int)d);
			l = 3;
		    } else
			stream.Printf(" %4d", (u_int)d);
		}
	stream.Write((u8)'\n');
    }
}


// ----------------------------------------------------------------------------


    //  Image Conversion

static u_int type2numchannels(ImageType t)
{
    switch (t) {
	case IT_Mono:
	case IT_CIEY:
	case IT_CIEL:
	    return 1;

	case IT_RGB:
	case IT_CIEXYZ:
	case IT_CIELab:
	case IT_YUV:
	case IT_YUVr:
	    return 3;

	case IT_Unknown:
	default:
	    return 0;
    }
}

void Image::Convert(ImageType from, ImageType to)
{
    u_int c, r;
    ColorSpace color;
    u_int cols = GetCols(0);
    u_int rows = GetRows(0);
    u_int channels = GetChannels();
    for (u_int ch = 1; ch < channels; ch++)
	if (cols != GetCols(ch) || rows != GetRows(ch))
	    Die("%s: All channels must have the same size\n", __FUNCTION__);
    if (NumChannels < type2numchannels(from) ||
	NumChannels < type2numchannels(to))
	Die("%s: Not enough channels for the specified conversion types\n",
	    __FUNCTION__);

    switch (from) {
	case IT_RGB:
	    switch (to) {
		case IT_RGB:
		    // Trivial
		    break;

		case IT_CIEXYZ:
		    for (r = 0; r < rows; r++)
			for (c = 0; c < cols; c++) {
			    Color_RGB rgb;
			    Color_XYZ xyz;
			    rgb.r = (*this)(c, r, 0);
			    rgb.g = (*this)(c, r, 1);
			    rgb.b = (*this)(c, r, 2);
			    color.Convert(rgb, xyz);
			    (*this)(c, r, 0) = (PixType)(xyz.x+0.5);
			    (*this)(c, r, 1) = (PixType)(xyz.y+0.5);
			    (*this)(c, r, 2) = (PixType)(xyz.z+0.5);
			}
		    break;

		case IT_CIELab:
		    for (r = 0; r < rows; r++)
			for (c = 0; c < cols; c++) {
			    Color_RGB rgb;
			    Color_LAB lab;
			    rgb.r = (*this)(c, r, 0)/255.0;
			    rgb.g = (*this)(c, r, 1)/255.0;
			    rgb.b = (*this)(c, r, 2)/255.0;
			    color.Convert(rgb, lab);
			    (*this)(c, r, 0) = (PixType)(lab.l*255.0/100.0+0.5);
			    if (lab.a < -128.0)
				lab.a = -128.0;
			    else if (lab.a > 127.0)
				lab.a = 127.0;
			    if (lab.b < -128.0)
				lab.b = -128.0;
			    else if (lab.b > 127.0)
				lab.b = 127.0;
			    (*this)(c, r, 1) = (PixType)(lab.a+0.5);
			    (*this)(c, r, 2) = (PixType)(lab.b+0.5);
			}
		    break;

		case IT_YUVr:
		    for (r = 0; r < rows; r++)
			for (c = 0; c < cols; c++) {
			    Color_RGB16 rgb;
			    Color_YUVr16 yuvr;
			    rgb.r = (*this)(c, r, 0);
			    rgb.g = (*this)(c, r, 1);
			    rgb.b = (*this)(c, r, 2);
			    ColorSpace::Convert(rgb, yuvr);
			    (*this)(c, r, 0) = yuvr.yr;
			    (*this)(c, r, 1) = yuvr.ur;
			    (*this)(c, r, 2) = yuvr.vr;
			}
		    break;

		default:
		    goto conversion_not_supported;
	    }
	    break;

	case IT_CIEXYZ:
	    switch (to) {
		case IT_RGB:
		    for (r = 0; r < rows; r++)
			for (c = 0; c < cols; c++) {
			    Color_XYZ xyz;
			    Color_RGB rgb;
			    xyz.x = (*this)(c, r, 0);
			    xyz.y = (*this)(c, r, 1);
			    xyz.z = (*this)(c, r, 2);
			    color.Convert(xyz, rgb);
			    (*this)(c, r, 0) = (PixType)(rgb.r+0.5);
			    (*this)(c, r, 1) = (PixType)(rgb.g+0.5);
			    (*this)(c, r, 2) = (PixType)(rgb.b+0.5);
			}
		    break;

		case IT_CIEXYZ:
		    // Trivial
		    break;

		case IT_CIELab:
		    for (r = 0; r < rows; r++)
			for (c = 0; c < cols; c++) {
			    Color_XYZ xyz;
			    Color_LAB lab;
			    xyz.x = (*this)(c, r, 0)/255.0;
			    xyz.y = (*this)(c, r, 1)/255.0;
			    xyz.z = (*this)(c, r, 2)/255.0;
			    color.Convert(xyz, lab);
			    (*this)(c, r, 0) = (PixType)(lab.l*255.0/100.0+0.5);
			    if (lab.a < -128.0)
				lab.a = -128.0;
			    else if (lab.a > 127.0)
				lab.a = 127.0;
			    if (lab.b < -128.0)
				lab.b = -128.0;
			    else if (lab.b > 127.0)
				lab.b = 127.0;
			    (*this)(c, r, 1) = (PixType)(lab.a+0.5);
			    (*this)(c, r, 2) = (PixType)(lab.b+0.5);
			}
		    break;

		default:
		    goto conversion_not_supported;
	    }
	    break;

	case IT_CIELab:
	    switch (to) {
		case IT_RGB:
		    for (r = 0; r < rows; r++)
			for (c = 0; c < cols; c++) {
			    Color_LAB lab;
			    Color_RGB rgb;
			    lab.l = (*this)(c, r, 0)*100.0/255.0;
			    lab.a = (*this)(c, r, 1);
			    lab.b = (*this)(c, r, 2);
			    color.Convert(lab, rgb);
			    (*this)(c, r, 0) = (PixType)(rgb.r*255.0+0.5);
			    (*this)(c, r, 1) = (PixType)(rgb.g*255.0+0.5);
			    (*this)(c, r, 2) = (PixType)(rgb.b*255.0+0.5);
			}
		    break;

		case IT_CIEXYZ:
		    for (r = 0; r < rows; r++)
			for (c = 0; c < cols; c++) {
			    Color_LAB lab;
			    Color_XYZ xyz;
			    lab.l = (*this)(c, r, 0)*100.0/255.0;
			    lab.a = (*this)(c, r, 1);
			    lab.b = (*this)(c, r, 2);
			    color.Convert(lab, xyz);
			    (*this)(c, r, 0) = (PixType)(xyz.x*255.0+0.5);
			    (*this)(c, r, 1) = (PixType)(xyz.y*255.0+0.5);
			    (*this)(c, r, 2) = (PixType)(xyz.z*255.0+0.5);
			}
		    break;

		case IT_CIELab:
		    // Trivial
		    break;

		default:
		    goto conversion_not_supported;
	    }
	    break;

	case IT_YUVr:
	    switch (to) {
		case IT_YUVr:
		    // Trivial
		    break;

		case IT_RGB:
		    for (r = 0; r < rows; r++)
			for (c = 0; c < cols; c++) {
			    Color_YUVr16 yuvr;
			    Color_RGB16 rgb;
			    yuvr.yr = (*this)(c, r, 0);
			    yuvr.ur = (*this)(c, r, 1);
			    yuvr.vr = (*this)(c, r, 2);
			    ColorSpace::Convert(yuvr, rgb);
			    (*this)(c, r, 0) = rgb.r;
			    (*this)(c, r, 1) = rgb.g;
			    (*this)(c, r, 2) = rgb.b;
			}
		    break;

		default:
		    goto conversion_not_supported;
	    }
	    break;

	case IT_CIEY:
	    switch (to) {
		case IT_CIEY:
		    // Trivial
		    break;

		case IT_CIEL:
		    for (r = 0; r < rows; r++)
			for (c = 0; c < cols; c++) {
			    Color_CIEY y;
			    Color_CIEL l;
			    y.y = (*this)(c, r, 0)*255.0;
			    color.Convert(y, l);
			    (*this)(c, r, 0) = (PixType)(l.l*255.0/100.0+0.5);
			};
		    break;

		default:
		    goto conversion_not_supported;
	    }
	    break;

	case IT_CIEL:
	    switch (to) {
		case IT_CIEY:
		    for (r = 0; r < rows; r++)
			for (c = 0; c < cols; c++) {
			    Color_CIEL l;
			    Color_CIEY y;
			    l.l = (*this)(c, r, 0)*100.0/255.0;
			    color.Convert(l, y);
			    (*this)(c, r, 0) = (PixType)(y.y*255.0+0.5);
			};
		    break;

		case IT_CIEL:
		    // Trivial
		    break;

		default:
		    goto conversion_not_supported;
	    }
	    break;

	default:
conversion_not_supported:
	    Die("%s: Conversion from type %d to type %d is not supported\n",
		__FUNCTION__, from, to);
    }
}


    //  Read the Next Character from a PNM File and Care about Comments

static u8 GetRealChar(Stream &stream)
{
    u8 c;

    for (stream.Read(c); c == '#'; stream.Read(c))
	do
	    stream.Read(c);
	while (c != '\n');
    return(c);
}


    //  Skip Leading Whitespace and Read an ASCII Number from a PNM File

static u_int GetNumber(Stream &stream)
{
    u_int value = 0;
    u8 c;

    do
	c = GetRealChar(stream);
    while ((c == ' ') || (c == '\t') || (c == '\r') || (c == '\n'));
    while (1) {
	if ((c >= '0') && (c <= '9')) {
	    value = 10*value+(c-'0');
	    c = GetRealChar(stream);
	    continue;
	}
	if ((c == ' ') || (c == '\t') || (c == '\r') || (c == '\n'))
	    break;
	Die("%s: Not a number\n", __FUNCTION__);
    }
    return(value);
}


// ---------------------------------------------------------------------------


    //  Manipulation


void Image::Resize(u_int cols, u_int rows, u_int channels)
{
    for (u_int ch = 0; ch < NumChannels; ch++)
	delete Channels[ch];
    delete[] Channels;
    NumChannels = channels;
    Channels = new (Channel *)[NumChannels];
    for (u_int ch = 0; ch < NumChannels; ch++)
	Channels[ch] = new NTChannel(cols, rows);
}

Image& Image::operator=(const Image &im)
{
    if (this != &im) {
	for (u_int ch = 0; ch < NumChannels; ch++)
	    delete Channels[ch];
	delete[] Channels;
	NumChannels = im.NumChannels;
	Channels = new (Channel *)[NumChannels];
	for (u_int ch = 0; ch < NumChannels; ch++)
	    Channels[ch] = im[ch]->Clone();
    }
    return(*this);
}

void Image::InsertChannel(const Channel &data, u_int channel)
{
    Channel **newChannels = new (Channel *)[++NumChannels];
    for (u_int ch = 0; ch < channel; ch++)
	newChannels[ch] = Channels[ch];
    for (u_int ch = channel+1; ch < NumChannels; ch++)
	newChannels[ch] = Channels[ch-1];
    newChannels[channel] = data.Clone();
    delete[] Channels;
    Channels = newChannels;
}

void Image::DeleteChannel(u_int channel)
{
    Channel **newChannels = new (Channel *)[--NumChannels];
    for (u_int ch = 0; ch < channel; ch++)
	newChannels[ch] = Channels[ch];
    for (u_int ch = channel; ch < NumChannels; ch++)
	newChannels[ch] = Channels[ch+1];
    delete Channels[channel];
    delete[] Channels;
    Channels = newChannels;
}

    //  Wavelet Transforms

void Image::Fwt(const TransformDescriptor transform[], u_int depth)
{
    for (u_int ch = 0; ch < NumChannels; ch++) {
	assert(!(*this)[ch]->IsLifted());
	LChannel *channel = ((NTChannel *)((*this)[ch]))->Fwt(transform, depth);
	if (channel) {
	    delete Channels[ch];
	    Channels[ch] = channel;
	}
    }
}

void Image::IFwt(void)
{
    for (u_int ch = 0; ch < NumChannels; ch++)
	if ((*this)[ch]->IsLifted()) {
	    NTChannel *channel = ((LChannel *)((*this)[ch]))->IFwt();
	    if (channel) {
		delete Channels[ch];
		Channels[ch] = channel;
	    }
	}
}

void Image::RedundantFwt(const Wavelet &wavelet, TransformType type)
{
    for (u_int ch = 0; ch < NumChannels; ch++) {
	assert(!(*this)[ch]->IsLifted());
	LChannel *channel = NULL;
	switch (type) {
	    case TT_ColsRows:
		channel = ((NTChannel*)(*this)[ch])->RedundantFwtCR(wavelet);
		break;
	    case TT_Cols:
		channel = ((NTChannel*)(*this)[ch])->RedundantFwtC(wavelet);
		break;
	    case TT_Rows:
		channel = ((NTChannel*)(*this)[ch])->RedundantFwtR(wavelet);
		break;
	}
	if (channel) {
	    delete Channels[ch];
	    Channels[ch] = channel;
	}
    }
}


    // Wavelet Based Operations

void Image::Scale(f32 scale, const Wavelet &wavelet)
{
    for (u_int ch = 0; ch < NumChannels; ch++) {
	Channel *channel = (*this)[ch]->Scale(scale, wavelet);
	if (channel) {
	    delete Channels[ch];
	    Channels[ch] = channel;
	}
    }
}


