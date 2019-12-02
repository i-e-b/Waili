#
#	Root Makefile
#
#   $Id: Makefile,v 4.2.2.1.2.1 1999/07/20 16:15:29 geert Exp $
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

SUBDIRS =	include lib test
ifeq (man,$(wildcard man))
SUBDIRS +=	man
endif

All:		Rules.config .depend
		set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i; done

Rules.config:
		$(MAKE) -C config

clean:
		rm -f .depend
		set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean; done

distclean:	clean
		rm -f Rules.config

config:		dummy
		rm -f Rules.config
		$(MAKE) Rules.config

.depend:
		set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i depend; done
		touch .depend

depend:
		rm -f .depend
		$(MAKE) .depend

dummy:
