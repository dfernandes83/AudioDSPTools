//
//  ImpulseResponse.h
//  NeuralAmpModeler-macOS
//
//  Created by Steven Atkinson on 12/30/22.
//
// Impulse response processing

#pragma once

#include <filesystem>

#include <Eigen/Dense>

#include "dsp.h"
#include "wav.h"

namespace dsp
{
class ImpulseResponse : public History
{
public:
  struct IRData;
  ImpulseResponse(const char* fileName, const double sampleRate);
  ImpulseResponse(const IRData& irData, const double sampleRate);
  double** Process(double** inputs, const size_t numChannels, const size_t numFrames) override;
  IRData GetData();
  double GetSampleRate() const { return mSampleRate; };
  // TODO states for the IR class
  dsp::wav::LoadReturnCode GetWavState() const { return this->mWavState; };

private:
  // Set the weights, given that the plugin is running at the provided sample
  // rate.
  void _SetWeights();

  // State of audio
  dsp::wav::LoadReturnCode mWavState;
  // Keep a copy of the raw audio that was loaded so that it can be resampled
  std::vector<float> mRawAudio;
  double mRawAudioSampleRate;
  // This class's own Process()/_SetWeights() stay mono-only regardless (channel 1+ just copies
  // channel 0's output, see Process()) -- kept only so GetData() can round-trip a stereo source's
  // channel count honestly for a caller that reads it back out without ever calling Process() here.
  int mRawNumChannels = 1;
  // Resampled to the required sample rate.
  std::vector<float> mResampled;
  double mSampleRate;

  const size_t mMaxLength = 8192;
  // The weights
  Eigen::VectorXf mWeight;
};

struct dsp::ImpulseResponse::IRData
{
  std::vector<float> mRawAudio;
  double mRawAudioSampleRate;
  // 1 = mono, 2 = true stereo (mRawAudio interleaved L,R,L,R,...). Default 1 for source
  // compatibility with any code that never sets it. dsp::wav::Load() never returns anything else.
  int mNumChannels = 1;
};

}; // namespace dsp
