Chromium Embedded Framework (CEF) Binary Distribution
-------------------------------------------------------------------------------

Date:             March 04, 2013

CEF Version:      1.1364.1123
CEF URL:          https://chromiumembedded.googlecode.com/svn/branches/1364/cef1@1123

Chromium Verison: 25.0.1364.152
Chromium URL:     http://src.chromium.org/svn/branches/1364/src@185281


This distribution contains all components necessary to build and distribute an
application using CEF. Please see the LICENSING section of this document for
licensing terms and conditions.


CONTENTS
--------

cefclient   Contains the cefclient sample application configured to build
            using the files in this distribution.

Debug       Contains libcef.dll and other components required to run the debug
            version of CEF-based applications. Also acts as the build target for
            the Debug build of cefclient.

docs        Contains C++ API documentation generated from the CEF header files.

include     Contains all required CEF and NPAPI-related header files.  Read
            the include/internal/npapi/README-TRANSFER.txt file for more
            information about the NPAPI-related header files.

lib         Contains Debug and Release versions of the libcef.lib library file
            that all CEF-based applications must link against.

libcef_dll  Contains the source code for the libcef_dll_wrapper static library
            that all applications using the CEF C++ API must link against.

Release     Contains libcef.dll and other components required to run the release
            version of CEF-based applications. Also acts as the build target for
            the Release build of cefclient.


USAGE
-----

Visual Studio 2012 and Visual Studio 2010:
  Open the cefclient2010.sln solution in Visual Studio and build.

Visual Studio 2008:
  Open the cefclient2008.sln solution in Visual Studio and build.

Visual Studio 2005:
  1. Open the cefclient.vcproj and libcef_dll_wrapper.vcproj files in a text
     editor. Change Version="9.00" to Version="8.00".
  2. Open the cefclient2005.sln file in a text editor. Change "Version 9.00" to
     "Version 8.00".
  3. Open the cefclient2005.sln solution in Visual Studio and build.

Please visit the CEF Website for additional usage information.

http://code.google.com/p/chromiumembedded


REDISTRIBUTION
--------------

This binary distribution contains the below components. Components listed under
the "required" section must be redistributed with all applications using CEF.
Components listed under the "optional" section may be excluded if the related
features will not be used.

Required components:

* CEF core library
    libcef.dll

* Unicode support
    icudt.dll

Optional components:

* Localized resources
    locales/
  Note: Contains localized strings for WebKit UI controls. A .pak file is loaded
  from this folder based on the CefSettings.locale value. Only configured
  locales need to be distributed. If no locale is configured the default locale
  of "en-US" will be used. Locale file loading can be disabled completely using
  CefSettings.pack_loading_disabled. The locales folder path can be customized
  using CefSettings.locales_dir_path.

* Other resources
    devtools_resources.pak
  Note: Contains WebKit image and inspector resources. Pack file loading can be
  disabled completely using CefSettings.pack_loading_disabled. The resources
  directory path can be customized using CefSettings.resources_dir_path.

* Angle and Direct3D support
    d3dcompiler_43.dll
    d3dx9_43.dll
    libEGL.dll
    libGLESv2.dll
  Note: Without these components the default ANGLE_IN_PROCESS graphics
  implementation for HTML5 accelerated content like 2D canvas, 3D CSS and
  WebGL will not function. To use the desktop GL graphics implementation which
  does not require these components (and does not work on all systems) set
  CefSettings.graphics_implementation to DESKTOP_IN_PROCESS.


LICENSING
---------

The CEF project is BSD licensed. Please read the LICENSE.txt file included with
this binary distribution for licensing terms and conditions. Other software
included in this distribution is provided under other licenses. Please visit the
below link for complete Chromium and third-party licensing information.

http://code.google.com/chromium/terms.html 
