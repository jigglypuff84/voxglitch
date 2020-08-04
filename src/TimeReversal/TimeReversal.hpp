struct TimeReversal : Module
{
  float left_audio = 0;
  float right_audio = 0;

  AudioBuffer audio_buffer;

  enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
    AUDIO_INPUT_LEFT,
    AUDIO_INPUT_RIGHT,
    PLAYBACK_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
    AUDIO_OUTPUT_LEFT,
    AUDIO_OUTPUT_RIGHT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	TimeReversal()
	{
    config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
	}

	// Autosave module data.  VCV Rack decides when this should be called.
	json_t *dataToJson() override
	{
		json_t *root = json_object();

		return root;
	}

	// Load module data
	void dataFromJson(json_t *root) override
	{
	}

	void process(const ProcessArgs &args) override
	{
    // Read incoming audio into buffer
    // audio_buffer.push(inputs[AUDIO_INPUT_LEFT].getVoltage(), inputs[AUDIO_INPUT_RIGHT].getVoltage());

  }
};
