/* SimpleCEFDLL.cpp

Author: Luis Miguel Santamaría Pérez

The MIT License (MIT)

Copyright (c) 2016 Luis Miguel Santamaría Pérez

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
Status API Training Shop Blog About Pricing
© 2016 GitHub, Inc. Terms Privacy Security 

*/

#include "stdafx.h"

#include "SimpleCEFDLL.h"

#include <map>

#include "include/cef_app.h"
#include "include/cef_client.h"
#include "include/cef_browser.h"

//We create a class that inherits off of CefClient is necessary to derive
//from CefLifeSpanHandler to obtein a reference to the browser
class zSimpleCefHandler :	public CefClient,
							public CefLifeSpanHandler
{
public:
	zSimpleCefHandler()  {}
    virtual ~zSimpleCefHandler() {}
    
	//Retunrs a reference to embedded browser, we need it in the
	//SimpleCEFDLL_ResizeBrowser function
	CefRefPtr<CefBrowser> GetBrowser() { return m_Browser; }

	//An override of the function in class CefClient
	CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler(){ return this; }

	//An override of the function in CefLifeSpanHandler, we set here
	// the member m_Browser
	void OnAfterCreated(CefRefPtr<CefBrowser> browser) {
		m_Browser = browser;
		CefLifeSpanHandler::OnAfterCreated( browser );
	}

protected:

	CefRefPtr<CefBrowser> m_Browser;

	IMPLEMENT_REFCOUNTING(zSimpleCefHandler);
};

// We create a dummy class that inherits off of CefApp, it only 
// saves a reference to every zSimpleCefHandler created
class zSimpleCefApp : public CefApp
{
public:
	zSimpleCefApp () {}
    virtual ~zSimpleCefApp (){}

	bool GetCEFHandler( HWND hWnd, CefRefPtr<zSimpleCefHandler>& simpleCEFHandler ) 
	{ 
		auto search = m_MapCEFHandlers.find( hWnd );
		if( search != m_MapCEFHandlers.end() ) 
		{
			simpleCEFHandler = search->second; 
			return true;
		}
		return false;
	}
	void SetCEFHandler( HWND hWnd, CefRefPtr<zSimpleCefHandler> simpleCEFHandler ) 
	{  
		m_MapCEFHandlers.insert( std::pair< HWND, CefRefPtr<zSimpleCefHandler> >( hWnd, simpleCEFHandler ) ); 
	}

private:
	std::map< HWND, CefRefPtr<zSimpleCefHandler> > m_MapCEFHandlers;

	IMPLEMENT_REFCOUNTING (zSimpleCefApp);
};


//Global reference to CEF application
CefRefPtr<zSimpleCefApp> g_App;

//Initialize the CEF library hInstance will be GetModuleHandle(NULL)
void SimpleCEFDLL_Initialize( HINSTANCE hInstance )
{
	g_App = new zSimpleCefApp;

	CefMainArgs main_args( hInstance );
	CefExecuteProcess( main_args, g_App.get() );

	CefSettings settings;

	//This two settings are mandatory for work in Windows XP
	settings.multi_threaded_message_loop = true;
	settings.single_process = true;

	CefInitialize( main_args, settings, g_App.get() );
}

//Create the browser in our previously created window with hWnd handle
//and the url to load. This function must be called in WM_CREATE
void SimpleCEFDLL_CreateBrowser( HWND hWnd, const wchar_t* url )
{
	CefRefPtr<zSimpleCefHandler> simpleCefHandler = new zSimpleCefHandler;

	g_App->SetCEFHandler( hWnd, simpleCefHandler );

	//We fill the info to adapt our window to embedd the browser
	CefWindowInfo info;

	//Get the window rect
	RECT rect;
	GetClientRect( hWnd, &rect);
	
	//We will create the browser as a child of our window
	info.SetAsChild( hWnd,rect);

	CefBrowserSettings settingsBrowser;
    CefBrowserHost::CreateBrowser(info, simpleCefHandler.get(), CefString( url ), settingsBrowser, NULL);
}

//We must call in WM_SIZE event, hWnd will be the our window handle
void SimpleCEFDLL_ResizeBrowser( HWND hWnd )
{
	//Get the CEF handle
	CefRefPtr<zSimpleCefHandler> simpleCefHandler; 
	if( !g_App->GetCEFHandler( hWnd, simpleCefHandler ) )
		return;

	if( !simpleCefHandler.get() )
		return;

	if( !simpleCefHandler->GetBrowser().get() )
		return;

	if( !simpleCefHandler->GetBrowser()->GetHost().get() )
		return;
	
	CefWindowHandle hWndBrowser = simpleCefHandler->GetBrowser()->GetHost()->GetWindowHandle();
	if( hWndBrowser )
	{
		//Get the size of our window
		RECT rect;
		GetClientRect( hWnd, &rect );
		
		//Set the size to the browser
		HDWP hdwp = BeginDeferWindowPos (1);
		hdwp = DeferWindowPos (hdwp, hWndBrowser, NULL,rect.left, 
			rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER);
		EndDeferWindowPos (hdwp);
	}
}

//Shutdown CEF. It doesn't work. You can optionally terminate the process
void SimpleCEFDLL_Shutdown( bool terminate )
{
	//SetErrorMode(SEM_NOGPFAULTERRORBOX);
	//CefShutdown();

	if( terminate )
		TerminateProcess( GetCurrentProcess(), 0 );
}
