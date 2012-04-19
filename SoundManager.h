#pragma once
#include <windows.h>
#include <xaudio2.h>

#ifdef _XBOX //Big-Endian
#define fourccRIFF 'RIFF'
#define fourccDATA 'data'
#define fourccFMT 'fmt '
#define fourccWAVE 'WAVE'
#define fourccXWMA 'XWMA'
#define fourccDPDS 'dpds'
#endif

#ifndef _XBOX //Little-Endian
#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'
#endif

class SoundManager
{
private:
	IXAudio2* pXAudio2;
	IXAudio2MasteringVoice* pMasterVoice;
	IXAudio2SourceVoice** pSourceVoice; // array of source voices for each song buffer
	IXAudio2SourceVoice* currentSourceVoice; // which song we played last
	IXAudio2SourceVoice* loopingSourceVoice;
	char** songsLoaded; // array of fileNames for songs loaded
	WAVEFORMATEXTENSIBLE wfx;
	XAUDIO2_BUFFER* buffer; // array of buffers, one used for each song
	int numOfFiles; // the total number of music files we have loaded
	bool isSongLooping; // lets us know if a song is looping or not, NOTE: Only one song can be looped at a time
	bool isPaused; // lets us know if a song has been paused or not

	HRESULT Init(void);
	TCHAR* charToTCHAR(char* string);
	void SetVolume(float num);
public:
	SoundManager(void);
	~SoundManager(void);
	HRESULT LoadSoundFile(char* fileName);
	HRESULT PlaySoundFile(char* songName);
	HRESULT PlaySoundFile(int songNum);
	HRESULT LoopSound(char* songName);
	HRESULT LoopSound(int songNum);
	HRESULT StopCurrentSound(void);
	HRESULT PauseCurrentSound(void);
	void ChangeVolume(float num);
	bool GetIsPaused();
};

