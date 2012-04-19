#include "StdAfx.h"
#include "SoundManager.h"

//sets up variables for a new soundmanager
SoundManager::SoundManager(void)
{
	int const MAX_NUM_SONGS = 10; // The Maximum number of songs we can handle
	pXAudio2 = NULL;
	pMasterVoice = NULL;
	currentSourceVoice = NULL;
	numOfFiles = 0;

	buffer = new XAUDIO2_BUFFER[MAX_NUM_SONGS];
	pSourceVoice = new IXAudio2SourceVoice*[MAX_NUM_SONGS];
	songsLoaded = new char*[MAX_NUM_SONGS];
	isSongLooping = false;
	isPaused = false;
	Init();
	SetVolume(0.1f); // Start our starting volume level
}

//intitializes the sound so it can be 
HRESULT SoundManager::Init(void)
{
	HRESULT hr;

	// Create an instance of the XAudio2 engine
	if ( FAILED(hr = XAudio2Create( &pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR ) ) )
		return hr;

	// The mastering voices encapsulates an audio device, is the final destination for all audio that passes through an audio graph.
	if ( FAILED(hr = pXAudio2->CreateMasteringVoice( &pMasterVoice, XAUDIO2_DEFAULT_CHANNELS,
         XAUDIO2_DEFAULT_SAMPLERATE, 0, 0, NULL ) ) )
		return hr;

	return 0;
}

// Helper Method from Microsoft MSDN that helps us load and buffer a music file
HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD & dwChunkSize, DWORD & dwChunkDataPosition)
{
    HRESULT hr = S_OK;
    if( INVALID_SET_FILE_POINTER == SetFilePointer( hFile, 0, NULL, FILE_BEGIN ) )
        return HRESULT_FROM_WIN32( GetLastError() );

    DWORD dwChunkType;
    DWORD dwChunkDataSize;
    DWORD dwRIFFDataSize = 0;
    DWORD dwFileType;
    DWORD bytesRead = 0;
    DWORD dwOffset = 0;

    while (hr == S_OK)
    {
        DWORD dwRead;
        if( 0 == ReadFile( hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL ) )
            hr = HRESULT_FROM_WIN32( GetLastError() );

        if( 0 == ReadFile( hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL ) )
            hr = HRESULT_FROM_WIN32( GetLastError() );

        switch (dwChunkType)
        {
        case fourccRIFF:
            dwRIFFDataSize = dwChunkDataSize;
            dwChunkDataSize = 4;
            if( 0 == ReadFile( hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL ) )
                hr = HRESULT_FROM_WIN32( GetLastError() );
            break;

        default:
            if( INVALID_SET_FILE_POINTER == SetFilePointer( hFile, dwChunkDataSize, NULL, FILE_CURRENT ) )
            return HRESULT_FROM_WIN32( GetLastError() );            
        }

        dwOffset += sizeof(DWORD) * 2;
        
        if (dwChunkType == fourcc)
        {
            dwChunkSize = dwChunkDataSize;
            dwChunkDataPosition = dwOffset;
            return S_OK;
        }

        dwOffset += dwChunkDataSize;
        
        if (bytesRead >= dwRIFFDataSize) return S_FALSE;

    }

    
    return S_OK;
    
}

// Helper Method from Microsoft MSDN that helps us load and buffer a music file
HRESULT ReadChunkData(HANDLE hFile, void * buffer, DWORD buffersize, DWORD bufferoffset)
{
    HRESULT hr = S_OK;
    if( INVALID_SET_FILE_POINTER == SetFilePointer( hFile, bufferoffset, NULL, FILE_BEGIN ) )
        return HRESULT_FROM_WIN32( GetLastError() );
    DWORD dwRead;
    if( 0 == ReadFile( hFile, buffer, buffersize, &dwRead, NULL ) )
        hr = HRESULT_FROM_WIN32( GetLastError() );
    return hr;
}

//speaks for itself
HRESULT SoundManager::LoadSoundFile(char* fn)
{	
	// Convert char to TCHAR
	TCHAR* fileName = charToTCHAR(fn);
	// Add it to the array of our song names
	songsLoaded[numOfFiles] = fn;

	// Declare our structures
	WAVEFORMATEXTENSIBLE wf = {0};
	XAUDIO2_BUFFER buff = {0};
	wfx = wf;
	buffer[numOfFiles] = buff;
	
	// Which music file we want to play
	TCHAR * strFileName = fileName;

	// Open the file
	HANDLE hFile = CreateFile(strFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

	if( INVALID_HANDLE_VALUE == hFile )
		return HRESULT_FROM_WIN32( GetLastError() );

	if( INVALID_SET_FILE_POINTER == SetFilePointer( hFile, 0, NULL, FILE_BEGIN ) )
		return HRESULT_FROM_WIN32( GetLastError() );

	// Locate the 'RIFF' chunk in the audio file, and check the file type.
	DWORD dwChunkSize;
	DWORD dwChunkPosition;

	//check the file type, should be fourccWAVE or 'XWMA'
	FindChunk(hFile,fourccRIFF,dwChunkSize, dwChunkPosition );
	DWORD filetype;

	ReadChunkData(hFile,&filetype,sizeof(DWORD),dwChunkPosition);

	if (filetype != fourccWAVE)
		return S_FALSE;

	// Locate the 'fmt ' chunk, and copy its contents into a WAVEFORMATEXTENSIBLE structure.
	FindChunk(hFile,fourccFMT, dwChunkSize, dwChunkPosition );
	ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition );

	//fill out the audio data buffer with the contents of the fourccDATA chunk
	FindChunk(hFile,fourccDATA,dwChunkSize, dwChunkPosition );
	BYTE * pDataBuffer = new BYTE[dwChunkSize];
	ReadChunkData(hFile, pDataBuffer, dwChunkSize, dwChunkPosition);

	buffer[numOfFiles].AudioBytes = dwChunkSize;  //buffer containing audio data
	buffer[numOfFiles].pAudioData = pDataBuffer;  //size of the audio buffer in bytes
	buffer[numOfFiles].Flags = XAUDIO2_END_OF_STREAM; // tell the source voice not to expect any data after this buffer

	// Create a source voice
	HRESULT hr;
	if( FAILED(hr = pXAudio2->CreateSourceVoice( &pSourceVoice[numOfFiles], (WAVEFORMATEX*)&wfx,
              0, XAUDIO2_DEFAULT_FREQ_RATIO, NULL, NULL, NULL ) ) ) return hr;

	if( FAILED(hr = pSourceVoice[numOfFiles]->SubmitSourceBuffer( &buffer[numOfFiles] ) ) )
		return hr;

	numOfFiles++;

	return 0;
}

// Play the Sound File identified by name and path
HRESULT SoundManager::PlaySoundFile(char* songName)
{
	int songNum = -1;
	// look for the song in out list of songs
	for(int i = 0; i < numOfFiles; i++)
	{
		char* fileName = songsLoaded[i];
		if(strcmp(songName, fileName) == 0)
		{
			songNum = i;
			break;
		}
	}
	
	// Could not find the song so exit out of the method
	if(songNum == -1)
		return -1;

	PlaySoundFile(songNum);

	return 0;
}

// Play the Sound File identified by number loaded
HRESULT SoundManager::PlaySoundFile(int songNum)
{
	// Make sure we have entered a valid number
	if(songNum > numOfFiles || songNum < 0)
		return -1;
	
	HRESULT hr;
	if ( FAILED(hr = pSourceVoice[songNum]->Start( 0, XAUDIO2_COMMIT_NOW ) ) )
		return hr;
	currentSourceVoice = pSourceVoice[songNum];
	return 0;
}

// Will the stop the sound that was most recently played
HRESULT SoundManager::StopCurrentSound(void)
{
	HRESULT hr;
	if( currentSourceVoice != NULL)
	{
		if( FAILED(hr = currentSourceVoice->Stop( 0, XAUDIO2_COMMIT_NOW ) ) )
			return hr;
	}

	return 0;
}

// Will toggle pause on or off for the sound that was most recently played
HRESULT SoundManager::PauseCurrentSound(void)
{
	if( currentSourceVoice != NULL)
	{
		if(isPaused) // was paused, not start from same spot
		{
			currentSourceVoice->Start(0);
			isPaused = false;
		}
		else // pause the song ar current spot
		{
			currentSourceVoice->Stop(0);
			isPaused = true;
		}
	}
	
	return 0;
}

// Pass in the name of the song you want to loop, method must be checked every frame
HRESULT SoundManager::LoopSound(char* songName)
{
	int songNum = -1;
	// look for the song in out list of songs
	for(int i = 0; i < numOfFiles; i++)
	{
		char* fileName = songsLoaded[i];
		if(strcmp(songName, fileName) == 0)
		{
			songNum = i;
			break;
		}
	}
	
	// Could not find the song so exit out of the method
	if(songNum == -1)
		return -1;

	LoopSound(songNum);

	return 0;
}

// Pass in the number of the song you want to loop, method must be checked every frame
HRESULT SoundManager::LoopSound(int songNum)
{
	HRESULT hr;

	// find the state of the source voice that is playing our song
	if(pSourceVoice != NULL)
	{
		XAUDIO2_VOICE_STATE state;
		pSourceVoice[songNum]->GetState( &state );

		// if the song isn't looping, play it
		if(!isSongLooping)
		{
			if( FAILED(hr = PlaySoundFile(songNum)))
				return hr;
			// buffer the same song to be played after this one is done
			loopingSourceVoice = pSourceVoice[songNum];
			loopingSourceVoice->SubmitSourceBuffer(&buffer[songNum]);
			isSongLooping = true;
		}
		else if(state.BuffersQueued == NULL) // a playthrough has finished, we are no longer looping
		{
			isSongLooping = false;
			pSourceVoice[songNum]->Stop(0, XAUDIO2_COMMIT_NOW);
		}
	}
	return 0;
}

// lets us convert from a char to a tchar
TCHAR* SoundManager::charToTCHAR(char* myString)
{
	char* pchString = myString;

	// Find out required string size
	int iRequiredSize = ::MultiByteToWideChar(CP_ACP, NULL, pchString, -1, NULL, 0);

	// Alloc buffer for converted string
	TCHAR* pwchString = new TCHAR[iRequiredSize];

	// Do the string conversion
	MultiByteToWideChar(CP_ACP, NULL, pchString , -1, pwchString, iRequiredSize);

	return pwchString;
}

// Lets us increase of decrease the volume depending on what the param is, -1.0f to 1.0f
void SoundManager::ChangeVolume(float num)
{
	// validate our input
	if(num == 0.0f)
		return;
	else if(num > 1.0f)
		num = 1.0f;
	else if(num < -1.0f)
		num = -1.0f;

	// find the current volume level
	float currentVolume; 
	pMasterVoice->GetVolume(&currentVolume);

	// add our change to it
	currentVolume += num;

	// set the volume to our new value
	SetVolume(currentVolume);
}

// Lets us set the volume, values are from 0.0 - 1.0, with 1.0 being loudest and 0.0 being mute
void SoundManager::SetVolume(float num)
{
	// Make sure it is a valid value
	if(num > 1.0f)
		num = 1.0f;
	else if(num < 0.0f)
		num = 0.0f;

	pMasterVoice->SetVolume(num);
}

bool SoundManager::GetIsPaused(void)
{
	return isPaused;
}

//clears out buffers and releases anything that was used
SoundManager::~SoundManager(void)
{
	//loopingSourceVoice->Stop(0, XAUDIO2_COMMIT_NOW); // Stop our looping song
	buffer = NULL; // array of buffers, one used for each song
	pSourceVoice = NULL; // array of source voices for each song buffer
	loopingSourceVoice = NULL;
	pXAudio2->StopEngine();
	pXAudio2->Release();
	//pMasterVoice->DestroyVoice();
}
