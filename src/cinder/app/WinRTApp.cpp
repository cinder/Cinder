#include "cinder/app/WinRTApp.h"
#include "cinder/app/AppBasic.h"
#include "cinder/app/AppImplWinRTBasic.h"
#include <wrl/client.h>
#include <agile.h>

using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Popups;
using namespace Windows::UI::Input;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;


WinRTApp::WinRTApp() :
	m_windowClosed(false),
	m_windowVisible(true)
{

}

void WinRTApp::Initialize(CoreApplicationView^ applicationView)
{
	applicationView->Activated +=
        ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>(this, &WinRTApp::OnActivated);

	CoreApplication::Suspending +=
        ref new EventHandler<SuspendingEventArgs^>(this, &WinRTApp::OnSuspending);

	CoreApplication::Resuming +=
        ref new EventHandler<Platform::Object^>(this, &WinRTApp::OnResuming);

}

void WinRTApp::SetWindow(CoreWindow^ window)
{
	window->SizeChanged += 
        ref new TypedEventHandler<CoreWindow^, WindowSizeChangedEventArgs^>(this, &WinRTApp::OnWindowSizeChanged);

	window->VisibilityChanged +=
		ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &WinRTApp::OnVisibilityChanged);

	window->Closed += 
        ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>(this, &WinRTApp::OnWindowClosed);

	window->PointerCursor = ref new CoreCursor(CoreCursorType::Arrow, 0);

	window->PointerReleased +=
		ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &WinRTApp::OnPointerReleased);

	window->PointerPressed +=
		ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &WinRTApp::OnPointerPressed);

	window->PointerMoved +=
		ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &WinRTApp::OnPointerMoved);
	
	window->KeyDown +=
        ref new TypedEventHandler<CoreWindow^, KeyEventArgs^>(this, &WinRTApp::OnKeyDown);

    window->KeyUp +=
        ref new TypedEventHandler<CoreWindow^, KeyEventArgs^>(this, &WinRTApp::OnKeyUp);

#if 0
	window->CharacterReceived += 
		ref new TypedEventHandler<CoreWindow^, CharacterReceivedEventArgs^>(this, &WinRTApp::OnCharacterReceived);

#endif 
}




void WinRTApp::OnCharacterReceived(CoreWindow^ sender, CharacterReceivedEventArgs^ args)
{
	unsigned int code = args->KeyCode;
}

void WinRTApp::OnKeyDown(CoreWindow^ sender, KeyEventArgs^ args)
{
	mApp->handleKeyDown(args);

#if 0
	Windows::System::VirtualKey Key;
	Key = args->VirtualKey;

	// Figure out the command from the keyboard.
	if (Key == VirtualKey::D) 
	{
		MessageDialog^ dlg = ref new MessageDialog("Are you sure you want to do this?", "Confirm");
		dlg->ShowAsync();
	}  
#endif // 0

}

void WinRTApp::OnKeyUp(CoreWindow^ sender, KeyEventArgs^ args)
{
	mApp->handleKeyUp(args);
}

void WinRTApp::Load(Platform::String^ entryPoint)
{
}

void WinRTApp::Run()
{
	// call Cinder runReady and pass our CoreWindow to it.
	cinder::app::AppBasic* app = cinder::app::AppBasic::get();
	mApp = app->getImpl();
	mApp->runReady(CoreWindow::GetForCurrentThread());

	// Cinder now controls the app
#if 0
	while (!m_windowClosed)
	{
		if (m_windowVisible)
		{
			//timer->Update();
			CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
			//m_renderer->Update(timer->Total, timer->Delta);
			//m_renderer->Render();
			//m_renderer->Present(); // This call is synchronized to the display frame rate.
		}
		else
		{
			CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending);
		}
	}
#endif
}

void WinRTApp::Uninitialize()
{
}

void WinRTApp::OnWindowSizeChanged(CoreWindow^ sender, WindowSizeChangedEventArgs^ args)
{
	mApp->UpdateForWindowSizeChange();
}

void WinRTApp::OnVisibilityChanged(CoreWindow^ sender, VisibilityChangedEventArgs^ args)
{
	mApp->setVisible(args->Visible);
	m_windowVisible = args->Visible;
}

void WinRTApp::OnWindowClosed(CoreWindow^ sender, CoreWindowEventArgs^ args)
{
	m_windowClosed = true;
}




void WinRTApp::OnPointerPressed(CoreWindow^ sender, PointerEventArgs^ args)
{
	mApp->handlePointerDown(args);
}

void WinRTApp::OnPointerReleased(CoreWindow^ sender, PointerEventArgs^ args)
{
	mApp->handlePointerUp(args);
}

void WinRTApp::OnPointerMoved(CoreWindow^ sender, PointerEventArgs^ args)
{
	mApp->handlePointerMoved(args);

}

void WinRTApp::OnActivated(CoreApplicationView^ applicationView, IActivatedEventArgs^ args)
{
	CoreWindow::GetForCurrentThread()->Activate();
}

void WinRTApp::OnSuspending(Platform::Object^ sender, SuspendingEventArgs^ args)
{
	// Save app state asynchronously after requesting a deferral. Holding a deferral
	// indicates that the application is busy performing suspending operations. Be
	// aware that a deferral may not be held indefinitely. After about five seconds,
	// the app will be forced to exit.
	//SuspendingDeferral^ deferral = args->SuspendingOperation->GetDeferral();
}
 
void WinRTApp::OnResuming(Platform::Object^ sender, Platform::Object^ args)
{
	// Restore any data or state that was unloaded on suspend. By default, data
	// and state are persisted when resuming from suspend. Note that this event
	// does not occur if the app was previously terminated.
}

IFrameworkView^ Direct3DApplicationSource::CreateView()
{
    return ref new WinRTApp();
}
