#pragma once

#include "dr_wav.h"
#define MAX_BUFFER_SIZE 176400

struct AudioBuffer
{
  int read_head = 0;
  int write_head = 0;

	float leftPlayBuffer[MAX_BUFFER_SIZE];
	float rightPlayBuffer[MAX_BUFFER_SIZE];
  bool frozen = false;
  int sample_position = 0;

	AudioBuffer()
	{
	}

	virtual ~AudioBuffer() {}

	virtual void push(float left_audio, float right_audio)
	{
    write_head++;
    if(write_head >= MAX_BUFFER_SIZE) write_head = 0;

    read_head = write_head;

    if(! frozen)
    {
      leftPlayBuffer[write_head] = left_audio;
      rightPlayBuffer[write_head] = right_audio;
    }
	};

  std::pair<float, float> getStereoOutput(unsigned int sample_position)
  {
    unsigned int index = (sample_position + read_head) % MAX_BUFFER_SIZE;
    return {leftPlayBuffer[index], rightPlayBuffer[index]};
  }

  unsigned int getBufferSize()
  {
    return(MAX_BUFFER_SIZE);
  }
};
