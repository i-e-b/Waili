//
//	Simple Interactive Demo
//
//  $Id: Demo.C,v 4.6.2.3.2.1 1999/07/20 16:16:19 geert Exp $
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
#include <sys/time.h>
#include <unistd.h>


#include <waili/Image.h>
#include <waili/Storage.h>

#define MAX_CMD_LINE	256

int ExitFlag = 0;
char CommandLine[MAX_CMD_LINE];
char *File = NULL;
Wavelet *MyWavelet = NULL;
Image MyImage;
Image *Backup = NULL;
int ImageLoaded = 0;
const char **DeleteList = NULL;
u_int DeleteListLen = 0;


struct Command {
    const char *Name;
    void (*Function)(int argc, const char **);
};

#define arraysize(x)	(sizeof(x)/sizeof(*(x)))


static const char *TmpFileName(void)
{
    static int cnt = 0;
    char *filename = new char[sizeof("/var/tmp/Demo.xxxxx.xxxxx")];
    sprintf(filename, "/var/tmp/Demo.%05d.%05d", getpid(), cnt++);
    DeleteListLen++;
    const char **list = new const char *[DeleteListLen];
    for (u_int i = 0; i < DeleteListLen-1; i++)
	list[i] = DeleteList[i];
    list[DeleteListLen-1] = filename;
    if (DeleteList)
	delete [] DeleteList;
    DeleteList = list;
    return filename;
}


static void CleanUp(void)
{
    if (DeleteList) {
	for (u_int i = 0; i < DeleteListLen; i++) {
	    unlink(DeleteList[i]);
	    delete [] DeleteList[i];
	}
	delete [] DeleteList;
    }
}


static char *ReadCommandLine(void)
{
    fgets(CommandLine, MAX_CMD_LINE, stdin);
    u_int last = strlen(CommandLine)-1;
    if (CommandLine[last] == '\n')
	CommandLine[last] = '\0';
    return CommandLine;
}


    /*
     *  Split a line into arguments
     */

static void CreateArgs(char *cmdline, int *argcp, const char **argvp[])
{
    int argc = 0, i;
    char *p, *q;
    const char **argv = NULL;

    *argcp = 0;
    p = cmdline;
    for (argc = 0;; argc++) {
	while (*p == ' ' || *p == '\t')
	    p++;
	if (!*p)
	    break;
	if (*p == '"') {
	    for (p++; *p && *p != '"'; p++)
		if (*p == '\\') {
		    p++;
		    if (!*p)
			break;
		}
	    if (*p)
		p++;
	} else
	    while (*p && *p != ' ' && *p != '\t')
		p++;
    }
    if (argc) {
	if (!(argv = new const char*[argc+1]))
	    return;
	p = cmdline;
	for (i = 0; i < argc; i++) {
	    while (*p == ' ' || *p == '\t')
		p++;
	    if (!*p)
		break;
	    if (*p == '"') {
		q = p++;
		argv[i] = q;
		while (*p && *p != '"') {
		    if (*p == '\\') {
			p++;
			if (!*p)
			    break;
		    }
		    *q++ = *p++;
		}
	    } else {
		q = p;
		argv[i] = q;
		while (*p && *p != ' ' && *p != '\t') {
		    if (*p == '\\') {
			p++;
			if (!*p)
			    break;
		    }
		    *q++ = *p++;
		}
	    }
	    *q = '\0';
	    p++;
	}
    }
    *argcp = argc;
    *argvp = argv;
}


static void DeleteArgs(int argc, const char *argv[])
{
    if (argc)
	delete [] argv;
}


    /*
     *  Partial strcasecmp() (s1 may be an abbreviation for s2)
     */

static int PartStrCaseCmp(const char *s1, const char *s2)
{
    char c1, c2;

    while (*s1) {
	c1 = *s1++;
	if (c1 >= 'A' && c1 <= 'Z')
	    c1 += ('a'-'A');
	c2 = *s2++;
	if (c2 >= 'A' && c2 <= 'Z')
	    c2 += ('a'-'A');
	if (!c2 || c1 != c2)
	    return 0;
    }
    return 1;
}


static int ExecCommand(const struct Command commands[], u_int numcommands,
		       int argc, const char *argv[])
{
    if (argc)
	for (u_int i = 0; i < numcommands; i++)
	    if (PartStrCaseCmp(argv[0], commands[i].Name)) {
		commands[i].Function(argc-1, argv+1);
		return 1;
	    }
    return 0;
}

static void Do_NotYetImplemented(int, const char **)
{
    fputs("Not yet implemented\n", stderr);
}

static int NoImage(void)
{
    int res = 0;

    if (!ImageLoaded) {
	fputs("No image loaded\n", stderr);
	res = 1;
    }
    return res;
}

static int NoBackup(void)
{
    int res = 0;

    if (!Backup) {
	fputs("No backup image present\n", stderr);
	res = 1;
    }
    return res;
}

static int NoWavelet(void)
{
    int res = 0;

    if (!MyWavelet) {
	fputs("No wavelet specified\n", stderr);
	res = 1;
    }
    return res;
};

void Do_Help(int, const char **)
{
    fputs("\nDemo\n\n"
	 "    Help, ?            Display this help\n"
	 "    Quit, eXit         Terminate program\n"
	 "    Load <image>       Load an image\n"
	 "    Save <image>       Save an image\n"
	 "    View               View the current image\n"
	 "    Wavelet <np> <nd>  Specify the CDF wavelet\n"
	 "    Wavelet <n>        Specify another wavelet type:\n"
	 "                          1: CRF (13, 7)\n"
	 "                          2: SWE (13, 7)\n"
	 "    Fstep cr           One forward transform step (rows and columns)\n"
	 "    Fstep c            One forward transform step (columns only)\n"
	 "    Fstep r            One forward transform step (rows only)\n"
	 "    Bstep              One backward transform step\n"
	 "    Ifwt               Full inverse transform\n"
	 "    Noise <var>        Add Gaussian noise with a specific variance\n"
	 "    Denoise            GCV Denoising\n"
	 "    BAckup             Create a backup of the current image\n"
	 "    Psnr               PSNR, compared to the backup image\n"
	 "    Threshold <value>  Hard thresholding with a specific threshold\n"
	 "    SCale <value>      Scale the image\n"
	 "    Histogram <level> <subband> <channel>\n"
	 "                       View the histogram of a subband\n"
	 "    Entropy            Calculate the first-order entropy in bits per pixel\n"
	 "    Yuv                Convert from RGB to YUVr (or vice versa)\n"
	 "\n", stderr);
}

void Do_Quit(int, const char **)
{
    ExitFlag = 1;
}

void Do_Load(int argc, const char *argv[])
{
    if (argc > 0) {
	delete File;
	File = new char[strlen(argv[0])+1];
	strcpy(File, argv[0]);
    }
    if (!File) {
	fputs("No file specified\n", stderr);
	return;
    }
    MyImage.Import(File);
    ImageLoaded = 1;
}

void Do_Save(int argc, const char *argv[])
{
    if (argc > 0) {
	delete File;
	File = new char[strlen(argv[0])+1];
	strcpy(File, argv[0]);
    }
    if (!File) {
	fputs("No file specified\n", stderr);
	return;
    }
    MyImage.Export(File);
}

void Do_View(int, const char **)
{
    char command[32];
    if (NoImage())
	return;

    const char *filename = TmpFileName();
    MyImage.Export(filename);
    sprintf(command, "xv %s &", filename);
    system(command);
}

void Do_Wavelet(int argc, const char *argv[])
{
    if (argc < 1) {
	fputs("Invalid arguments\n", stderr);
	return;
    }
    if (argc >= 2) {
	u_int np = atoi(argv[0]);
	u_int nd = atoi(argv[1]);
	delete MyWavelet;
	MyWavelet = Wavelet::CreateCDF(np, nd);
    } else {
	u_int n = atoi(argv[0]);
	switch (n) {
	    case 1:
		delete MyWavelet;
		MyWavelet = new Wavelet_CRF_13_7;
		break;
	    case 2:
		delete MyWavelet;
		MyWavelet = new Wavelet_SWE_13_7;
		break;
	    default:
		fprintf(stderr, "Unknown wavelet type %d\n", n);
		return;
	}
    }
}

void Do_FStep_CR(int, const char **)
{
    for (u_int ch = 0; ch < MyImage.GetChannels(); ch++) {
	Channel *ch2 = MyImage[ch]->PushFwtStepCR(*MyWavelet);
	if (ch2 && ch2 != MyImage[ch]) {
	    delete MyImage[ch];
	    MyImage[ch] = ch2;
	}
    }
}

void Do_FStep_C(int, const char **)
{
    for (u_int ch = 0; ch < MyImage.GetChannels(); ch++) {
	Channel *ch2 = MyImage[ch]->PushFwtStepC(*MyWavelet);
	if (ch2 && ch2 != MyImage[ch]) {
	    delete MyImage[ch];
	    MyImage[ch] = ch2;
	}
    }
}

void Do_FStep_R(int, const char **)
{
    for (u_int ch = 0; ch < MyImage.GetChannels(); ch++) {
	Channel *ch2 = MyImage[ch]->PushFwtStepR(*MyWavelet);
	if (ch2 && ch2 != MyImage[ch]) {
	    delete MyImage[ch];
	    MyImage[ch] = ch2;
	}
    }
}

const struct Command Commands_FStep[] = {
    { "c", Do_FStep_C },
    { "r", Do_FStep_R },
    { "cr", Do_FStep_CR },
    { "rc", Do_FStep_CR },
};

void Do_FStep(int argc, const char *argv[])
{
    if (NoImage() || NoWavelet())
	return;

    if (argc) {
	if (!ExecCommand(Commands_FStep, arraysize(Commands_FStep), argc, argv))
	    fputs("Invalid arguments\n", stderr);
    } else
	Do_FStep_CR(argc-1, argv+1);
}

void Do_BStep(int, const char **)
{
    if (NoImage())
	return;

    for (u_int ch = 0; ch < MyImage.GetChannels(); ch++) {
	Channel *ch2 = MyImage[ch]->PopFwtStep();
	if (ch2 && ch2 != MyImage[ch]) {
	    delete MyImage[ch];
	    MyImage[ch] = ch2;
	}
    }
}

void Do_IFwt(int, const char **)
{
    if (NoImage())
	return;

    MyImage.IFwt();
}

    //  Uniform random generator between 0 and 1

static double ranu(void)
{
  double x;
  x = (double)random()/INT_MAX;
  return x;
}

static int igauss = 0 ;


    //  Gaussian random generator

static double rang(double sdv)
{
    static double x1, x2;
    double s, num1, num2, v1, v2, tvar;
    struct timeval tv1;
    int seed;

    if (igauss != 2) {
	if (igauss == 0) {
	    gettimeofday(&tv1, (struct timezone *)NULL);
	    // `real' rand value
	    seed = (int) tv1.tv_sec & ((1L << 17) - 1);
	    // starts with a `real' rand value
	    srandom(seed);
	}
	do {
	    num1 = ranu();
	    num2 = ranu();
	    v1 = 2.0*num1-1.0;
	    v2 = 2.0*num2-1.0;
	    s = v1*v1+v2*v2;
	} while (s >= 1.0 || s == 0.0);
	tvar = sqrt(-2.0*log(s)/s);
	x1 = v1*tvar;
	x2 = v2*tvar;
	igauss = 2;
	return x1*sdv;
    } else {
	igauss = 1;
	return (x2*sdv);
    }
}

void Do_Noise(int argc, const char *argv[])
{
    if (NoImage())
	return;

    if (!argc) {
	fputs("No variance specified\n", stderr);
	return;
    }

    double variance = atof(argv[0]);
    double stdev = sqrt(variance);
    for (u_int ch = 0; ch < MyImage.GetChannels(); ch++)
	for (u_int r = 0; r < MyImage[ch]->GetRows(); r++)
	    for (u_int c = 0; c < MyImage[ch]->GetCols(); c++)
		(*MyImage[ch])(c, r) += (s16)(rang(stdev)+0.5);
}

void Do_Denoise(int, const char **)
{
    if (NoImage())
	return;

    for (u_int ch = 0; ch < MyImage.GetChannels(); ch++) {
	Channel *channel = MyImage[ch];
	while (channel && channel->IsLifted()) {
	    LChannel *lch = (LChannel *)channel;
	    for (u_int i = SubBand_LH; i <= SubBand_HH; i++) {
		NTChannel *ntch = (NTChannel *)(*lch)[(SubBand)i];
		if (ntch && ntch->GetCols()*ntch->GetRows() >= 1000) {
		    u_int threshold = ntch->OptimalGCVThreshold();
		    ntch->ThresholdSoft(threshold);
		}
		channel = (*lch)[SubBand_LL];
	    }
	}
    }
}

void Do_Backup(int, const char **)
{
    if (NoImage())
	return;
    if (Backup) {
	delete Backup;
	Backup = NULL;
    }
    Backup = MyImage.Clone();
}

void Do_Psnr(int, const char **)
{
    if (NoImage() || NoBackup())
	return;
    if (MyImage.GetChannels() != Backup->GetChannels()) {
	fputs("The current image must have the same number of channels as the "
	      "backup image\n", stderr);
	return;
    }

    for (u_int ch = 0; ch < MyImage.GetChannels(); ch++)
	printf("Channel %d: %f dB\n", ch, MyImage[ch]->Psnr(*(*Backup)[ch]));
}

void Do_Threshold(int argc, const char *argv[])
{
    if (NoImage())
	return;
    if (!argc) {
	fputs("No threshold specified\n", stderr);
	return;
    }

    double threshold = atof(argv[0]);
    u64 count = 0;
    u64 total = 0;
    for (u_int ch = 0; ch < MyImage.GetChannels(); ch++) {
	count += MyImage[ch]->Threshold(threshold);
	total += MyImage.GetCols(ch)*MyImage.GetRows(ch);
    }
    printf("Zeroed %llu coefficients (out of %llu)\n", count, total);
}

void Do_Scale(int argc, const char *argv[])
{
    if (NoImage() || NoWavelet())
	return;
    if (!argc) {
	fputs("No scale factor specified\n", stderr);
	return;
    }

    double scale = fabs(atof(argv[0]));
    MyImage.Scale(scale, *MyWavelet);
}

void Do_Histogram(int argc, const char *argv[])
{
    if (NoImage())
	return;

    if (!argc) {
	fputs("No level specified\n", stderr);
	return;
    }
    u_int level = atoi(argv[0]);
    argc--;
    argv++;

    if (!argc) {
	fputs("No subband specified\n", stderr);
	return;
    }
    u_int subband = atoi(argv[0]);
    if (subband > SubBand_HH) {
	fputs("Illegal subband\n", stderr);
	return;
    }
    argc--;
    argv++;

    u_int ch = 0;
    if (argc) {
	ch = atoi(argv[0]);
	if (ch > MyImage.GetChannels()) {
	    fputs("Illegal channel\n", stderr);
	    return;
	}
    }
    Channel *channel = MyImage[ch];
    if (level == 0) {
	if (subband != SubBand_LL) {
	    fputs("Subband must be 0 for this level\n", stderr);
	    return;
	}
    } else {
	u_int l2 = level;
	while (l2 > 1) {
	    if (!channel || !channel->IsLifted()) {
		fputs("Illegal level\n", stderr);
		return;
	    }
	    LChannel *lch = (LChannel *)channel;
	    channel = (*lch)[SubBand_LL];
	    l2--;
	}
	if (!channel || !channel->IsLifted()) {
	    fputs("Illegal level\n", stderr);
	    return;
	}
	LChannel *lch = (LChannel *)channel;
	channel = (*lch)[(SubBand)subband];
    }
    if (!channel || channel->IsLifted()) {
	fputs("Illegal level\n", stderr);
	return;
    }
    NTChannel *ntchan = (NTChannel *)channel;
    PixType min, max;
    ntchan->GetMinMax(min, max);
    u64 *histogram = ntchan->Histogram(min, max);
    const char *plotdata = TmpFileName();
    Stream stream;
    stream.Open(plotdata, "w");
    for (int i = min; i <= max; i++)
#ifdef __linux__
	stream.Printf("%d\t%Ld\n", i, histogram[i-min]);
#else // no %Ld
	stream.Printf("%d\t%ld\n", i, (long)histogram[i-min]);
#endif // no %Ld
    stream.Close();
    delete [] histogram;
    const char *plotscript = TmpFileName();
    stream.Open(plotscript, "w");
    stream.Printf("plot [%d:%d]\"%s\" title \"Level %d Subband %d [%d:%d]\" "
		  "with impulses\n",
		  min, max, plotdata, level, subband, min, max);
    stream.Puts("pause -1\n");
    stream.Close();
    char command[160];
    sprintf(command, "xterm -iconic -ut -T gnuplot -e gnuplot -name \"Level "
		     "%d Subband %d [%d:%d]\" %s &",
		     level, subband, min, max, plotscript);
    system(command);
}

void Do_Entropy(int, const char **)
{
    if (NoImage())
	return;
    for (u_int ch = 0; ch < MyImage.GetChannels(); ch++)
	printf("Channel %d: %f bits/pixel\n", ch, MyImage[ch]->Entropy());
}

static bool is_yuv = false;

void Do_YUV(int, const char **)
{
    if (NoImage())
	return;

    if (is_yuv)
	MyImage.Convert(IT_RGB, IT_YUVr);
    else
	MyImage.Convert(IT_YUVr, IT_RGB);
    is_yuv = !is_yuv;
}

const struct Command Commands[] = {
    { "help", Do_Help },
    { "?", Do_Help },
    { "quit", Do_Quit },
    { "exit", Do_Quit },
    { "x", Do_Quit },
    { "load", Do_Load },
    { "save", Do_Save },
    { "view", Do_View },
    { "wavelet", Do_Wavelet },
    { "fstep", Do_FStep },
    { "bstep", Do_BStep },
    { "ifwt", Do_IFwt },
    { "noise", Do_Noise },
    { "denoise", Do_Denoise },
    { "backup", Do_Backup },
    { "psnr", Do_Psnr },
    { "threshold", Do_Threshold },
    { "scale", Do_Scale },
    { "histogram", Do_Histogram },
    { "entropy", Do_Entropy },
    { "yuv", Do_YUV },
};


    /*
     *  Parse the Command Line
     */

static void ParseCommandLine(char *line)
{
    int argc;
    const char **argv;

    CreateArgs(line, &argc, &argv);
    if (argc && !ExecCommand(Commands, arraysize(Commands), argc, argv))
	fputs("Unknown command\n", stderr);
    DeleteArgs(argc, argv);
}


int main(void)
{
    char *line;

    fputs("Please enter your commands (`help' for help)\n\n", stderr);
    do {
	fprintf(stderr, "demo> ");
	line = ReadCommandLine();
	ParseCommandLine(line);
    } while (!ExitFlag);
    CleanUp();
    exit(0);
}
