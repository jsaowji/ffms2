
#ifndef FFAUDIOVAPOURSOURCES_H
#define FFAUDIOVAPOURSOURCES_H

#include "VapourSynth4.h"
#include "ffms.h"

struct VSAudioSource4 {
private:
  VSAudioInfo audioInfo;
  FFMS_AudioSource* audiosrc;
  uint8_t Buf[VS_AUDIO_FRAME_SAMPLES * 32 * 6];
  
  FFMS_ErrorInfo errinfo;
  char errmsg[1024];
  
public:
  VSAudioSource4(const char *SourceFile, int Track, int DelayMode, FFMS_Index *Index, const VSAPI *vsapi, VSCore *core);
  ~VSAudioSource4();

  static const VSFrame* GetFrame(int n, int activationReason, void *instanceData, void **frameData, VSFrameContext *frameCtx, VSCore *core, const VSAPI *vsapi);
  static void Free(void *instanceData, VSCore *core, const VSAPI *vsapi);

  const VSAudioInfo* GetAudioInfo() { return &audioInfo; }
};


#endif
