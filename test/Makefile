#
#	Makefile for test
#
#   $Id: Makefile,v 4.2.2.1.2.1 1999/07/20 16:16:19 geert Exp $
# 
#   Copyright (C) 1996-1999 Department of Computer Science, K.U.Leuven, Belgium
# 
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
# 
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
# 
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

include	../Rules.config


ALL =		Example Demo Proof Rangecheck Test Histogram

All:		$(ALL)

Example:	Example.o $(DEPLIBS)
		$(CXX) -o Example Example.o $(LFLAGS)

Demo:		Demo.o $(DEPLIBS)
		$(CXX) -o Demo Demo.o $(LFLAGS)

Proof:		Proof.o $(DEPLIBS)
		$(CXX) -o Proof Proof.o $(LFLAGS)

Test:		Test.o $(DEPLIBS)
		$(CXX) -o Test Test.o $(LFLAGS)

Rangecheck:	Rangecheck.o $(DEPLIBS)
		$(CXX) -o Rangecheck Rangecheck.o $(LFLAGS)

Histogram:	Histogram.o $(DEPLIBS)
		$(CXX) -o Histogram Histogram.o $(LFLAGS)


depend:
		rm -f .depend
		$(CXX) $(CFLAGS) -M -E *.C > .depend

clean:
		rm -f .depend *.o $(ALL) result.pgm out.pgm out.ppm reco.pgm \
		reco.ppm


include ../Rules.make
