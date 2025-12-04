#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CinderImGui.h"
#include <deque>
#include <sstream>
#include <iomanip>

using namespace ci;
using namespace ci::app;
using namespace std;

// Event record structure
struct EventRecord {
	string type;          // "MouseDown", "KeyDown", etc.
	string details;       // Main event details
	string extraInfo;     // Additional info (position, code, etc.)
	bool shift;
	bool ctrl;
	bool alt;
	bool meta;
	bool leftMouse;
	bool rightMouse;
	bool middleMouse;
	double timestamp;

	EventRecord() : shift(false), ctrl(false), alt(false), meta(false),
					leftMouse(false), rightMouse(false), middleMouse(false), timestamp(0.0) {}
};

class EventTestApp : public App {
public:
	void setup() override;
	void update() override;
	void draw() override;

	void fileDrop( FileDropEvent event ) override;
    void resize() override { console() << "Resized!" << getWindowSize() << std::endl; }

private:
	void addEvent( const EventRecord& record );
	void drawModifierBadge( const char* text, const ImVec4& color );
	void drawEventList();
	void drawTextInputPanel();
	string utf32ToUtf8( uint32_t codepoint );

	// Signal handlers for events (connected at high priority to observe all events)
	void handleKeyDown( KeyEvent& event );
	void handleKeyUp( KeyEvent& event );
	void handleKeyChar( KeyEvent& event );
	void handleMouseDown( MouseEvent& event );
	void handleMouseUp( MouseEvent& event );
	void handleMouseWheel( MouseEvent& event );
	void handleMouseMove( MouseEvent& event );
	void handleMouseDrag( MouseEvent& event );

	deque<EventRecord> mEvents;
	static const size_t MAX_EVENTS = 10000;

	// Signal connections for event monitoring
	signals::Connection mKeyDownConnection;
	signals::Connection mKeyUpConnection;
	signals::Connection mKeyCharConnection;
	signals::Connection mMouseDownConnection;
	signals::Connection mMouseUpConnection;
	signals::Connection mMouseWheelConnection;
	signals::Connection mMouseMoveConnection;
	signals::Connection mMouseDragConnection;

	// Filter options
	bool mShowMouse = true;
	bool mShowMouseMove = false;  // Mouse move generates too many events
	bool mShowKeyboard = true;
	bool mShowFileDrop = true;

	// UI state
	bool mAutoScroll = true;
	float mBadgeRounding = 4.0f;
	float mBadgePadding = 6.0f;

	// Text input testing
	char mTextBuffer[256] = "Type here to test Unicode...";
	char mMultilineBuffer[1024] = "Multi-line text input test...";
};

void EventTestApp::setup()
{
	ImGui::Initialize( ImGui::Options().window( getWindow() ) );  // ImGui at priority 1
	ImGui::GetStyle().ScaleAllSizes( getWindowContentScale() );
	ImGui::GetStyle().FontScaleMain = getWindowContentScale();

	// Remove window padding for full-screen docked look
	ImGui::GetStyle().WindowPadding = ImVec2( 8, 8 );
	ImGui::GetStyle().WindowBorderSize = 0.0f;

	// Connect event monitors at priority 2 (higher than ImGui's priority 1)
	// This lets us observe all events while still allowing ImGui to consume them
	mKeyDownConnection = getWindow()->getSignalKeyDown().connect( 2,
		[this]( KeyEvent& event ) { handleKeyDown( event ); } );
	mKeyUpConnection = getWindow()->getSignalKeyUp().connect( 2,
		[this]( KeyEvent& event ) { handleKeyUp( event ); } );
	mKeyCharConnection = getWindow()->getSignalKeyChar().connect( 2,
		[this]( KeyEvent& event ) { handleKeyChar( event ); } );

	mMouseDownConnection = getWindow()->getSignalMouseDown().connect( 2,
		[this]( MouseEvent& event ) { handleMouseDown( event ); } );
	mMouseUpConnection = getWindow()->getSignalMouseUp().connect( 2,
		[this]( MouseEvent& event ) { handleMouseUp( event ); } );
	mMouseWheelConnection = getWindow()->getSignalMouseWheel().connect( 2,
		[this]( MouseEvent& event ) { handleMouseWheel( event ); } );
	mMouseMoveConnection = getWindow()->getSignalMouseMove().connect( 2,
		[this]( MouseEvent& event ) { handleMouseMove( event ); } );
	mMouseDragConnection = getWindow()->getSignalMouseDrag().connect( 2,
		[this]( MouseEvent& event ) { handleMouseDrag( event ); } );

	// Start with a welcome message
	EventRecord welcome;
	welcome.type = "Info";
	welcome.details = "Ready - try keyboard, mouse, file drop";
	welcome.extraInfo = "";
	welcome.timestamp = getElapsedSeconds();
	mEvents.push_back( welcome );
}

void EventTestApp::update()
{
}

void EventTestApp::addEvent( const EventRecord& record )
{
	mEvents.push_back( record );
	if( mEvents.size() > MAX_EVENTS ) {
		mEvents.pop_front();
	}
}

string EventTestApp::utf32ToUtf8( uint32_t codepoint )
{
	string result;
	if( codepoint < 0x80 ) {
		result += static_cast<char>( codepoint );
	}
	else if( codepoint < 0x800 ) {
		result += static_cast<char>( 0xC0 | ( codepoint >> 6 ) );
		result += static_cast<char>( 0x80 | ( codepoint & 0x3F ) );
	}
	else if( codepoint < 0x10000 ) {
		result += static_cast<char>( 0xE0 | ( codepoint >> 12 ) );
		result += static_cast<char>( 0x80 | ( ( codepoint >> 6 ) & 0x3F ) );
		result += static_cast<char>( 0x80 | ( codepoint & 0x3F ) );
	}
	else if( codepoint < 0x110000 ) {
		result += static_cast<char>( 0xF0 | ( codepoint >> 18 ) );
		result += static_cast<char>( 0x80 | ( ( codepoint >> 12 ) & 0x3F ) );
		result += static_cast<char>( 0x80 | ( ( codepoint >> 6 ) & 0x3F ) );
		result += static_cast<char>( 0x80 | ( codepoint & 0x3F ) );
	}
	return result;
}

void EventTestApp::drawModifierBadge( const char* text, const ImVec4& color )
{
	// Measure text
	ImVec2 textSize = ImGui::CalcTextSize( text );
	ImVec2 badgeSize( textSize.x + mBadgePadding * 2, textSize.y + mBadgePadding );

	// Get cursor position
	ImVec2 cursorPos = ImGui::GetCursorScreenPos();

	// Draw rounded rectangle background
	ImDrawList* drawList = ImGui::GetWindowDrawList();
	ImU32 col = ImGui::GetColorU32( color );
	drawList->AddRectFilled( cursorPos,
							ImVec2( cursorPos.x + badgeSize.x, cursorPos.y + badgeSize.y ),
							col, mBadgeRounding );

	// Draw text
	drawList->AddText( ImVec2( cursorPos.x + mBadgePadding, cursorPos.y + mBadgePadding * 0.5f ),
					  ImGui::GetColorU32( ImVec4( 1, 1, 1, 1 ) ), text );

	// Advance cursor using Dummy (table-friendly)
	ImGui::Dummy( ImVec2( badgeSize.x, badgeSize.y ) );
	ImGui::SameLine();
}

void EventTestApp::drawEventList()
{
	float windowWidth = (float)getWindowWidth() * 0.7f;  // 70% of window width
	ImGui::SetNextWindowPos( ImVec2( 0, 0 ) );
	ImGui::SetNextWindowSize( ImVec2( windowWidth, (float)getWindowHeight() ) );

	ImGui::Begin( "Event Monitor", nullptr,
				 ImGuiWindowFlags_NoTitleBar |
				 ImGuiWindowFlags_NoResize |
				 ImGuiWindowFlags_NoMove |
				 ImGuiWindowFlags_NoCollapse |
				 ImGuiWindowFlags_NoBringToFrontOnFocus );

	ImGui::Text( "Event Monitor (%zu)", mEvents.size() );

	ImGui::Checkbox( "Mouse", &mShowMouse );
	ImGui::SameLine();
	ImGui::Checkbox( "Move", &mShowMouseMove );
	ImGui::SameLine();
	ImGui::Checkbox( "Keyboard", &mShowKeyboard );
	ImGui::SameLine();
	ImGui::Checkbox( "FileDrop", &mShowFileDrop );
	ImGui::SameLine();
	ImGui::Checkbox( "Auto", &mAutoScroll );

	ImGui::Separator();

	// Scrollable event list
	ImGui::BeginChild( "EventList", ImVec2( 0, 0 ), true );

	ImGuiTableFlags tableFlags =
		ImGuiTableFlags_RowBg |                 // Alternating row colors
		ImGuiTableFlags_BordersInnerV |         // Vertical borders between columns
		ImGuiTableFlags_PadOuterX |             // Padding on left/right edges
		ImGuiTableFlags_ScrollY |               // Enable vertical scrolling
		ImGuiTableFlags_SizingFixedFit;         // Fixed fit sizing

	if( ImGui::BeginTable( "EventTable", 4, tableFlags ) ) {
		const auto& style = ImGui::GetStyle();
		const float typeColWidth = ImGui::CalcTextSize( "MouseWheel" ).x + style.ItemSpacing.x * 2.0f;
		const float initiatorWidth = ImGui::CalcTextSize( "middle" ).x + style.ItemSpacing.x * 2.0f;
		const float coordWidth = ImGui::CalcTextSize( "(  999,  999)" ).x + style.ItemSpacing.x * 2.0f;

		ImGui::TableSetupColumn( "Type", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, typeColWidth );
		ImGui::TableSetupColumn( "Init/Key", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, initiatorWidth );
		ImGui::TableSetupColumn( "Pos/Code", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, coordWidth );
		ImGui::TableSetupColumn( "Mods", ImGuiTableColumnFlags_WidthStretch );

		int row = 0;
		for( auto it = mEvents.rbegin(); it != mEvents.rend(); ++it ) {
			const EventRecord& evt = *it;

			// Apply filters
			bool isMouseEvent = ( evt.type.find( "Mouse" ) != string::npos );
			bool isMouseMoveEvent = ( evt.type == "MouseMove" || evt.type == "MouseDrag" );
			bool isKeyboardEvent = ( evt.type.find( "Key" ) != string::npos );
			bool isFileDropEvent = ( evt.type == "FileDrop" );

			if( isMouseEvent && ! mShowMouse ) continue;
			if( isMouseMoveEvent && ! mShowMouseMove ) continue;
			if( isKeyboardEvent && ! mShowKeyboard ) continue;
			if( isFileDropEvent && ! mShowFileDrop ) continue;

			// Event type with color coding
			ImVec4 typeColor( 0.6f, 0.6f, 0.6f, 1.0f );
			if( isKeyboardEvent ) typeColor = ImVec4( 0.4f, 0.8f, 0.4f, 1.0f );
			else if( isMouseEvent ) typeColor = ImVec4( 0.4f, 0.6f, 1.0f, 1.0f );
			else if( isFileDropEvent ) typeColor = ImVec4( 1.0f, 0.8f, 0.4f, 1.0f );

			ImGui::TableNextRow();
			ImGui::PushID( row++ );

			// Column 0: Event type
			ImGui::TableSetColumnIndex( 0 );
			ImGui::AlignTextToFramePadding();
			ImGui::TextColored( typeColor, "%s", evt.type.c_str() );

			// Column 1: Initiator/Key
			ImGui::TableSetColumnIndex( 1 );
			ImGui::AlignTextToFramePadding();
			ImGui::TextUnformatted( evt.details.c_str() );

			// Column 2: Position/KeyCode
			ImGui::TableSetColumnIndex( 2 );
			ImGui::AlignTextToFramePadding();
			ImGui::TextUnformatted( evt.extraInfo.c_str() );

			// Column 3: Modifiers & Buttons
			ImGui::TableSetColumnIndex( 3 );
			ImGui::AlignTextToFramePadding();
			bool hasModifiers = evt.shift || evt.ctrl || evt.alt || evt.meta || evt.leftMouse || evt.rightMouse || evt.middleMouse;
			if( hasModifiers ) {
				if( evt.shift ) drawModifierBadge( "shift", ImVec4( 0.3f, 0.5f, 0.7f, 0.9f ) );
				if( evt.ctrl ) drawModifierBadge( "ctrl", ImVec4( 0.7f, 0.3f, 0.5f, 0.9f ) );
				if( evt.alt ) drawModifierBadge( "alt", ImVec4( 0.5f, 0.7f, 0.3f, 0.9f ) );
				if( evt.meta ) drawModifierBadge( "meta", ImVec4( 0.7f, 0.5f, 0.3f, 0.9f ) );
				if( evt.leftMouse ) drawModifierBadge( "L", ImVec4( 0.5f, 0.5f, 0.5f, 0.9f ) );
				if( evt.rightMouse ) drawModifierBadge( "R", ImVec4( 0.5f, 0.5f, 0.5f, 0.9f ) );
				if( evt.middleMouse ) drawModifierBadge( "M", ImVec4( 0.5f, 0.5f, 0.5f, 0.9f ) );
			}

			ImGui::PopID();
		}

		ImGui::EndTable();
	}

	// Auto-scroll to top AFTER drawing all content
	if( mAutoScroll ) {
		ImGui::SetScrollY( 0.0f );
	}

	ImGui::EndChild();

	ImGui::End();
}

void EventTestApp::drawTextInputPanel()
{
	float windowWidth = (float)getWindowWidth() * 0.3f;
	float leftPanelWidth = (float)getWindowWidth() * 0.7f;

	ImGui::SetNextWindowPos( ImVec2( leftPanelWidth, 0 ) );
	ImGui::SetNextWindowSize( ImVec2( windowWidth, (float)getWindowHeight() ) );

	ImGui::Begin( "Text Input Test", nullptr,
				 ImGuiWindowFlags_NoTitleBar |
				 ImGuiWindowFlags_NoResize |
				 ImGuiWindowFlags_NoMove |
				 ImGuiWindowFlags_NoCollapse );

	ImGui::Text( "Single-line input:" );
	ImGui::InputText( "##input1", mTextBuffer, sizeof(mTextBuffer) );
	ImGui::Spacing();

	ImGui::Text( "Multi-line input:" );
	ImGui::InputTextMultiline( "##input2", mMultilineBuffer, sizeof(mMultilineBuffer), ImVec2( -1, 200 ) );

	ImGui::End();
}

void EventTestApp::draw()
{
	gl::clear( Color( 0.1f, 0.1f, 0.12f ) );
	drawEventList();
	drawTextInputPanel();
}

void EventTestApp::handleMouseDown( MouseEvent& event )
{
	EventRecord rec;
	rec.type = "MouseDown";

	// Column 2: Initiator (L/M/R)
	rec.details = event.isLeft() ? "L" : event.isRight() ? "R" : event.isMiddle() ? "M" : "?";

	// Column 3: Formatted coordinates (5 chars wide each)
	stringstream ss;
	ss << "(" << std::setw(5) << event.getPos().x << "," << std::setw(5) << event.getPos().y << ")";
	rec.extraInfo = ss.str();

	rec.shift = event.isShiftDown();
	rec.ctrl = event.isControlDown();
	rec.alt = event.isAltDown();
	rec.meta = event.isMetaDown();
	rec.leftMouse = event.isLeftDown();
	rec.rightMouse = event.isRightDown();
	rec.middleMouse = event.isMiddleDown();
	rec.timestamp = getElapsedSeconds();

	addEvent( rec );
}

void EventTestApp::handleMouseUp( MouseEvent& event )
{
	EventRecord rec;
	rec.type = "MouseUp";

	rec.details = event.isLeft() ? "L" : event.isRight() ? "R" : event.isMiddle() ? "M" : "?";

	stringstream ss;
	ss << "(" << std::setw(5) << event.getPos().x << "," << std::setw(5) << event.getPos().y << ")";
	rec.extraInfo = ss.str();

	rec.shift = event.isShiftDown();
	rec.ctrl = event.isControlDown();
	rec.alt = event.isAltDown();
	rec.meta = event.isMetaDown();
	rec.leftMouse = event.isLeftDown();
	rec.rightMouse = event.isRightDown();
	rec.middleMouse = event.isMiddleDown();
	rec.timestamp = getElapsedSeconds();

	addEvent( rec );
}

void EventTestApp::handleMouseWheel( MouseEvent& event )
{
	EventRecord rec;
	rec.type = "MouseWheel";

	stringstream ss;
	ss << event.getWheelIncrement();
	rec.details = ss.str();

	stringstream ss2;
	ss2 << "(" << std::setw(5) << event.getPos().x << "," << std::setw(5) << event.getPos().y << ")";
	rec.extraInfo = ss2.str();

	rec.shift = event.isShiftDown();
	rec.ctrl = event.isControlDown();
	rec.alt = event.isAltDown();
	rec.meta = event.isMetaDown();
	rec.leftMouse = event.isLeftDown();
	rec.rightMouse = event.isRightDown();
	rec.middleMouse = event.isMiddleDown();
	rec.timestamp = getElapsedSeconds();

	addEvent( rec );
}

void EventTestApp::handleMouseMove( MouseEvent& event )
{
	EventRecord rec;
	rec.type = "MouseMove";

	rec.details = "";

	stringstream ss;
	ss << "(" << std::setw(5) << event.getPos().x << "," << std::setw(5) << event.getPos().y << ")";
	rec.extraInfo = ss.str();

	rec.shift = event.isShiftDown();
	rec.ctrl = event.isControlDown();
	rec.alt = event.isAltDown();
	rec.meta = event.isMetaDown();
	rec.leftMouse = event.isLeftDown();
	rec.rightMouse = event.isRightDown();
	rec.middleMouse = event.isMiddleDown();
	rec.timestamp = getElapsedSeconds();

	addEvent( rec );
}

void EventTestApp::handleMouseDrag( MouseEvent& event )
{
	EventRecord rec;
	rec.type = "MouseDrag";

	rec.details = event.isLeft() ? "L" : event.isRight() ? "R" : event.isMiddle() ? "M" : "?";

	stringstream ss;
	ss << "(" << std::setw(5) << event.getPos().x << "," << std::setw(5) << event.getPos().y << ")";
	rec.extraInfo = ss.str();

	rec.shift = event.isShiftDown();
	rec.ctrl = event.isControlDown();
	rec.alt = event.isAltDown();
	rec.meta = event.isMetaDown();
	rec.leftMouse = event.isLeftDown();
	rec.rightMouse = event.isRightDown();
	rec.middleMouse = event.isMiddleDown();
	rec.timestamp = getElapsedSeconds();

	addEvent( rec );
}

void EventTestApp::handleKeyDown( KeyEvent& event )
{
	// This handler runs at priority 2 (before ImGui at priority 1)
	// We observe all events but don't mark them handled, so ImGui can still consume them

	if( ! mShowKeyboard )
		return;

	EventRecord rec;
	rec.type = "KeyDown";

	char ch = ( event.getChar() == 0 ) ? ' ' : event.getChar();
	stringstream ss;
	ss << "'" << ch << "'";
	rec.details = ss.str();

	stringstream ss2;
	ss2 << "0x" << std::hex << std::uppercase << event.getCode();
	rec.extraInfo = ss2.str();

	rec.shift = event.isShiftDown();
	rec.ctrl = event.isControlDown();
	rec.alt = event.isAltDown();
	rec.meta = event.isMetaDown();
	rec.leftMouse = false;
	rec.rightMouse = false;
	rec.middleMouse = false;
	rec.timestamp = getElapsedSeconds();

	addEvent( rec );
	// Note: We do NOT call event.setHandled() - we're just observing
}

void EventTestApp::handleKeyUp( KeyEvent& event )
{
	// This handler runs at priority 2 (before ImGui at priority 1)
	// We observe all events but don't mark them handled, so ImGui can still consume them

	if( ! mShowKeyboard )
		return;

	EventRecord rec;
	rec.type = "KeyUp";

	char ch = ( event.getChar() == 0 ) ? ' ' : event.getChar();
	stringstream ss;
	ss << "'" << ch << "'";
	rec.details = ss.str();

	stringstream ss2;
	ss2 << "0x" << std::hex << std::uppercase << event.getCode();
	rec.extraInfo = ss2.str();

	rec.shift = event.isShiftDown();
	rec.ctrl = event.isControlDown();
	rec.alt = event.isAltDown();
	rec.meta = event.isMetaDown();
	rec.leftMouse = false;
	rec.rightMouse = false;
	rec.middleMouse = false;
	rec.timestamp = getElapsedSeconds();

	addEvent( rec );
	// Note: We do NOT call event.setHandled() - we're just observing
}

void EventTestApp::handleKeyChar( KeyEvent& event )
{
	// This handler runs at priority 2 (before ImGui at priority 1)
	// We observe all events but don't mark them handled, so ImGui can still consume them

	if( ! mShowKeyboard )
		return;

	EventRecord rec;
	rec.type = "KeyChar";

	uint32_t utf32 = event.getCharUtf32();

	if( utf32 > 0 ) {
		string utf8char = utf32ToUtf8( utf32 );

		stringstream ss;
		if( utf32 >= 32 && utf32 != 127 ) {
			ss << "'" << utf8char << "'";
		}
		else {
			ss << "[ctrl]";
		}
		rec.details = ss.str();

		stringstream ss2;
		ss2 << "U+" << hex << uppercase << utf32;
		rec.extraInfo = ss2.str();
	}
	else {
		rec.details = "[none]";
		rec.extraInfo = "";
	}

	rec.shift = event.isShiftDown();
	rec.ctrl = event.isControlDown();
	rec.alt = event.isAltDown();
	rec.meta = event.isMetaDown();
	rec.leftMouse = false;
	rec.rightMouse = false;
	rec.middleMouse = false;
	rec.timestamp = getElapsedSeconds();

	addEvent( rec );
	// Note: We do NOT call event.setHandled() - we're just observing
}

void EventTestApp::fileDrop( FileDropEvent event )
{
	EventRecord rec;
	rec.type = "FileDrop";

	stringstream ss;
	ss << event.getNumFiles() << " file(s)";
	rec.details = ss.str();

	stringstream ss2;
	ss2 << "(" << std::setw(5) << event.getPos().x << "," << std::setw(5) << event.getPos().y << ")";
	rec.extraInfo = ss2.str();

	rec.shift = event.isShiftDown();
	rec.ctrl = event.isControlDown();
	rec.alt = event.isAltDown();
	rec.meta = event.isMetaDown();
	rec.leftMouse = false;
	rec.rightMouse = false;
	rec.middleMouse = false;
	rec.timestamp = getElapsedSeconds();

	addEvent( rec );
}

CINDER_APP( EventTestApp, RendererGl, []( EventTestApp::Settings *settings ) {
	settings->setWindowSize( 1024, 768 );
	settings->setTitle( "Cinder Event Test" );
	settings->setHighDensityDisplayEnabled( true );
} )
