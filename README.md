Simple CEF( Chromium Embedded Framework ) DLL

How to generate 

If you want you can go directly to the step 4.

1-Download cef_binary_3.1650.1461_windows32.7z from https://cefbuilds.com/. Is a revision of the trunk from 2013-10-16 for Windows 32 bits.
2-Extract and open the solution cefClient2010.sln with Visual Studio C++ 2010 Express.
3-Build the project libcef_dll_wrapper. It will generate libcef_dll_wrapper.lib that is included in this repository.
4-Download or clone this repository.
5-Build the solution SimpleCEFDLL.sln. It will generate SimpleCEFDLL.dll and Test_SimpleCEFDLL.dll that shows how to use the DLL.
6-You must copy with the DLL:
	-Directory locales
	-cef.pak
	-devtools_resources.pak
	-The rest of DLLs.

Linker options necessaries to build de DLL

1-/NODEFAULTLIB:LIBCMTD
2-/FORCE:MULTIPLE
