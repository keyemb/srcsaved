#
# iwidgets.tcl
# ----------------------------------------------------------------------
# Invoked automatically by [incr Tk] upon startup to initialize
# the [incr Widgets] package.
# ----------------------------------------------------------------------
#  AUTHOR: Mark L. Ulferts               EMAIL: mulferts@spd.dsccc.com
#
#  @(#) $Id: iwidgets.tcl 144 2003-02-05 10:56:26Z mdejong $
# ----------------------------------------------------------------------
#                Copyright (c) 1995  Mark L. Ulferts
# ======================================================================
# See the file "license.terms" for information on usage and
# redistribution of this file, and for a DISCLAIMER OF ALL WARRANTIES.

package require Tcl 8.0
package require Tk 8.0
package require Itcl 3.0
package require Itk 3.0

namespace eval ::iwidgets {
    namespace export *

    variable library [file dirname [info script]]
    variable version 3.0.0
}

lappend auto_path [file join $iwidgets::library scripts]
package provide Iwidgets $iwidgets::version

# For now we need to import all of the itcl functions into the global
# namespace. This should be removed once iwidgets are upgraded to use the
# itcl:: names directly.

namespace import -force itcl::*