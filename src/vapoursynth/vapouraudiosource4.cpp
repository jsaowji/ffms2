#include "vapouraudiosource4.h"
#include "ffms.h"
#include <string>
#include <stdexcept>

const VSFrame* VSAudioSource4::GetFrame(int n, int activationReason, void *instanceData, void **frameData, VSFrameContext *frameCtx, VSCore *core, const VSAPI *vsapi){
  VSAudioSource4* userData = (VSAudioSource4*)instanceData;
  auto sample_n = n * VS_AUDIO_FRAME_SAMPLES;
  auto sample_cnt = VS_AUDIO_FRAME_SAMPLES;

  int abc = userData->audioInfo.numSamples - sample_n;
  if ( sample_cnt >= abc) sample_cnt = abc;
  
  auto vsframe = vsapi->newAudioFrame(&userData->audioInfo.format,sample_cnt, 0,core);
  FFMS_GetAudio(userData->audiosrc,userData->Buf,sample_n,sample_cnt,&userData->errinfo);

  auto format = userData->audioInfo.format;
  for(int c = 0; c < format.numChannels; c++) {
    uint8_t *output = vsapi->getWritePtr(vsframe, c);
    for(int s = 0; s < sample_cnt; s++ ){
      for( int b = 0; b < format.bytesPerSample; b++) {
        output[s * format.bytesPerSample + b] = userData->Buf[s * format.bytesPerSample * format.numChannels + c *  format.bytesPerSample +b];
      }
    }
  }
  
  return vsframe;
}

void VSAudioSource4::Free(void *instanceData, VSCore *core, const VSAPI *vsapi) {
  delete (VSAudioSource4*)instanceData;
}

VSAudioSource4::~VSAudioSource4() {
  FFMS_DestroyAudioSource(audiosrc);
}

VSAudioSource4::VSAudioSource4(const char *SourceFile, int Track, int DelayMode, FFMS_Index *Index, const VSAPI *vsapi, VSCore *core) {
  errinfo.Buffer = errmsg;
  errinfo.BufferSize = sizeof(errmsg);

  auto audiosource = FFMS_CreateAudioSource(SourceFile, Track, Index, DelayMode, &errinfo);
  if (audiosource == NULL) {
        throw std::runtime_error(std::string("Source: ") + errinfo.Buffer);

    return;
  }
  auto props = FFMS_GetAudioProperties(audiosource);

  auto bitsPerSample = props->BitsPerSample;

  auto vsaudioInfo = VSAudioInfo{
    .format = {
      .sampleType = props->SampleFormat == FFMS_FMT_FLT ? stFloat : stInteger,
      .bitsPerSample = bitsPerSample,
      .bytesPerSample = (bitsPerSample + 7) / 8, /* implicit from bitsPerSample */
      .numChannels = props->Channels, /* implicit from channelLayout */
      .channelLayout = static_cast<uint64_t>(props->ChannelLayout),
    },
    .sampleRate = props->SampleRate,
    .numSamples = props->NumSamples,
    .numFrames = static_cast<int>((props->NumSamples + VS_AUDIO_FRAME_SAMPLES-1) / VS_AUDIO_FRAME_SAMPLES),
  };

  int numDeps = 0;

  audioInfo = vsaudioInfo;
  audiosrc = audiosource;
}
