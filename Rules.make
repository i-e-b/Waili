#
#	Default rules
#
#   $Id: Rules.make,v 4.0.4.1 1999/07/20 16:15:29 geert Exp $
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

%.o:		%.C
		$(CXX) $(CFLAGS) -c $< -o $@


ifeq (.depend,$(wildcard .depend))
include .depend
endif
