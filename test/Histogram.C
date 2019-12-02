//
//	Create histograms of the various subbands
//
//  $Id: Histogram.C,v 4.3.4.1 1999/07/20 16:16:19 geert Exp $
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <waili/Image.h>
#include <waili/Storage.h>


const char *ProgramName;
const char *Prefix = NULL;


static void
#ifdef __GNUG__
__attribute__ ((noreturn))
#endif // __GNUG__
Usage(void)
{
    Die("\nUsage: %s [options] filename [prefix]\n\n"
	"Valid options are:\n"
	"    -h, --help            display this usage information\n"
	"    -p, --primal number   number of primal vanishing moments (default: 2)\n"
	"    -d, --dual number     number of dual vanishing moments (default: 2)\n"
	"    -l, --levels          number of transform levels (default: 1)\n"
	"\n",
	ProgramName);
}


void CreateHistogram(const NTChannel *ntch, u_int channel, u_int level,
		     u_int subband)
{
    char *epsfile = new char[strlen(Prefix)+sizeof("_xx_xx_x.eps")];
    sprintf(epsfile, "%s_%02d_%02d_%1d.eps", Prefix, channel, level, subband);

    Stream stream;
    PixType min, max;
    u64 numpixels;
    u64 *histogram = ntch->FullHistogram(min, max, numpixels);
    char *plotdata = new char[sizeof("/tmp/tmp.xxxxx.data")];
    sprintf(plotdata, "/tmp/tmp.%05d.data", getpid());
    stream.Open(plotdata, "w");
    for (int i = min; i <= max; i++)
#ifdef __linux__
	stream.Printf("%d\t%Ld\n", i, histogram[i-min]);
#else // no %Ld
	stream.Printf("%d\t%ld\n", i, (long)histogram[i-min]);
#endif // no %Ld
    stream.Close();

    char *plotscript = new char[sizeof("/tmp/tmp.xxxxx.script")];
    sprintf(plotscript, "/tmp/tmp.%05d.script", getpid());
    stream.Open(plotscript, "w");
    stream.Puts("set term postscript eps\n");
    stream.Printf("set output '%s'\n", epsfile);
    stream.Printf("plot [%d:%d]\"%s\" title \"Channel %d Level %d Subband %d "
		  "[%d:%d]\" with impulses\n",
		  min, max, plotdata, channel, level, subband, min, max);
    stream.Puts("quit\n");
    stream.Close();

    char *command = new char[strlen(plotscript)+sizeof("gnuplot ")];
    sprintf(command, "gnuplot %s", plotscript);
    system(command);

    delete [] command;
    unlink(plotscript);
    delete [] plotscript;
    unlink(plotdata);
    delete [] plotdata;
    delete [] epsfile;
}


int main(int argc, char *argv[])
{
    const char *infile = NULL;
    Image image;
    u_int maxlevels = 1;
    int np = 2, nd = 2;
    Wavelet *wavelet = NULL;

    ProgramName = argv[0];
    while (--argc > 0) {
	argv++;
	if (!strcmp(argv[0], "-h") || !strcmp(argv[0], "--help"))
	    Usage();
	else if (!strcmp(argv[0], "-p") || !strcmp(argv[0], "--primal"))
	    if (argc > 1) {
		np = atoi(argv[1]);
		argc--;
		argv++;
	    } else
		Usage();
	else if (!strcmp(argv[0], "-d") || !strcmp(argv[0], "--dual"))
	    if (argc > 1) {
		nd = atoi(argv[1]);
		argc--;
		argv++;
	    } else
		Usage();
	else if (!strcmp(argv[0], "-l") || !strcmp(argv[0], "--levels"))
	    if (argc > 1) {
		maxlevels = atoi(argv[1]);
		argc--;
		argv++;
	    } else
		Usage();
	else if (!infile)
	    infile = argv[0];
	else if (!Prefix)
	    Prefix = argv[0];
	else
	    Usage();
    }
    if (!infile)
	Usage();
    if (!Prefix)
	Prefix = infile;
    image.Import(infile);
    wavelet = Wavelet::CreateCDF(np, nd);
    for (u_int ch = 0; ch < image.GetChannels(); ch++) {
	fprintf(stderr, "channel %d: 0", ch);
	NTChannel *ntch = (NTChannel *)image[ch];
	CreateHistogram(ntch, ch, 0, 0);
	for (u_int l = 1; l <= maxlevels; l++) {
	    LChannel *lch = ntch->PushFwtStepCR(*wavelet);
	    if (!lch)
		break;
	    fprintf(stderr, " %d", l);
	    for (u_int sb = SubBand_LL; sb <= SubBand_HH; sb++)
		CreateHistogram((const NTChannel *)(*lch)[(SubBand)sb], ch, l,
				sb);
	    ntch = (NTChannel *)(*lch)[SubBand_LL];
	}
	fputs("\n", stderr);
    }
    return(0);
}
