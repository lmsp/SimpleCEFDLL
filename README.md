# Simple CEF( Chromium Embedded Framework ) DLL

This DLL can be used to embed an instance of Chromium browser in a window of your application. Is written in C++ in Win32. Has been tested in Windows XP( 32 bits ) and in Windows 8.1( 64 bits ). Has been embedded in two applications written with Visual Studio C++ 2010 and C++ Builder 2009.

## How to use

The DLL exports four functions:

1. `void SimpleCEFDLL_Initialize( HINSTANCE hInstance );`
2. `void SimpleCEFDLL_CreateBrowser( HWND hWnd, const wchar_t* url );`
3. `void SimpleCEFDLL_ResizeBrowser( HWND hWnd );`
4. `void SimpleCEFDLL_Shutdown( bool terminate );`

### How to use with Visual Studio C++ 2010

You can see the example application Test_SimpleCEFDLL in this repository.

### How to use with C++ Builder 2009

1. You first must typedef de prototypes of your functions:

```C++
typedef void (*SimpleCEFDLL_Initialize)(HINSTANCE);
typedef void (*SimpleCEFDLL_CreateBrowser)(HWND,const wchar_t*);
typedef void (*SimpleCEFDLL_ResizeBrowser)(HWND);
typedef void (*SimpleCEFDLL_Shutdown)(bool);
```

2. In the WinMain entry you can write:

```C++
Application->Initialize();

HINSTANCE hDLL = LoadLibrary("SimpleCEFDLL.dll");
SimpleCEFDLL_Initialize fnInitialize = (SimpleCEFDLL_Initialize)GetProcAddress((HMODULE)hDLL, 
																				"SimpleCEFDLL_Initialize");
SimpleCEFDLL_Shutdown fnShutdown = (SimpleCEFDLL_Shutdown)GetProcAddress((HMODULE)hDLL, "SimpleCEFDLL_Shutdown");

(*fnInitialize)(GetModuleHandle(NULL));

Application->MainFormOnTaskBar = true;
Application->CreateForm(__classid(TForm1), &Form1);

Application->Run();

(*fnShutdown)(true);
```

3. Now in the TFormBrowser that you want to show the browser embedded:

```C++
void __fastcall TFormBrowser::FormCreate(TObject *Sender)
{
	HINSTANCE hDLL = LoadLibrary("SimpleCEFDLL.dll");
	SimpleCEFDLL_CreateBrowser fnCreateBrowser = (SimpleCEFDLL_CreateBrowser)GetProcAddress((HMODULE)hDLL, 
																				"SimpleCEFDLL_CreateBrowser");
	
	//This pointer to function can be a member of TFormBrowser
	m_fnResizeBrowser = (SimpleCEFDLL_ResizeBrowser)GetProcAddress((HMODULE)hDLL, "SimpleCEFDLL_ResizeBrowser");
	
	String url( "http://www.google.es" );
	(*fnCreateBrowser)(Handle, url.c_str() );
}

void __fastcall TFormBrowser::FormResize(TObject *Sender)
{
	(*m_fnResizeBrowser)(Handle);
}
```

## How to build de DLL

If you want you can go directly to the step 4.

1. Download cef_binary_3.1650.1461_windows32.7z from https://cefbuilds.com/. Is a revision of the trunk from 2013-10-16 for Windows 32 bits.
2. Extract and open the solution cefClient2010.sln with Visual Studio C++ 2010 Express.
3. Build the project libcef_dll_wrapper. It will generate libcef_dll_wrapper.lib that is included in this repository.
4. Download or clone this repository.
5. Build the solution SimpleCEFDLL.sln. It will generate SimpleCEFDLL.dll and Test_SimpleCEFDLL.dll that shows how to use the DLL.
6. You must copy with the DLL:
	* Directory locales
	* cef.pak
	* devtools_resources.pak
	* The rest of DLLs.

## Linker options necessaries to build de DLL

1. /NODEFAULTLIB:LIBCMTD
2. /FORCE:MULTIPLE
