#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Rand.h"
#include "cinder/qtime/QuickTimeGl.h"
#include "cinder/CinderImGui.h"
#include "cinder/Log.h"

#include <deque>

using namespace ci;
using namespace ci::app;
using namespace std;

// Configuration
static constexpr int	HARD_CAP = 10;			// Absolute max simultaneous videos
static constexpr float	AVG_OP_INTERVAL = 0.5f; // Average seconds between random operations
static constexpr float	PANEL_WIDTH = 400.0f;
static constexpr float	CELL_PADDING = 8.0f;	// Padding around video cells

// Log entry for history
struct LogEntry
{
	double		timestamp;
	string		message;
	bool		isError;
};

// Active video slot
struct VideoSlot
{
	qtime::MovieGlRef	movie;
	fs::path			path;
	string				name;
	double				loadTime;
	int					operationCount{ 0 };
};

class QuickTimeStressTestApp : public App {
  public:
	void setup() override;
	void update() override;
	void draw() override;
	void cleanup() override;

  private:
	void scanDirectory( const fs::path& dir );
	void loadRandomVideo();
	void closeRandomVideo();
	void performRandomOperation();
	void logMessage( const string& msg, bool isError = false );

	void drawVideoGrid();
	void drawStatsPanel();

	// Candidate video files
	vector<fs::path>	mCandidates;
	set<fs::path>		mFailedPaths;

	// Active videos
	vector<VideoSlot>	mActiveVideos;

	// Stats
	int					mTotalLoads{ 0 };
	int					mTotalCloses{ 0 };
	int					mTotalOperations{ 0 };
	int					mTotalErrors{ 0 };
	double				mStartTime{ 0.0 };
	double				mLastOpTime{ 0.0 };
	double				mNextOpTime{ 0.0 };

	// Log history
	deque<LogEntry>		mLogHistory;
	static constexpr size_t MAX_LOG_ENTRIES = 100;

	// UI state
	bool				mShowStats{ true };
	bool				mPaused{ false };
	gl::TextureFontRef	mFont;

	// Operation toggles
	bool				mEnableSeeking{ true };
	bool				mEnablePausing{ true };
	bool				mEnableRateChanges{ true };
};

void QuickTimeStressTestApp::setup()
{
	ImGui::Initialize( ImGui::Options().window( getWindow() ) );
	mFont = gl::TextureFont::create( Font( "Arial", 18 ) );
	mStartTime = getElapsedSeconds();
	mNextOpTime = mStartTime + randFloat( 0.1f, AVG_OP_INTERVAL * 2.0f );

	// Prompt for directory
	fs::path dir = getFolderPath();
	if( dir.empty() ) {
		logMessage( "No directory selected, quitting.", true );
		quit();
		return;
	}

	logMessage( "Scanning directory: " + dir.string() );
	scanDirectory( dir );

	if( mCandidates.empty() ) {
		logMessage( "No video files found!", true );
		quit();
		return;
	}

	logMessage( "Found " + to_string( mCandidates.size() ) + " video files" );
	CI_LOG_I( "QuickTimeStressTest: Found " << mCandidates.size() << " candidates" );
}

void QuickTimeStressTestApp::cleanup()
{
	mActiveVideos.clear();
}

void QuickTimeStressTestApp::scanDirectory( const fs::path& dir )
{
	try {
		for( auto& entry : fs::recursive_directory_iterator( dir ) ) {
			if( ! entry.is_regular_file() )
				continue;

			auto ext = entry.path().extension().string();
			// Convert to lowercase for comparison
			transform( ext.begin(), ext.end(), ext.begin(), ::tolower );

			if( ext == ".mp4" || ext == ".mov" ) {
				mCandidates.push_back( entry.path() );
			}
		}
	}
	catch( const fs::filesystem_error& e ) {
		logMessage( "Filesystem error: " + string( e.what() ), true );
	}
}

void QuickTimeStressTestApp::loadRandomVideo()
{
	if( mCandidates.empty() || (int)mActiveVideos.size() >= HARD_CAP )
		return;

	// Pick a random candidate that's not already loaded
	vector<fs::path> available;
	set<fs::path> loadedPaths;
	for( auto& slot : mActiveVideos )
		loadedPaths.insert( slot.path );

	for( auto& path : mCandidates ) {
		if( loadedPaths.find( path ) == loadedPaths.end() )
			available.push_back( path );
	}

	if( available.empty() )
		return;

	fs::path chosen = available[randInt( (int)available.size() )];

	try {
		auto movie = qtime::MovieGl::create( chosen );
		movie->setLoop( true, false );
		movie->play();

		VideoSlot slot;
		slot.movie = movie;
		slot.path = chosen;
		slot.name = chosen.filename().string();
		slot.loadTime = getElapsedSeconds();

		mActiveVideos.push_back( slot );
		mTotalLoads++;

		logMessage( "Loaded: " + slot.name + " (" + to_string( movie->getWidth() ) + "x" + to_string( movie->getHeight() ) + ")" );
	}
	catch( const ci::Exception& e ) {
		mTotalErrors++;
		logMessage( "Failed to load: " + chosen.filename().string() + " - " + e.what(), true );

		// Remove from candidates
		mCandidates.erase( remove( mCandidates.begin(), mCandidates.end(), chosen ), mCandidates.end() );
		mFailedPaths.insert( chosen );
	}
}

void QuickTimeStressTestApp::closeRandomVideo()
{
	if( mActiveVideos.empty() )
		return;

	int idx = randInt( (int)mActiveVideos.size() );
	logMessage( "Closed: " + mActiveVideos[idx].name + " (ops: " + to_string( mActiveVideos[idx].operationCount ) + ")" );
	mActiveVideos.erase( mActiveVideos.begin() + idx );
	mTotalCloses++;
}

void QuickTimeStressTestApp::performRandomOperation()
{
	if( mActiveVideos.empty() )
		return;

	int idx = randInt( (int)mActiveVideos.size() );
	auto& slot = mActiveVideos[idx];
	if( ! slot.movie )
		return;

	// Build weighted list of enabled operations
	// Format: { operation_id, weight }
	// 0=SeekRandom, 1=SeekStart, 2=Pause, 3=Play, 4=Toggle, 5=Volume, 6=Rate
	vector<pair<int, float>> ops;

	if( mEnableSeeking ) {
		ops.push_back( { 0, 40.0f } );	// Seek random
		ops.push_back( { 1, 15.0f } );	// Seek to start
	}
	if( mEnablePausing ) {
		ops.push_back( { 2, 5.0f } );	// Pause
		ops.push_back( { 4, 5.0f } );	// Toggle
	}
	ops.push_back( { 3, 15.0f } );		// Play (always enabled)
	ops.push_back( { 5, 10.0f } );		// Volume (always enabled)
	if( mEnableRateChanges ) {
		ops.push_back( { 6, 10.0f } );	// Rate
	}

	if( ops.empty() )
		return;

	// Calculate total weight and pick random operation
	float totalWeight = 0.0f;
	for( auto& op : ops )
		totalWeight += op.second;

	float r = randFloat( totalWeight );
	float cumulative = 0.0f;
	int selectedOp = ops.back().first;

	for( auto& op : ops ) {
		cumulative += op.second;
		if( r < cumulative ) {
			selectedOp = op.first;
			break;
		}
	}

	slot.operationCount++;
	mTotalOperations++;

	switch( selectedOp ) {
		case 0: { // Seek random
			float duration = slot.movie->getDuration();
			if( duration > 0 ) {
				float seekTo = randFloat( duration );
				slot.movie->seekToTime( seekTo );
			}
			break;
		}
		case 1: // Seek to start
			slot.movie->seekToStart();
			break;
		case 2: // Pause
			slot.movie->stop();
			break;
		case 3: // Play
			slot.movie->play();
			break;
		case 4: // Toggle play/pause
			if( slot.movie->isPlaying() )
				slot.movie->stop();
			else
				slot.movie->play();
			break;
		case 5: // Volume
			slot.movie->setVolume( randFloat() );
			break;
		case 6: // Rate
			slot.movie->setRate( randFloat( 0.5f, 2.0f ) );
			break;
	}
}

void QuickTimeStressTestApp::logMessage( const string& msg, bool isError )
{
	LogEntry entry;
	entry.timestamp = getElapsedSeconds() - mStartTime;
	entry.message = msg;
	entry.isError = isError;

	mLogHistory.push_front( entry );
	while( mLogHistory.size() > MAX_LOG_ENTRIES )
		mLogHistory.pop_back();

	if( isError )
		CI_LOG_E( msg );
	else
		CI_LOG_I( msg );
}

void QuickTimeStressTestApp::update()
{
	if( mPaused )
		return;

	double now = getElapsedSeconds();

	// Perform random operations at intervals
	if( now >= mNextOpTime ) {
		// Determine target video count (half of candidates, capped at HARD_CAP/2)
		int target = min( (int)mCandidates.size() / 2, HARD_CAP / 2 );
		target = max( target, 1 );

		int current = (int)mActiveVideos.size();

		// Probabilities based on current vs target
		float loadProb, closeProb;
		if( current < target ) {
			loadProb = 0.35f;	// More likely to load when below target
			closeProb = 0.10f;
		}
		else if( current > target ) {
			loadProb = 0.10f;	// More likely to close when above target
			closeProb = 0.35f;
		}
		else {
			loadProb = 0.20f;	// Balanced when at target
			closeProb = 0.20f;
		}

		// Also allow for zero videos sometimes
		if( current == 0 && randFloat() < 0.9f ) {
			loadRandomVideo();
		}
		// Don't exceed hard cap
		else if( current >= HARD_CAP ) {
			if( randFloat() < 0.5f )
				closeRandomVideo();
			else
				performRandomOperation();
		}
		else {
			float r = randFloat();
			if( r < loadProb ) {
				loadRandomVideo();
			}
			else if( r < loadProb + closeProb ) {
				closeRandomVideo();
			}
			else {
				performRandomOperation();
			}
		}

		mLastOpTime = now;
		// Random interval centered around AVG_OP_INTERVAL
		mNextOpTime = now + randFloat( AVG_OP_INTERVAL * 0.2f, AVG_OP_INTERVAL * 1.8f );
	}
}

void QuickTimeStressTestApp::draw()
{
	gl::clear( Color( 0.08f, 0.08f, 0.1f ) );

	drawVideoGrid();

	if( mShowStats )
		drawStatsPanel();
}

void QuickTimeStressTestApp::drawVideoGrid()
{
	if( mActiveVideos.empty() ) {
		gl::color( 0.4f, 0.4f, 0.4f );
		string msg = "Stress test running...\nVideos loaded: 0";
		vec2 size = mFont->measureString( msg );
		mFont->drawString( msg, vec2( getWindowCenter() ) - size * 0.5f );
		return;
	}

	float panelWidth = mShowStats ? PANEL_WIDTH : 0.0f;
	float availableWidth = getWindowWidth() - panelWidth;
	float windowHeight = (float)getWindowHeight();

	// Guard against zero dimensions
	if( availableWidth <= 0 || windowHeight <= 0 )
		return;

	int count = (int)mActiveVideos.size();
	int cols = (int)ceil( sqrt( (double)count ) );
	int rows = ( count + cols - 1 ) / cols;

	float cellWidth = availableWidth / (float)cols;
	float cellHeight = windowHeight / (float)rows;

	// Guard against zero cell dimensions
	if( cellWidth <= 0 || cellHeight <= 0 )
		return;

	for( int i = 0; i < count; ++i ) {
		auto& slot = mActiveVideos[i];
		auto texture = slot.movie ? slot.movie->getTexture() : nullptr;

		int col = i % cols;
		int row = i / cols;

		float x = col * cellWidth;
		float y = row * cellHeight;

		if( texture && slot.movie ) {
			float videoAspect = slot.movie->getAspectRatio();
			if( videoAspect <= 0 ) videoAspect = 1.0f; // Guard against invalid aspect
			float cellAspect = cellWidth / cellHeight;

			float drawWidth, drawHeight;
			if( videoAspect > cellAspect ) {
				drawWidth = cellWidth - CELL_PADDING;
				drawHeight = drawWidth / videoAspect;
			}
			else {
				drawHeight = cellHeight - CELL_PADDING;
				drawWidth = drawHeight * videoAspect;
			}

			float drawX = x + ( cellWidth - drawWidth ) / 2;
			float drawY = y + ( cellHeight - drawHeight ) / 2;

			gl::color( Color::white() );
			gl::draw( texture, Rectf( drawX, drawY, drawX + drawWidth, drawY + drawHeight ) );

			// Draw status indicator
			gl::color( slot.movie->isPlaying() ? ColorA( 0, 1, 0, 0.7f ) : ColorA( 1, 0.5f, 0, 0.7f ) );
			gl::drawSolidCircle( vec2( drawX + 10, drawY + 10 ), 5 );
		}
		else {
			gl::color( 0.15f, 0.15f, 0.15f );
			gl::drawSolidRect( Rectf( x + CELL_PADDING / 2, y + CELL_PADDING / 2,
									  x + cellWidth - CELL_PADDING / 2, y + cellHeight - CELL_PADDING / 2 ) );
		}
	}
}

void QuickTimeStressTestApp::drawStatsPanel()
{
	ImGui::SetNextWindowPos( ImVec2( getWindowWidth() - PANEL_WIDTH, 0 ), ImGuiCond_Always );
	ImGui::SetNextWindowSize( ImVec2( PANEL_WIDTH, (float)getWindowHeight() ), ImGuiCond_Always );

	ImGui::Begin( "Stress Test Stats", &mShowStats, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize );

	// Controls
	if( ImGui::Checkbox( "Paused", &mPaused ) ) {
		logMessage( mPaused ? "Test paused" : "Test resumed" );
	}

	ImGui::Separator();

	// Operation toggles
	if( ImGui::CollapsingHeader( "Operation Toggles", ImGuiTreeNodeFlags_DefaultOpen ) ) {
		ImGui::Checkbox( "Enable Seeking", &mEnableSeeking );
		ImGui::Checkbox( "Enable Pausing", &mEnablePausing );
		ImGui::Checkbox( "Enable Rate Changes", &mEnableRateChanges );
	}

	ImGui::Separator();

	// Global stats
	if( ImGui::CollapsingHeader( "Global Stats", ImGuiTreeNodeFlags_DefaultOpen ) ) {
		double elapsed = getElapsedSeconds() - mStartTime;
		int mins = (int)( elapsed / 60 );
		int secs = (int)elapsed % 60;

		ImGui::Text( "Runtime: %d:%02d", mins, secs );
		ImGui::Text( "FPS: %.1f", getAverageFps() );
		ImGui::Separator();
		ImGui::Text( "Candidate files: %d", (int)mCandidates.size() );
		ImGui::Text( "Failed files: %d", (int)mFailedPaths.size() );
		ImGui::Separator();
		ImGui::Text( "Total loads: %d", mTotalLoads );
		ImGui::Text( "Total closes: %d", mTotalCloses );
		ImGui::Text( "Total operations: %d", mTotalOperations );
		ImGui::Text( "Total errors: %d", mTotalErrors );

		if( elapsed > 0 ) {
			ImGui::Text( "Ops/sec: %.2f", mTotalOperations / elapsed );
		}
	}

	ImGui::Separator();

	// Active videos
	if( ImGui::CollapsingHeader( "Active Videos", ImGuiTreeNodeFlags_DefaultOpen ) ) {
		ImGui::Text( "Count: %d / %d (hard cap)", (int)mActiveVideos.size(), HARD_CAP );

		for( size_t i = 0; i < mActiveVideos.size(); ++i ) {
			auto& slot = mActiveVideos[i];
			ImGui::PushID( (int)i );

			bool playing = slot.movie && slot.movie->isPlaying();
			ImVec4 color = playing ? ImVec4( 0.4f, 0.9f, 0.4f, 1.0f ) : ImVec4( 0.9f, 0.6f, 0.2f, 1.0f );
			ImGui::TextColored( color, "%s", playing ? ">" : "||" );
			ImGui::SameLine();

			// Truncate long names
			string displayName = slot.name;
			if( displayName.length() > 25 )
				displayName = displayName.substr( 0, 22 ) + "...";

			ImGui::Text( "%s", displayName.c_str() );

			if( slot.movie ) {
				ImGui::SameLine();
				ImGui::TextColored( ImVec4( 0.5f, 0.5f, 0.5f, 1.0f ), "(%dx%d)", slot.movie->getWidth(), slot.movie->getHeight() );
			}

			ImGui::PopID();
		}

		if( mActiveVideos.empty() ) {
			ImGui::TextColored( ImVec4( 0.5f, 0.5f, 0.5f, 1.0f ), "No active videos" );
		}
	}

	ImGui::Separator();

	// Log history
	if( ImGui::CollapsingHeader( "Log History", ImGuiTreeNodeFlags_DefaultOpen ) ) {
		ImGui::BeginChild( "LogScroll", ImVec2( 0, 200 ), true );

		for( auto& entry : mLogHistory ) {
			ImVec4 color = entry.isError ? ImVec4( 1.0f, 0.4f, 0.4f, 1.0f ) : ImVec4( 0.8f, 0.8f, 0.8f, 1.0f );

			int mins = (int)( entry.timestamp / 60 );
			int secs = (int)entry.timestamp % 60;
			ImGui::TextColored( ImVec4( 0.5f, 0.5f, 0.5f, 1.0f ), "[%d:%02d]", mins, secs );
			ImGui::SameLine();
			ImGui::TextColored( color, "%s", entry.message.c_str() );
		}

		ImGui::EndChild();
	}

	ImGui::End();
}

CINDER_APP( QuickTimeStressTestApp, RendererGl, []( App::Settings* settings ) {
	settings->setWindowSize( 1600, 900 );
	settings->setTitle( "QuickTime Stress Test" );
	settings->setResizable( true );
} )
