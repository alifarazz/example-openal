#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>

#ifdef __linux__
  #include <AL/al.h>
  #include <AL/alc.h>
  #include <AL/alut.h>
#else
  #include <OpenAL/al.h>
  #include <OpenAL/alc.h>
  #include <OpenAL/alut.h>
#endif



#define SAMPLERATE 44100


static void alutReportError()
{
  fprintf (stderr, "> ALUT error: %s\n",
           alutGetErrorString(alutGetError()));
  return ;
}

static void alReportError(const char * errorString)
{
	ALenum error;
	if (AL_NO_ERROR != (error = alGetError())) {
		fprintf(stderr, "> %s: %s\n", errorString, alGetString(error));
	}
	return ;
}

static void alcReportError(ALCdevice *device, const char * errorString)
{
	ALenum error;
	if (AL_NO_ERROR != (error = alcGetError(device))) {
		fprintf(stderr, "> %s: %s\n", errorString, alcGetString(device, error));
	}
	return ;
}

int main(int argc, char *argv[])
{
	ALuint        alDuration;
	ALuint        alSource;
	ALuint        alResolution;
	ALuint        alBuffer;
	/* ALsizei       alBufferLen; */
	ALuint        alTotalSamples;
	ALCint        alcNCapturedSampales;
	ALCenum       alcFormat;
	ALCuint       alcFreq;
	ALCsizei      alcRingBufferLen;
	ALCsizei      alcSampleSetLen;
	ALCbyte      *alcSampleSet = NULL;
	ALCcontext   *context;
	ALCdevice    *playbackDevice;
	ALCdevice    *captureDevice;

	alDuration = 1;
	alResolution = 1; /* bytes per sample */
	alcFormat = AL_FORMAT_MONO8;

    /**************************************************/
	/* AL format:        Resolution:				  */
	/* 8  bit  mono   -> 1 byte  per sample			  */
	/* 16 bit  mono   -> 2 bytes per sample			  */
	/* 8  bit  stereo -> 2 bytes per sample	          */
    /* 16 bit  stereo -> 4 bytes per sample			  */
	/**************************************************/

	/* Magic Formula, Don't Touch */
	alcFreq = SAMPLERATE;
	alTotalSamples = alcFreq * alDuration * alResolution;
	alcRingBufferLen = alTotalSamples * 2; /* EDIT */

	printf("playback: %s\n",
		   alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER));
	printf("capture:  %s\n",
		   alcGetString(NULL, ALC_CAPTURE_DEFAULT_DEVICE_SPECIFIER));

	/***********************/
	/* DEVICE SETUP    	   */
	/***********************/

	if (NULL == (playbackDevice = alcOpenDevice(
					 NULL/*, ALC_DEFAULT_DEVICE_SPECIFIER*/)))
		alcReportError(NULL, "Could not Open Playback Device");

	if (NULL == (captureDevice = alcCaptureOpenDevice(
					 NULL, alcFreq, alcFormat, alcRingBufferLen)))
		alcReportError(NULL, "Could not Open Capture Device");

	if (NULL == (context = alcCreateContext(playbackDevice, NULL)))
		alcReportError(playbackDevice, "Could not Create an OpenAL context");

	if (ALC_TRUE != alcMakeContextCurrent(context))
		alcReportError(playbackDevice, "Could not Switch to context");

	/* No Need for OpenAL 3D Spatialization. */
	alDistanceModel(AL_NONE);

	/* Clear Error Code */
	alGetError();
	alcGetError(playbackDevice);
	alcGetError(captureDevice);

	
	if (!alutInit (&argc, argv))
		alutReportError();
	/******************/
	/* CAPTURE    	  */
	/******************/

	    /******************************************/
	    /* TODO:								  */
	    /* 	add alutSleep(ALfloat duration)		  */
     	/******************************************/

	alcCaptureStart(captureDevice);
	do {
		/* alutSleep(0.2); */
		alcGetIntegerv(captureDevice, ALC_CAPTURE_SAMPLES,
				   sizeof(alcNCapturedSampales), &alcNCapturedSampales);
		/* printf("%d \n", alcNCapturedSampales); */

	} while (alcNCapturedSampales < alTotalSamples); /* EDIT (ALCint < ALuint */
	alcCaptureStop(captureDevice);
	alcSampleSetLen = alcNCapturedSampales; /* ALCsizei <- ALCint */

	alcSampleSet = (ALCbyte *) malloc(sizeof(ALCbyte) * alcSampleSetLen);
	/* Retrieve Samples and Pour alBufferLen of Them Into alBuffer*/
	alcCaptureSamples(captureDevice, alcSampleSet, alcSampleSetLen);

	puts("capture stops");
	
	/*******************/
	/* PLAYBACK	       */
	/*******************/

	alGenSources(1, &alSource);
	alGenBuffers(1, &alBuffer);

	/* Pour Retrieved Samples to The Generated Buffer, 3rd arg is const */
	/* 4th arg(ALsizei <- ALCsizei), 5th arg(ALsizei <- ALCuint) */
	alBufferData(alBuffer, alcFormat, alcSampleSet, alcSampleSetLen, alcFreq);
	alSourcei(alSource, AL_BUFFER, alBuffer); /* 3rd arg(ALint <- ALuint) */

	alSourcePlay(alSource);
	alutSleep(alDuration);
 	alSourceStop(alSource);



	/****************************/
	/* Error Checking           */
	/****************************/

	puts("Programme reached end, Here are The Error Reports:");
	alReportError("alGeterror: ");
	alcReportError(playbackDevice, "alcGeterror: ");
	alcReportError(captureDevice, "alcGeterror: ");
	puts("End of Error Reports");


	/*********************/
	/* Clean UP!     	 */
	/*********************/

	free(alcSampleSet);

	alDeleteSources(1, &alSource);
	alDeleteBuffers(1, &alBuffer);

	if (!alutExit())
		alutReportError();
	context = alcGetCurrentContext();
	playbackDevice = alcGetContextsDevice(context);
	alcMakeContextCurrent(NULL);
	alcDestroyContext(context);
	alcCloseDevice(playbackDevice);
	alcCloseDevice(captureDevice);

	alReportError("alGetError: ");

	return 0;
}

/***************************************************************************/
/* TODO:                                                                   */
/*      /\********************************************************\/       */
/*      /\* correct variable names and types                   	 *\/       */
/*      /\* encapsulate in functions                             *\/       */
/*      /\* fix-up record loop                                   *\/       */
/*      /\* fix-up playback sleep                                *\/       */
/*      /\* fix "Clean UP!" block                                *\/       */
/*      /\********************************************************\/       */
/***************************************************************************/
