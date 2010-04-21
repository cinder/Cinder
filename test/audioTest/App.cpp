
#include <iostream>
#include <string>
#include <sstream>
#include <windows.h>
#include <WMSDK.h>

#include <xaudio2.h>

#include "flint/Flint.h"
#include "flint/app/AppBasic.h"
#include "flint/Stream.h"

#define MAX_BUFFER_COUNT 3


using namespace fli;
using namespace fli::app;


//BOOL CALLBACK acmDriverEnumCallback( HACMDRIVERID hadid, DWORD dwInstance, DWORD fdwSupport );
class StreamingVoiceContext : public IXAudio2VoiceCallback
{
public:
	HANDLE hBufferEndEvent;

	StreamingVoiceContext() : hBufferEndEvent( CreateEvent( NULL, FALSE, FALSE, NULL ) ){}
	~StreamingVoiceContext(){ CloseHandle( hBufferEndEvent ); }
	void STDMETHODCALLTYPE OnBufferEnd( void* pvContext ) {
		//std::cout << "Buffers over" << std::endl;
		SetEvent( hBufferEndEvent );
	}

    //Unused methods are stubs
	void STDMETHODCALLTYPE OnStreamEnd() { }
    void STDMETHODCALLTYPE OnVoiceProcessingPassEnd() { }
    void STDMETHODCALLTYPE OnVoiceProcessingPassStart(UINT32 SamplesRequired) {    }
    void STDMETHODCALLTYPE OnBufferStart(void * pBufferContext) {  }
    void STDMETHODCALLTYPE OnLoopEnd(void * pBufferContext) {    }
    void STDMETHODCALLTYPE OnVoiceError(void * pBufferContext, HRESULT Error) { }
};

IXAudio2 * gXAudio;

class WMACallback : public IWMReaderCallback, public IWMReaderCallbackAdvanced
{
public:
	WMACallback();
	~WMACallback();
	void Clear();

	HRESULT __stdcall QueryInterface(const struct _GUID& guis, void** ppInterface);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();

	HRESULT __stdcall OnStatus(WMT_STATUS Status, HRESULT hr, WMT_ATTR_DATATYPE dwType, BYTE* pValue, void* pvContext);
	HRESULT __stdcall OnSample(DWORD dwOutputNum, QWORD cnsSampleTime, QWORD cnsSampleDuration, DWORD dwFlags, INSSBuffer* pSample, void* pvContext);
	HRESULT __stdcall OnStreamSample(WORD wStreamNum, QWORD cnsSampleTime, QWORD cnsSampleDuration, DWORD dwFalgs, INSSBuffer * pSample, void * pvContext);
	HRESULT __stdcall OnTime(QWORD qwCurrentTime, void * pvContext);
	HRESULT __stdcall OnStreamSelection(WORD wStreamCount, WORD * pStreamNumbers, WMT_STREAM_SELECTION * pSelections, void * pvContext );
	HRESULT __stdcall OnOutputPropsChanged(DWORD dwOutputNum, WM_MEDIA_TYPE * pMediaType, void * pvContext);
	HRESULT __stdcall AllocateForOutput(DWORD dwOutputNum, DWORD cbBuffer, INSSBuffer  *  * ppBuffer, void * pvContext);
	HRESULT __stdcall AllocateForStream(WORD wStreamNum, DWORD cbBuffer, INSSBuffer  *  * ppBuffer, void  *pvContext);
private:
	LONG	m_iRefCount;
};

class WMA
{
public:
	WMA();
	~WMA();
	void Clear();

	//bool Open(std::string sFileName);
	bool Open(BYTE* pbData, uint32_t dwDataSize);
	bool Close();

	void Play();
	void seek( uint64_t seekDur );

	bool SingleRead(uint8_t* pBuffer, uint32_t dwSizeToRead, uint32_t* pdwSizeRead);
	uint32_t GetSize();
	bool Reset();

	WAVEFORMATEX* GetFormat();
	bool isEOF();

	void Destroy();
	
	friend class WMACallback;
private:

	bool PreOpen();
	bool PostOpen();

	bool OpenReader();

	HRESULT OnStatus(WMT_STATUS Status, HRESULT hr, WMT_ATTR_DATATYPE dwType, BYTE* pValue);
	HRESULT OnSample(DWORD dwOutputNum, QWORD cnsSampleTime, QWORD cnsSampleDuration, DWORD dwFlags, INSSBuffer* pSample);
	HRESULT STDMETHODCALLTYPE OnTime(QWORD qwCurrentTime);
private:
	uint32_t mMaxBufferSize;
	uint8_t * mDecodedBuffers;

	StreamingVoiceContext mVoiceCallback;
	IXAudio2SourceVoice	* mSourceVoice;
	
	uint32_t mCurrentBuffer;
	uint64_t mCurrentTime;

	IWMReader * m_pReader; 
	IWMReaderAdvanced * m_pReaderAdvanced;
	IWMReaderAdvanced2 * m_pReaderAdvanced2;
	IWMHeaderInfo * m_pHeaderInfo;
	WAVEFORMATEX m_WaveFormatEx;
	WMACallback m_Callback;


	HANDLE mBufferEndEvent;
	HANDLE m_hWaitEvent;
	HANDLE m_hRespondEvent;

	bool m_bOpen;
	HRESULT m_hrCallbackResult;

	uint32_t m_nStreamSize;
	uint32_t m_nBytesRead;

	bool m_bFirstRead;
	uint8_t * m_pBuffer;
	uint32_t m_nBufferSize;
	uint32_t m_nWritePtr;
	uint32_t m_nTargetPtr;
	bool m_bEOF;
	unsigned __int64 m_qwTime;

	CRITICAL_SECTION m_csTerm;
};

class AudioTest : public app::AppBasic {
public:
	void setup();
	void update();
	void draw();
	void keyDown( KeyEvent event );
	
private:
	//void initAudio();
	//int createSecondaryBuffer( LPDIRECTSOUNDBUFFER *ppBuffer, int channels, int secs, int samplerate, int bitspersample, DWORD flags );
	void fillBuffer( XAUDIO2_BUFFER pBuffer, float frequency, int buffersize, int samplerate );
	//void playBuffer( LPDIRECTSOUNDBUFFER pBuffer );
	//LPDIRECTSOUND8 mPDS;
	//LPDIRECTSOUNDBUFFER mSoundBuf;

	//void findWavChunk( DWORD fourcc, DWORD fourcc, DWORD & dwChunkSize, DWORD & dwChunkDataPosition );
	void decodeMp3();

	WMA * mMp3;
	IXAudio2MasteringVoice * mMasterVoice;

};

WMACallback::WMACallback()
{
	Clear();
}

WMACallback::~WMACallback()
{

}

void WMACallback::Clear()
{
	m_iRefCount = 0;
}

HRESULT WMACallback::QueryInterface( const struct _GUID& guid, void** ppInterface )
{
	if( guid == IID_IWMReaderCallback ) {
		*ppInterface = ( IWMReaderCallback* ) this;
	} else if( guid == IID_IWMReaderCallbackAdvanced ) {
		*ppInterface = ( IWMReaderCallbackAdvanced* ) this;
	} else {
		return E_NOINTERFACE;
	}

	return S_OK;
}

ULONG WMACallback::AddRef()
{
	return 1;
}

ULONG WMACallback::Release()
{
	return 1;
}

HRESULT WMACallback::OnStatus(WMT_STATUS Status,  HRESULT hr, WMT_ATTR_DATATYPE dwType, BYTE* pValue, void* pvContext)
{
	if( ! pvContext ) {
		return E_FAIL;
	}
	WMA* pWMA = static_cast<WMA*>(pvContext);
	return pWMA->OnStatus(Status, hr, dwType, pValue);
}

HRESULT WMACallback::OnSample(DWORD dwOutputNum, QWORD cnsSampleTime, QWORD cnsSampleDuration, DWORD dwFlags, INSSBuffer* pSample, void* pvContext)
{
	if(!pvContext) {
		return E_FAIL;
	}

	WMA* pWMA = static_cast<WMA*>(pvContext);
	return pWMA->OnSample(dwOutputNum, cnsSampleTime, cnsSampleDuration, dwFlags, pSample);
}

HRESULT WMACallback::OnStreamSample(WORD wStreamNum, QWORD cnsSampleTime, QWORD cnsSampleDuration, DWORD dwFalgs, INSSBuffer * pSample, void * pvContext)
{
	return E_NOTIMPL;
}

HRESULT WMACallback::OnTime(QWORD qwCurrentTime, void * pvContext)
{
	if(!pvContext)
		return E_FAIL;

	WMA* pWMA = static_cast<WMA*>(pvContext);
	return pWMA->OnTime( qwCurrentTime );
}

HRESULT WMACallback::OnStreamSelection(WORD wStreamCount, WORD * pStreamNumbers, WMT_STREAM_SELECTION * pSelections, void * pvContext )
{
	return S_OK;
}

HRESULT WMACallback::OnOutputPropsChanged(DWORD dwOutputNum, WM_MEDIA_TYPE * pMediaType, void * pvContext)
{
	return S_OK;
}

HRESULT WMACallback::AllocateForOutput(DWORD dwOutputNum, DWORD cbBuffer, INSSBuffer * * ppBuffer, void * pvContext)
{
	return E_NOTIMPL;
}

HRESULT WMACallback::AllocateForStream(WORD wStreamNum, DWORD cbBuffer, INSSBuffer  **ppBuffer, void  *pvContext)
{
	return E_NOTIMPL;
}

WMA::WMA()
{
	m_pBuffer = 0;
	m_nBufferSize = 0; 
	Clear();
}

WMA::~WMA()
{
	Close();
	if(m_pBuffer) {
		delete [] m_pBuffer;
		m_pBuffer = 0;
	}
	m_nBufferSize = 0;
}

void WMA::Clear()
{
	m_pReader = 0;
	m_pReaderAdvanced = 0;
	m_pReaderAdvanced2 = 0;
	m_pHeaderInfo = 0;
	ZeroMemory(&m_WaveFormatEx, sizeof(WAVEFORMATEX));
	m_hWaitEvent = 0;
    m_hRespondEvent = 0;
	m_bOpen = false;
	m_Callback.Clear();
	m_hrCallbackResult = 0;
	m_nStreamSize = 0;
	m_nBytesRead = 0;
	m_bFirstRead = true;
	m_nTargetPtr = 0;
	m_nWritePtr = 0;
	m_bEOF = false;
	m_qwTime = 0;

	ZeroMemory(&m_csTerm, sizeof(CRITICAL_SECTION));
}

bool WMA::PreOpen()
{
	InitializeCriticalSection( &m_csTerm );

	m_hWaitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hRespondEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	HRESULT hr = WMCreateReader(0, 0, &m_pReader);
	if(FAILED(hr)) {
		return false;
	}

	hr = m_pReader->QueryInterface( IID_IWMReaderAdvanced, (void**)&m_pReaderAdvanced );
	if(FAILED(hr)) {
		return false;
	}

	hr = m_pReader->QueryInterface( IID_IWMReaderAdvanced2, (void**)&m_pReaderAdvanced2 );
	if(FAILED(hr)) {
		return false;
	}

	hr = m_pReader->QueryInterface( IID_IWMHeaderInfo, (void**)&m_pHeaderInfo );
	if(FAILED(hr)) {
		return false;
	}

	return true;
}

/*bool WMA::Open(std::string sFilename)
{
	if(!PreOpen()) {
		return false;
	}

	FILE *f = fopen( sFilename.c_str(), "rb" );
	if( ! f ) {
		return false;
	}
	
	IStreamFile * is = new IStreamFile( f, true );
	fli_istream * fis = new fli_istream( is );
	HRESULT hr = m_pReaderAdvanced2->OpenStream( fis, &m_Callback, this );

	if(FAILED(hr)) {
		return false;
	}

	if(!PostOpen()) {
		return false;
	}

	return true;

}
*/
bool WMA::Open(uint8_t* pdData, uint32_t dwDataSize)
{
	if(!PreOpen()) {
		return false;
	}

	HGLOBAL hMem;
	::IStream* pStream;

	hMem = GlobalAlloc( GMEM_MOVEABLE | GMEM_NODISCARD | GMEM_SHARE, dwDataSize );

	LPVOID pMem = GlobalLock( hMem );
	CopyMemory( pMem, pdData, dwDataSize );
	GlobalUnlock( hMem );

	HRESULT hr = CreateStreamOnHGlobal(hMem, TRUE, &pStream);
	if(FAILED(hr)) {
		return false;
	}

	hr = m_pReaderAdvanced2->OpenStream(pStream, &m_Callback, this);
	if(FAILED(hr)) {
		return false;
	}

	if(!PostOpen()) {
		return false;
	}

	return true;
}

bool WMA::PostOpen()
{
	WaitForSingleObject( m_hRespondEvent, INFINITE );
	if( FAILED(m_hrCallbackResult) ) {
		return false;
	}

	uint32_t nOutputCount;
	HRESULT hr = m_pReader->GetOutputCount( &nOutputCount );

	if( FAILED(hr) ) {
		return false;
	}
	
	//ensure this is audio only
	if( nOutputCount != 1 ) {
		return false;
	}

	uint32_t nOutputFormatCount;
	hr = m_pReader->GetOutputFormatCount(0, &nOutputFormatCount);
	if(FAILED(hr)) {
		return FALSE;
	}

	uint32_t nFormatSize = 0; 
	BYTE* pBuf = 0;
	IWMOutputMediaProps* pProps = 0;
	for( uint32_t j = 0; j < nOutputFormatCount; j++ ) {
		hr = m_pReader->GetOutputFormat( 0, j, &pProps  );
		if(FAILED(hr)) {
			continue;
		}

		//get required size of the media type structure
		uint32_t nNewSize = 0;
		hr = pProps->GetMediaType( NULL, & nNewSize );
		if( FAILED(hr) ) {
			continue;
		}

		if(nNewSize > nFormatSize)
		{
			if( pBuf ) {
				delete [] pBuf;
				pBuf = 0;
			}

			nFormatSize = nNewSize;
			pBuf = new BYTE[nFormatSize];
		}

		WM_MEDIA_TYPE* pType = (WM_MEDIA_TYPE*) pBuf;
		hr = pProps->GetMediaType(pType, & nFormatSize);
		if(FAILED(hr))
			continue;
		

		if(pType->formattype == WMFORMAT_WaveFormatEx)
		{
			memcpy( &m_WaveFormatEx, pType->pbFormat, pType->cbFormat );
			if((m_WaveFormatEx.nChannels == 2) && (m_WaveFormatEx.wBitsPerSample == 16) && (m_WaveFormatEx.nSamplesPerSec == 44100)) {
				break;
			}
		}
		if(pProps) {
			pProps->Release();
			pProps = 0;
		}


	}
	if(pBuf) {
		delete [] pBuf;
		pBuf = 0;
	}

	m_pReader->SetOutputProps( 0, pProps );
	if(FAILED(hr)) {
		return false;
	}

	if(pProps) {
		pProps->Release();
		pProps = 0;
	}
	

	//tells it to read as fast possible
	//hr = m_pReaderAdvanced->SetUserProvidedClock(true);

	if(FAILED(hr)) {
		return false;
	}

	WORD wStreamNum = 0;
	WMT_ATTR_DATATYPE Type;
	QWORD dwDuration = 0;
	WORD wLength = 8;
	hr = m_pHeaderInfo->GetAttributeByName(&wStreamNum, g_wszWMDuration, &Type, (BYTE*)&dwDuration, &wLength);
	if(FAILED(hr)) {
		return false;
	}

	//divide by 10 million to get seconds
	double fTime = double(dwDuration) / 10000000.0f;

	m_nStreamSize = fTime * m_WaveFormatEx.nAvgBytesPerSec * 1.5;
	
	//create a default 1.5 second scratch buffer for seconding streams
	m_pBuffer = new uint8_t[m_WaveFormatEx.nAvgBytesPerSec * 1.5];
	m_nBufferSize = m_WaveFormatEx.nAvgBytesPerSec * 1.5;

	m_nTargetPtr = 0;
	m_nWritePtr = 0;

	m_bOpen = true;

	return true;
}


void WMA::Play()
{
	HRESULT hr;	

	//uint32_t decodedDataSize = GetSize();
	/*uint32_t decodedDataSize = m_WaveFormatEx.nSamplesPerSec * m_WaveFormatEx.wBitsPerSample / 8 * m_WaveFormatEx.nChannels * 6;

	uint32_t readSize = 0;

	BYTE * data1 = new BYTE[decodedDataSize];
	SingleRead( data1, decodedDataSize, &readSize );
	XAUDIO2_BUFFER buffer1 = {0};
	buffer1.AudioBytes = readSize;
	buffer1.Flags = XAUDIO2_END_OF_STREAM;
	buffer1.pAudioData = data1;*/
	
	mMaxBufferSize = 0;
	hr = m_pReaderAdvanced->GetMaxOutputSampleSize( 0, &mMaxBufferSize );

	//mMaxBufferSize = m_WaveFormatEx.nSamplesPerSec * m_WaveFormatEx.wBitsPerSample / 8 * m_WaveFormatEx.nChannels * 2;

	if( FAILED( hr = gXAudio->CreateSourceVoice( &mSourceVoice, &m_WaveFormatEx, 0, XAUDIO2_DEFAULT_FREQ_RATIO, &mVoiceCallback, NULL, NULL ) ))
	{
		std::cout << "Error creating source buffer:" << hr << std::endl;
		return;
	}

	if( FAILED( hr = mSourceVoice->Start( 0, XAUDIO2_COMMIT_NOW ) ) ) {
		std::cout << "Error starting buffer" << std::endl;
		return;
	}
	
	//mDecodedBuffers = new uint8_t[MAX_BUFFER_COUNT * (mMaxBufferSize + DECODING_BUFFER_ERROR)];
	mDecodedBuffers = new uint8_t[MAX_BUFFER_COUNT * mMaxBufferSize];
	mCurrentBuffer = 0;
	mCurrentTime = 0;

	hr = m_pReader->Start(0, 0, 1.0f, this);
	if( FAILED(hr) ) {
		std::cout << "Error starting decoder" << std::endl;
		return;
	}

	/*if( FAILED( hr = mSourceVoice->SubmitSourceBuffer( &buffer1 ) ) ) {
		std::cout << "Error submitting source buffer:" << hr << std::endl;
		return;
	}*/
}

void WMA::seek( uint64_t seekDur ) {
	m_pReader->Pause();
	mCurrentTime += seekDur;
	HRESULT hr = m_pReader->Start( mCurrentTime, 0, 1.0f, this );
	if( FAILED(hr) ) {
		std::cout << "Error restarting after seek" << std::endl;
	}
}

bool WMA::Close()
{
	m_bOpen = false;
	if(m_hWaitEvent) {
		if(m_pReader) {
			m_pReader->Stop();
		}

		SetEvent(m_hWaitEvent);
		EnterCriticalSection(&m_csTerm);
		LeaveCriticalSection(&m_csTerm);
		DeleteCriticalSection(&m_csTerm);
	}
	if(m_hWaitEvent)
	{
		CloseHandle(m_hWaitEvent);
		m_hWaitEvent = 0;
	}
	if(m_hRespondEvent)
	{
		CloseHandle(m_hRespondEvent);
		m_hRespondEvent = 0;
	}

	if(m_pHeaderInfo) {
		m_pHeaderInfo->Release();
		m_pHeaderInfo = 0;
	}

	if(m_pReaderAdvanced2) {
		m_pReaderAdvanced2->Release();
		m_pReaderAdvanced2 = 0;
	}

	if(m_pReaderAdvanced) {
		m_pReaderAdvanced->Release();
		m_pReaderAdvanced = 0;
	}

	if(m_pReader) {
		m_pReader->Release();
		m_pReader = 0;
	}

	if(m_pBuffer) {
		delete [] m_pBuffer;
		m_pBuffer = 0;
	}

	Clear();
	return true;
}

uint32_t WMA::GetSize()
{
	return m_nStreamSize;
}

WAVEFORMATEX * WMA::GetFormat()
{
	return &m_WaveFormatEx;
}

bool WMA::isEOF()
{
	return m_bEOF;
}

void WMA::Destroy()
{
	Close();
}

/*bool WMA::SingleRead(uint8_t* pBuffer, uint32_t dwSizeToRead, uint32_t *pdwSizeRead)
{
	if(!m_bOpen)
		return false;

	m_nTargetPtr = dwSizeToRead - m_nWritePtr;

	//check to see if we have enough extra data in the buffer to satisfy the read without doing any decoding
	if(dwSizeToRead > m_nWritePtr){
		if(m_bFirstRead) {
			//grow the decoding buffer is necessary
			if( dwSizeToRead > m_nBufferSize - DECODING_BUFFER_ERROR ) {
				if(m_pBuffer) {
					delete [] m_pBuffer;
					m_pBuffer = 0;
				}
				m_nBufferSize = dwSizeToRead;
				m_pBuffer = new uint8_t[m_nBufferSize + DECODING_BUFFER_ERROR];
			}

			HRESULT hr;
			hr = m_pReader->Start(0, 0, 1.0f, this);
			m_bFirstRead = false;
			if(FAILED(hr)) {
				return false;
			}
		} else {
			//release the thread do decoding continues
			SetEvent(m_hWaitEvent);
		}

		WaitForSingleObject( m_hRespondEvent, INFINITE );
		if(FAILED(m_hrCallbackResult)) {
			return false;
		}
	}

	//normally there should be more than enough data in the buffer
	if( m_nWritePtr >= dwSizeToRead ) {
		memcpy( pBuffer, m_pBuffer, dwSizeToRead );
		* pdwSizeRead = dwSizeToRead;
		memmove(m_pBuffer, m_pBuffer + dwSizeToRead, m_nWritePtr - dwSizeToRead);
		m_nWritePtr -= dwSizeToRead;
	}
	//But if we're at the end of the file, we have less then requested
	else {
		memcpy( pBuffer, m_pBuffer, m_nWritePtr );
		*pdwSizeRead = m_nWritePtr;
		m_nWritePtr = 0;
	}

	m_nBytesRead += *pdwSizeRead;

	if(m_bEOF) {
		m_pReader->Stop();
	}

	return true;
}*/

HRESULT WMA::OnStatus(WMT_STATUS Status, HRESULT hr, WMT_ATTR_DATATYPE dwType, BYTE* pValue)
{
	m_hrCallbackResult = hr;
	switch(Status)
	{
	case WMT_ERROR:
		SetEvent(m_hWaitEvent);
		SetEvent(m_hRespondEvent);
		std::cout << "WMT Error" << std::endl;
	return E_FAIL;
	case WMT_OPENED:
		std::cout << "Status WMT_OPENED" << std::endl;
		SetEvent(m_hRespondEvent);
	break;
	case WMT_END_OF_FILE:
		std::cout << "Status WMT_END_OF_FILE" << std::endl;
		m_bEOF = true;
		SetEvent(m_hWaitEvent);
		SetEvent(m_hRespondEvent);
	break;
	case WMT_STARTED:
		std::cout << "Status WMT_STARTED" << std::endl;
		/*std::cout << "Advancing deliver time at start" << std::endl;
		m_qwTime += 1000000;
		if(m_pReaderAdvanced) {
			m_pReaderAdvanced->DeliverTime(m_qwTime);
		}*/
	break;
	case WMT_STOPPED:
		std::cout << "Status WMT_STOPPED" << std::endl;
		SetEvent(m_hWaitEvent);
		SetEvent(m_hRespondEvent);
	break;
	};

	return S_OK;
}

HRESULT WMA::OnTime(QWORD qwCurrentTime)
{
	//keep asking for the specific duration of the time stream until EOF
	if( ! m_bEOF ) {
		m_qwTime += 1000000;
		if(m_pReaderAdvanced)
			m_pReaderAdvanced->DeliverTime(m_qwTime);
	}
	return S_OK;
}

HRESULT WMA::OnSample(DWORD dwOutputNum, QWORD cnsSampleTime, QWORD cnsSampleDuration, DWORD dwFlags, INSSBuffer *pSample)
{
	if( !m_bOpen ) {
		if(m_hRespondEvent) {
			SetEvent(m_hRespondEvent);
		}
		return S_OK;
	}

	if( m_bEOF ) {
		SetEvent(m_hRespondEvent);
		return S_OK;
	}

	/*BYTE* pBuf;
	DWORD dwLen;
	if(!pSample)
		return E_FAIL;
	

	HRESULT hr = pSample->GetBuffer(&pBuf);
	if(FAILED(hr))
		return E_FAIL;

	hr = pSample->GetLength(&dwLen);
	if(FAILED(hr)) 
		return E_FAIL;

	//Expand the decoding buffer if it's needed
	if((m_nWritePtr + dwLen) > m_nBufferSize) {
		uint32_t nNewBufferSize = m_nWritePtr + dwLen + DECODING_BUFFER_ERROR;
		uint8_t* pNewBuffer = new uint8_t[nNewBufferSize];
		memcpy( pNewBuffer, m_pBuffer, m_nWritePtr );
		if(m_pBuffer) {
			delete [] m_pBuffer;
			m_pBuffer = 0;
		}

		m_pBuffer = pNewBuffer;
		m_nBufferSize = nNewBufferSize;
	}

	memcpy(m_pBuffer + m_nWritePtr, pBuf, dwLen);
	m_nWritePtr += dwLen;

	if(m_nWritePtr >= m_nTargetPtr)
	{
		SetEvent(m_hRespondEvent);
		EnterCriticalSection(&m_csTerm);
		WaitForSingleObject(m_hWaitEvent, INFINITE);
		LeaveCriticalSection(&m_csTerm); //this should be happening when shit gets poped of the stack probably?
	}*/
	
	BYTE* pBuf;
	//BYTE* leakyBuffer;
	DWORD dwLen;
	if(!pSample)
		return E_FAIL;

	HRESULT hr = pSample->GetBuffer(&pBuf);
	if(FAILED(hr))
		return E_FAIL;

	hr = pSample->GetLength(&dwLen);
	if(FAILED(hr)) 
		return E_FAIL;
	
	//leakyBuffer = new BYTE[dwLen];
	XAUDIO2_VOICE_STATE state;
	for(; ; )
	{
		mSourceVoice->GetState( &state );
		if( state.BuffersQueued < MAX_BUFFER_COUNT ) {
			break;
		}
		//std::cout << "Blocking..." << std::cout;
		WaitForSingleObject( mVoiceCallback.hBufferEndEvent, INFINITE );
	}

	memcpy(&(mDecodedBuffers[mCurrentBuffer * mMaxBufferSize]), pBuf, dwLen);
	//memcpy( leakyBuffer, pBuf, dwLen );
	
	XAUDIO2_BUFFER buffer = {0};
	buffer.AudioBytes = dwLen;
	//buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.Flags = 0;
	//buffer.pAudioData = leakyBuffer;
	buffer.pAudioData = &(mDecodedBuffers[mCurrentBuffer * mMaxBufferSize]);
	mSourceVoice->SubmitSourceBuffer( &buffer );
	
	mCurrentTime = cnsSampleTime + cnsSampleDuration;
	mCurrentBuffer++;
	mCurrentBuffer %= MAX_BUFFER_COUNT;

	//probably not an awesome way to do this, since we're dropping audio -- use a ring buffer?
	//uint32_t copyLen = dwLen;
	//std::cout << dwLen << std::endl;
	//if(m_nWritePtr + copyLen > mMaxBufferSize) {
	//	copyLen = mMaxBufferSize - m_nWritePtr;
	//}

	/*memcpy(&(mDecodedBuffers[mCurrentBuffer * mMaxBufferSize]) + m_nWritePtr, pBuf, copyLen);
	m_nWritePtr += dwLen;

	if( m_nWritePtr >= mMaxBufferSize ) {
		//std::cout << "Starting Buffer Queue" << std::endl;
		
		
		//std::cout << "Actually Queuing Buffer" << std::endl;

		XAUDIO2_BUFFER buffer = {0};
		buffer.AudioBytes = m_nWritePtr;
		//buffer.Flags = XAUDIO2_END_OF_STREAM;
		buffer.Flags = 0;
		buffer.pAudioData = &(mDecodedBuffers[mCurrentBuffer * mMaxBufferSize]);
		mSourceVoice->SubmitSourceBuffer( &buffer );
		
		XAUDIO2_VOICE_STATE state;
		for(; ; )
		{
			mSourceVoice->GetState( &state );
			if( state.BuffersQueued < MAX_BUFFER_COUNT ) {
				break;
			}
			//std::cout << "Blocking..." << std::cout;
			WaitForSingleObject( mVoiceCallback.hBufferEndEvent, INFINITE );
		}
		//std::cout << "Continuing" << std::endl;

		mCurrentBuffer++;
		mCurrentBuffer %= MAX_BUFFER_COUNT;
		m_nWritePtr = 0;
	}*/

	//XAUDIO2_VOICE_STATE state;
	//mSourceVoice->GetState( &state );
	std::cout << cnsSampleTime << std::endl;


	return S_OK;
}

/*BOOL CALLBACK acmDriverEnumCallback( HACMDRIVERID hadid, DWORD dwInstance, DWORD fdwSupport )
{
	AudioTest * app = (AudioTest *)dwInstance;
	if( fdwSupport & ACMDRIVERDETAILS_SUPPORTF_CODEC ) {
		MMRESULT mmr;

		ACMDRIVERDETAILS details;
		details.cbStruct = sizeof( ACMDRIVERDETAILS );
		mmr = acmDriverDetails( hadid, &details, 0 );

		HACMDRIVER driver;
		mmr = acmDriverOpen( &driver, hadid, 0 );

		unsigned int i; 
		for( i = 0; i < details.cFormatTags; i++ ) {
			ACMFORMATTAGDETAILS fmtDetails;
			ZeroMemory( &fmtDetails, sizeof(fmtDetails) );
			fmtDetails.cbStruct = sizeof(ACMFORMATTAGDETAILS);
			fmtDetails.dwFormatTagIndex = i;
			std::cout << details.szLongName << std::cout;
			if( fmtDetails.dwFormatTag == WAVE_FORMAT_MPEGLAYER3 ) {
				std::cout << "MP3 Codec! " <<  details.szLongName << std::cout;
				app->mMp3Drivers++;
			}
		}
	}
	return true;
}*/

void AudioTest::setup()
{
#if defined( FLI_MSW )
	//create console for std::cout calls
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
#endif
	//initAudio();
	//int buffersize = createSecondaryBuffer( &mSoundBuf, 1, 1, 22010, 8, 0 );
	//fillBuffer( mSoundBuf, 440.0, buffersize, 22010 );
	//playBuffer( mSoundBuf );
	

	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	
	WAVEFORMATEX * wfx = 0;

	shared_ptr<IStreamFile> IStream( loadFile( "C:\\audio\\guitar.mp3" ) );
	//shared_ptr<IStreamFile> IStream( loadFile( "C:\\audio\\Buzzer.wav" ) );
	int size = IStream->size();
	BYTE * fileData = new BYTE[size];
	IStream->readData( fileData, size );
	
	mMp3 = NULL;
	mMp3 = new WMA();
	mMp3->Open( fileData, size );

	gXAudio = NULL;

	HRESULT hr;
	if( FAILED( hr = XAudio2Create( &gXAudio, 0, XAUDIO2_DEFAULT_PROCESSOR ) ) ) {
		std::cout << "Error initializing XAudio: " << hr << std::endl;
		return;
	}
	
	mMasterVoice = NULL;
	if( FAILED( hr = gXAudio->CreateMasteringVoice( &mMasterVoice, XAUDIO2_DEFAULT_CHANNELS, XAUDIO2_DEFAULT_SAMPLERATE, 0, 0, NULL ) ) ) {
		std::cout << "Error creating master voice: " << hr << std::endl;
		return;
	}

	mMp3->Play();

	/*XAUDIO2_BUFFER buffer = {0};
	
	buffer.AudioBytes = decodedDataSize;
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.pAudioData = data;*/

	/*int channels = 2;
	int secs = 1;
	int bitspersample = 8;
	int samplerate = 22010;
	int buffersize = samplerate * bitspersample / 8 * channels * secs;

	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = channels;
	wfx.nSamplesPerSec = samplerate;
	wfx.wBitsPerSample = bitspersample;
	wfx.nBlockAlign = (WORD)(wfx.wBitsPerSample / 8 * wfx.nChannels);
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

	BYTE * pDataBuffer = new BYTE[buffersize];

	buffer.AudioBytes = buffersize;
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.pAudioData = pDataBuffer;

	fillBuffer( buffer, 440.0, buffersize, samplerate );*/
}

void AudioTest::decodeMp3()
{
	/*MMRESULT mmr;

	//try to find an MP3 codec
	mMp3Drivers = 0;
	acmDriverEnum( acmDriverEnumCallback, (DWORD_PTR)this, 0 );
	if( mMp3Drivers == 0 ) {
		std::cout << "No MP3 Drivers" << std::endl;
	}*/


}

/*void AudioTest::initAudio()
{
	HRESULT hr;
	std::stringstream err;
	
	//create primary DS interface
	if( FAILED( hr = DirectSoundCreate8( NULL, &mPDS, NULL ) ) ) {
		err << "DirectSoundCreate8 failed: " << DXGetErrorString8(hr) << std::endl;
		throw(err.str());
	}
	
	//set cooperation level
	// According to a note in PortAudio, using GetDesktopWindow()
	// instead of GetForegroundWindow() is supposed to avoid problems
	// that occur when the application's window is not the foreground
	// window.  Also, if the application window closes before the
	// DirectSound buffer, DirectSound can crash.  However, for console
	// applications, no sound was produced when using GetDesktopWindow().
	HWND hWnd = GetForegroundWindow();


	if( FAILED( hr = mPDS->SetCooperativeLevel( hWnd, DSSCL_PRIORITY ) ) ) {
		std::cout << "SetCooperationLevel failed" << DXGetErrorString8(hr) << std::endl;
		return;
	}

	LPDIRECTSOUNDBUFFER pDSBPrimary = NULL;

	DSBUFFERDESC dsbd;
	ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = DSBCAPS_PRIMARYBUFFER;
	dsbd.dwBufferBytes = 0;
	dsbd.lpwfxFormat = NULL;


	if( FAILED( hr = mPDS->CreateSoundBuffer( &dsbd, &pDSBPrimary, NULL ) ) ) {
		std::cout << "CreateSoundBuffer failed:" << DXGetErrorString8(hr) << std::endl;
		return;
	}

	WAVEFORMATEX wfx;
	ZeroMemory( &wfx, sizeof(WAVEFORMATEX) );
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = (WORD)2;
	wfx.nSamplesPerSec = 22050;
	wfx.wBitsPerSample = (WORD)16;
	wfx.nBlockAlign = (WORD)(wfx.wBitsPerSample / 8 * wfx.nChannels);
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

	if( FAILED( hr = pDSBPrimary->SetFormat(&wfx) ) ) {
		std::cout << "pDSBPrimary->SetFormat() failed: " << DXGetErrorString8(hr) << std::endl;
		return;
	}

	if( pDSBPrimary ) {
		pDSBPrimary->Release();
		pDSBPrimary = NULL;
	}
}

int AudioTest::createSecondaryBuffer( LPDIRECTSOUNDBUFFER *ppBuffer, int channels, int secs, int samplerate, int bitspersample, DWORD flags )
{
	HRESULT hr;
	std::stringstream err;

	DSBUFFERDESC dsbd;
	WAVEFORMATEX wfx;

	ZeroMemory( &wfx, sizeof(WAVEFORMATEX) );
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = channels;
	wfx.nSamplesPerSec = samplerate;
	wfx.wBitsPerSample = bitspersample;
	wfx.nBlockAlign = (WORD)(wfx.wBitsPerSample / 8 * wfx.nChannels);
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
	ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = flags;
	dsbd.dwBufferBytes = samplerate * bitspersample / 8 * channels * secs;
	dsbd.guid3DAlgorithm = GUID_NULL;
	dsbd.lpwfxFormat = &wfx;

	if( FAILED( hr = mPDS->CreateSoundBuffer( &dsbd, ppBuffer, NULL ) ) )
	{
		err << "CreateSecondaryBuffer: CreateSoundBuffer failed: " << DXGetErrorString8(hr) << std::endl;
		throw(err.str());
	}
	return(dsbd.dwBufferBytes);
}*/

void AudioTest::fillBuffer( XAUDIO2_BUFFER pBuffer, float frequency, int buffersize, int samplerate )
{
	HRESULT hr;
	std::stringstream err;
	

	for( DWORD q = 0; q < pBuffer.AudioBytes; q++ ) {
		float pos = frequency / (float) samplerate * (float)q;
		//convert remainder to radians
		float r = (pos - floor(pos)) * 2 * M_PI;
		float value = sin(r);
		pBuffer.pAudioData[q] = 127 + (value * 127);
	}
}

/*void AudioTest::playBuffer( LPDIRECTSOUNDBUFFER pBuffer )
{
	pBuffer->Play( 0, 0, DSBPLAY_LOOPING );
}*/

void AudioTest::update()
{
	//mTrack->computeFFT( mFFTBuffer );
	//drawTimeTexture();
}

void AudioTest::draw()
{
	/*glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	if( ! mAudioPlayer->isPlaying() ) return;
	
	float ht = 400.0f;
	for( int i = 0; i < ( FLI_FFTDefaultBufferSize / 2 ); i++ ) {
		float barY = mFFTBuffer[i] / FLI_FFTDefaultBufferSize * ht;
		glBegin( GL_QUADS );
			glColor3f( 255.0f, 255.0f, 0.0f );
			glVertex2f( i * 3, ht );
			glVertex2f( i * 3 + 1, ht );
			glColor3f( 0.0f, 255.0f, 0.0f );
			glVertex2f( i * 3 + 1, ht - barY );
			glVertex2f( i * 3, ht - barY );
		glEnd();
	}*/

}

void AudioTest::keyDown( KeyEvent event ) {
	if( event.getChar() == 'x' ) {
		mMp3->seek( 10000000 );
	} else if( event.getChar() == 'z' ) {
		mMp3->seek( -10000000 );
	}
	/*switch( event.getChar() ) {
		case 'o': {
			/*std::string path = getOpenFilePath("");
			if( ! path.empty() ) {
				delete mAudioPlayer;
				delete mAudioFile;
				mAudioFile = new AudioFile( path.c_str() );
				mAudioPlayer = new AudioPlayer( mAudioFile );
				mAudioPlayer->play();
			}*/
		/*}
		break;
		case 'f':
			setFullScreen( ! isFullScreen() );
		break;
		case 'p':
			playOrPause();
		break;
		case 's':
			stop();
		break;
		case '.':
			adjustVolume( 0.1 );
		break;
		case ',':
			adjustVolume( -0.1 );
		break;
		case 'm':
			mute();
		break;
		case ';':
			adjustBalance( -0.1 );
		break;
		case '\'':
			adjustBalance( 0.1 );
		break;
		case 'l':
			centerBalance();
		break;
		case 't':
			showTime();
		break;
		case 'i':
			//showStatus();
			//printFFT();
			doSpectrumAnalysis();
		break;
		case 'c':
			changeSource();
		break;
	}*/
}

FLI_APP_BASIC( AudioTest )

