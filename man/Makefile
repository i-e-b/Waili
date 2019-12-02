#
#	Makefile for LaTeX2e documents
#
#   $Id: Makefile,v 4.6.2.1.2.1 1999/07/20 16:16:16 geert Exp $
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

ifeq (Rules.LaTeX.config,$(wildcard Rules.LaTeX.config))
include Rules.LaTeX.config
endif

# -----------------------------------------------------------------------------
# Document and figure definitions and dependencies
# -----------------------------------------------------------------------------

DOC =		Manual

MANPAGES =	Blit.tex \
		Channel.tex \
		Color.tex \
		ColorSpace.tex \
		Compiler.tex \
		Image.tex \
		Lifting.tex \
		Stream.tex \
		Timer.tex \
		Types.tex \
		Util.tex \
		Wavelet.tex

TEXSRCS =	Manual.tex \
		Macros.tex \
		Title.tex \
		Features.tex \
		Design.tex \
		Overview.tex \
		Man.tex $(MANPAGES) \
		Depend.tex \
		Install.tex \
		Demo.tex \
		Credits.tex

TEXSTYLES =	mymanpage.sty

GENFIGS =	WAILI.eps Channel_dep.eps Lifting_dep.eps Wavelet_dep.eps \
		Includes.eps Decomposition.eps

FIGS =		$(GENFIGS)

# -----------------------------------------------------------------------------
# Elementary build rules
# -----------------------------------------------------------------------------

include Rules.LaTeX.build
depend:

