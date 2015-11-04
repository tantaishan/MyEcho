NSIS Portable
===============================
Copyright 2004-2006 John T. Haller
Copyright 2007-2007 Zach Hudock
Copyright 2008-2009 Zach Thibeau

Website: http://PortableApps.com/NSISPortable

This software is OSI Certified Open Source Software.
OSI Certified is a certification mark of the Open Source Initiative.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.


ABOUT NSIS PORTABLE
======================
The NSIS Portable Launcher allows you to run NSIS from a removable drive whose letter changes as you move it to another computer.  The file archiver and the settings can be entirely self-contained on the drive and then used on any Windows computer.


LICENSE
=======
This code is released under the GPL.  Within the NSISPortableSource directory you will find the code (NSISPortable.nsi) as well as the full GPL license (License.txt).  If you use the launcher or code in your own product, please give proper and prominent attribution.


INSTALLATION / DIRECTORY STRUCTURE
==================================
By default, the program expects one of these directory structures:

-\ <--- Directory with NSISPortable.exe
	+\App\
	    +\NSIS\
	+\Data\
		+\settings\


It can be used in other directory configurations by including the NSISPortable.ini file in the same directory as NSISPortable.exe and configuring it as details in the INI file section below.  The INI file may also be placed in a subdirectory of the directory containing NSISPortable.exe called NSISPortable or 2 directories deep in PortableApps\NSISPortable or Data\NSISPortable.  All paths in the INI should remain relative to the EXE and not the INI.


NSISPortable.INI CONFIGURATION
=================================
The NSIS Portable Launcher will look for an ini file called NSISPortable.ini within its directory (see the paragraph above in the Installation/Directory Structure section).  If you are happy with the default options, it is not necessary, though.  There is an example INI included with this package to get you started.  The INI file is formatted as follows:

[NSISPortable]
NSISDirectory=App\NSIS
SettingsDirectory=Data\settings
NSISExecutable=NSIS.exe
AdditionalParameters=


The NSISDirectory and SettingsDirectory entries should be set to the *relative* path to the directories containing NSIS.exe and your settings from the current directory.  All must be a subdirectory (or multiple subdirectories) of the directory containing NSISPortable.exe.  The default entries for these are described in the installation section above.

The SettingsFile entry is the name of your NSIS settings file within the SettingsDirectory.

The NSISExecutable entry allows you to give an alternate filename for the NSIS executable.

The AdditionalParameters entry allows you to specify additional parameters to be passed to NSIS on the command line.


PROGRAM HISTORY / ABOUT THE AUTHORS
===================================
This launcher is loosely based on the Firefox Portable launcher, which contains methods suggested by mai9 and tracon of the mozillaZine.org forums.