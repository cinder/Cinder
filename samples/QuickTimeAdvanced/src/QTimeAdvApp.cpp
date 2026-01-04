#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Surface.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Rand.h"
#include "cinder/qtime/QuickTimeGl.h"
#include "cinder/CinderImGui.h"
#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;
using namespace std;

// Per-video state for UI
struct VideoSlot
{
	qtime::MovieGlRef	movie;
	string				name;
	float				seekPosition = 0.0f;
	float				volume = 1.0f;
	float				rate = 1.0f;
	bool				loop = true;
	bool				selected = false;
	bool				isSeeking = false;  // True while user is dragging seek slider
};

class QTimeAdvApp : public App {
  public:
	void setup() override;
	void keyDown( KeyEvent event ) override;
	void fileDrop( FileDropEvent event ) override;
	void update() override;
	void draw() override;

	void loadMovieFile( const fs::path& path );
	void drawVideoGrid();
	void drawControlPanel();
	void drawVideoControls( VideoSlot& slot, int index );

  private:
	static constexpr float			kPanelWidth = 320.0f;

	vector<VideoSlot>				mVideos;
	gl::TextureFontRef				mFont;
	int								mVideoToDelete = -1;  // Deferred deletion index

	fs::path	mLastPath;
	int			mGridColumns = 2;
	bool		mShowControls = true;
	bool		mAutoPlay = true;
	int			mSelectedVideo = -1;
};

void QTimeAdvApp::setup()
{
	// Initialize ImGui
	ImGui::Initialize( ImGui::Options().window( getWindow() ).enableKeyboard( true ) );

	// Cache font for placeholder text
	mFont = gl::TextureFont::create( Font( "Arial", 24 ) );

	CI_LOG_I( "QuickTimeAdvanced" );
}

void QTimeAdvApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'f' ) {
		setFullScreen( ! isFullScreen() );
	}
	else if( event.getChar() == 'o' ) {
		fs::path moviePath = getOpenFilePath();
		if( ! moviePath.empty() )
			loadMovieFile( moviePath );
	}
	else if( event.getChar() == 'c' ) {
		mShowControls = ! mShowControls;
	}
	else if( event.getChar() == 'x' ) {
		mVideos.clear();
		mSelectedVideo = -1;
	}
	else if( event.getChar() == ' ' ) {
		// Toggle play/pause on selected or all videos
		if( mSelectedVideo >= 0 && mSelectedVideo < (int)mVideos.size() ) {
			auto& movie = mVideos[mSelectedVideo].movie;
			if( movie ) {
				if( movie->isPlaying() )
					movie->stop();
				else
					movie->play();
			}
		}
		else {
			for( auto& slot : mVideos ) {
				if( slot.movie ) {
					if( slot.movie->isPlaying() )
						slot.movie->stop();
					else
						slot.movie->play();
				}
			}
		}
	}
}

void QTimeAdvApp::loadMovieFile( const fs::path& moviePath )
{
	try {
		auto movie = qtime::MovieGl::create( moviePath );

		VideoSlot slot;
		slot.movie = movie;
		slot.name = moviePath.filename().string();
		slot.loop = true;

		movie->setLoop( true, false );
		if( mAutoPlay )
			movie->play();

		mVideos.push_back( slot );
		mLastPath = moviePath;

		CI_LOG_I( "Loaded: " << slot.name << " (" << movie->getWidth() << "x" << movie->getHeight() << ")" );
	}
	catch( ci::Exception& exc ) {
		CI_LOG_E( "Failed to load: " << moviePath << " - " << exc.what() );
	}
}

void QTimeAdvApp::fileDrop( FileDropEvent event )
{
	for( size_t i = 0; i < event.getNumFiles(); ++i )
		loadMovieFile( event.getFile( i ) );
}

void QTimeAdvApp::update()
{
	// Handle deferred video deletion (safe outside of iteration)
	if( mVideoToDelete >= 0 && mVideoToDelete < (int)mVideos.size() ) {
		// Adjust selected video index if needed
		if( mSelectedVideo == mVideoToDelete ) {
			mSelectedVideo = -1;
		}
		else if( mSelectedVideo > mVideoToDelete ) {
			--mSelectedVideo;
		}
		mVideos.erase( mVideos.begin() + mVideoToDelete );
		mVideoToDelete = -1;
	}

	// Sync UI state with video state (skip if user is seeking)
	for( auto& slot : mVideos ) {
		if( slot.movie && ! slot.isSeeking ) {
			slot.seekPosition = slot.movie->getCurrentTime();
		}
	}
}

void QTimeAdvApp::draw()
{
	gl::clear( Color( 0.1f, 0.1f, 0.12f ) );

	// Draw video grid
	drawVideoGrid();

	// Draw ImGui control panel
	if( mShowControls ) {
		drawControlPanel();
	}
}

void QTimeAdvApp::drawVideoGrid()
{
	if( mVideos.empty() ) {
		// Draw placeholder text using cached font
		gl::color( 0.5f, 0.5f, 0.5f );
		string msg = "Drag & drop video files here\nor press 'O' to open";
		vec2 size = mFont->measureString( msg );
		mFont->drawString( msg, vec2( getWindowCenter() ) - size * 0.5f );
		return;
	}

	int cols = std::min( mGridColumns, (int)mVideos.size() );
	int rows = ( (int)mVideos.size() + cols - 1 ) / cols;

	float cellWidth = getWindowWidth() / (float)cols;
	float cellHeight = getWindowHeight() / (float)rows;

	// Adjust for control panel if visible
	float panelWidth = mShowControls ? kPanelWidth : 0.0f;
	float availableWidth = getWindowWidth() - panelWidth;
	cellWidth = availableWidth / (float)cols;

	for( size_t i = 0; i < mVideos.size(); ++i ) {
		auto& slot = mVideos[i];
		auto texture = slot.movie->getTexture();

		int col = (int)i % cols;
		int row = (int)i / cols;

		float x = col * cellWidth;
		float y = row * cellHeight;

		if( texture ) {
			// Calculate aspect-correct size
			float videoAspect = slot.movie->getAspectRatio();
			float cellAspect = cellWidth / cellHeight;

			float drawWidth, drawHeight;
			if( videoAspect > cellAspect ) {
				drawWidth = cellWidth - 8;
				drawHeight = drawWidth / videoAspect;
			}
			else {
				drawHeight = cellHeight - 8;
				drawWidth = drawHeight * videoAspect;
			}

			float drawX = x + ( cellWidth - drawWidth ) / 2;
			float drawY = y + ( cellHeight - drawHeight ) / 2;

			// Draw selection highlight
			if( (int)i == mSelectedVideo ) {
				gl::color( 0.3f, 0.5f, 0.8f );
				gl::drawStrokedRect( Rectf( drawX - 2, drawY - 2, drawX + drawWidth + 2, drawY + drawHeight + 2 ), 3.0f );
			}

			gl::color( Color::white() );
			gl::draw( texture, Rectf( drawX, drawY, drawX + drawWidth, drawY + drawHeight ) );
		}
		else {
			// Draw placeholder
			gl::color( 0.2f, 0.2f, 0.2f );
			gl::drawSolidRect( Rectf( x + 4, y + 4, x + cellWidth - 4, y + cellHeight - 4 ) );
		}
	}
}

void QTimeAdvApp::drawControlPanel()
{
	ImGui::SetNextWindowPos( ImVec2( getWindowWidth() - kPanelWidth, 0 ), ImGuiCond_Always );
	ImGui::SetNextWindowSize( ImVec2( kPanelWidth, (float)getWindowHeight() ), ImGuiCond_Always );

	ImGui::Begin( "Video Controls", &mShowControls, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize );

	// Global controls
	if( ImGui::CollapsingHeader( "Global", ImGuiTreeNodeFlags_DefaultOpen ) ) {
		ImGui::SliderInt( "Grid Columns", &mGridColumns, 1, 4 );
		ImGui::Checkbox( "Auto-play on load", &mAutoPlay );

		if( ImGui::Button( "Open File (O)" ) ) {
			fs::path moviePath = getOpenFilePath();
			if( ! moviePath.empty() )
				loadMovieFile( moviePath );
		}
		ImGui::SameLine();
		if( ImGui::Button( "Clear All (X)" ) ) {
			mVideos.clear();
			mSelectedVideo = -1;
		}

		ImGui::Separator();

		// Play/Pause all
		if( ImGui::Button( "Play All" ) ) {
			for( auto& slot : mVideos )
				if( slot.movie ) slot.movie->play();
		}
		ImGui::SameLine();
		if( ImGui::Button( "Pause All" ) ) {
			for( auto& slot : mVideos )
				if( slot.movie ) slot.movie->stop();
		}
		ImGui::SameLine();
		if( ImGui::Button( "Restart All" ) ) {
			for( auto& slot : mVideos ) {
				if( slot.movie ) {
					slot.movie->seekToStart();
					slot.movie->play();
				}
			}
		}
	}

	ImGui::Separator();

	// Per-video controls
	if( ImGui::CollapsingHeader( "Videos", ImGuiTreeNodeFlags_DefaultOpen ) ) {
		if( mVideos.empty() ) {
			ImGui::TextColored( ImVec4( 0.5f, 0.5f, 0.5f, 1.0f ), "No videos loaded" );
		}
		else {
			for( int i = 0; i < (int)mVideos.size(); ++i ) {
				ImGui::PushID( i );
				drawVideoControls( mVideos[i], i );
				ImGui::PopID();
			}
		}
	}

	ImGui::Separator();

	// Stats
	if( ImGui::CollapsingHeader( "Stats" ) ) {
		ImGui::Text( "Videos loaded: %d", (int)mVideos.size() );
		float fps = getAverageFps();
		ImGui::Text( "FPS: %.1f", fps );
		ImGui::Text( "Frame time: %.2f ms", fps > 0.0f ? 1000.0f / fps : 0.0f );
	}

	ImGui::End();
}

void QTimeAdvApp::drawVideoControls( VideoSlot& slot, int index )
{
	bool isSelected = ( index == mSelectedVideo );

	// Selectable header
	if( ImGui::Selectable( slot.name.c_str(), isSelected ) ) {
		mSelectedVideo = isSelected ? -1 : index;
	}

	if( ! slot.movie )
		return;

	ImGui::Indent();

	// Info line
	ImGui::TextColored( ImVec4( 0.6f, 0.6f, 0.6f, 1.0f ), "%dx%d | %.1fs | %.1f fps",
		slot.movie->getWidth(), slot.movie->getHeight(),
		slot.movie->getDuration(), slot.movie->getFramerate() );

	// Play/Pause
	bool isPlaying = slot.movie->isPlaying();
	if( ImGui::Button( isPlaying ? "Pause" : "Play", ImVec2( 50, 0 ) ) ) {
		if( isPlaying )
			slot.movie->stop();
		else
			slot.movie->play();
	}
	ImGui::SameLine();
	if( ImGui::Button( "<<", ImVec2( 30, 0 ) ) )
		slot.movie->seekToStart();
	ImGui::SameLine();
	if( ImGui::Button( ">>", ImVec2( 30, 0 ) ) )
		slot.movie->seekToEnd();
	ImGui::SameLine();
	if( ImGui::Button( "X", ImVec2( 25, 0 ) ) ) {
		// Defer deletion to update() to avoid vector invalidation during iteration
		mVideoToDelete = index;
	}

	// Seek slider with drag tracking to prevent jitter
	float duration = slot.movie->getDuration();
	if( duration > 0 ) {
		float pos = slot.seekPosition;
		if( ImGui::SliderFloat( "##seek", &pos, 0.0f, duration, "%.1fs" ) ) {
			slot.movie->seekToTime( pos );
			slot.seekPosition = pos;
		}
		// Track whether user is actively dragging the slider
		slot.isSeeking = ImGui::IsItemActive();
	}

	// Volume
	if( ImGui::SliderFloat( "Vol", &slot.volume, 0.0f, 1.0f, "%.2f" ) )
		slot.movie->setVolume( slot.volume );

	// Rate
	if( ImGui::SliderFloat( "Rate", &slot.rate, 0.0f, 4.0f, "%.2fx" ) )
		slot.movie->setRate( slot.rate );

	// Loop
	if( ImGui::Checkbox( "Loop", &slot.loop ) )
		slot.movie->setLoop( slot.loop, false );

	ImGui::Unindent();
	ImGui::Spacing();
}

CINDER_APP( QTimeAdvApp, RendererGl, []( App::Settings* settings ) {
	settings->setWindowSize( 1280, 720 );
	settings->setTitle( "QuickTime Advanced" );
	settings->setResizable( true );
} )