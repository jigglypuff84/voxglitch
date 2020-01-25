//
// Voxglitch "Ghosts" module for VCV Rack
//


#include "plugin.hpp"
#include "osdialog.h"
#include "dr_wav.h"
#include <vector>
#include "cmath"
#include <memory>

using namespace std;
#define MAX_BUFFER_SIZE 192000
#define MAX_NUMBER_OF_GRAINS 30
#define MAX_SPAWN_RATE 48000.0f

struct CircularBuffer
{
	float buffer[MAX_BUFFER_SIZE];
	unsigned int start_index = 0;
	unsigned int end_index = 0;
	unsigned int max_index = MAX_BUFFER_SIZE - 1;

	void append(float value)
	{
		buffer[end_index] = value;
	}

	void step()
	{
		start_index = end_index;
		end_index ++;
		if(end_index > max_index) end_index = 0;
	}

	float read(unsigned int index)
	{
		index = index % MAX_BUFFER_SIZE;
		return(buffer[index]);
	}

	unsigned int getStartIndex()
	{
		return(start_index);
	}

	unsigned int getEndIndex()
	{
		return(end_index);
	}
};

struct EvpGrain
{
	// Playback length for the ghost, measuring in .. er.. ticks?
	unsigned int playback_length = 0;
	unsigned int playback_position = 0;
	unsigned int playback_start_position = 0;
	// unsigned int buffer_playback_position = 0;

	// circular_buffer points to the audio stored in the circular buffer
	CircularBuffer *circular_buffer;

	EvpGrain(CircularBuffer *circular_buffer, unsigned int playback_length, unsigned int playback_start_position)
	{
		this->circular_buffer = circular_buffer;
		this->playback_length = playback_length;
		this->playback_start_position = playback_position;
	}

	float getOutput()
	{
		return(this->circular_buffer->read(this->playback_start_position + this->playback_position));
	}

	void step()
	{
        // Step the playback position forward.
		playback_position = playback_position + 1;

        // If the playback position is past the playback length, then wrap the playback position to the beginning
		if(playback_position >= playback_length) playback_position = playback_position % playback_length;

		// If the playback position is less than 0, it's possible that we're playing the buffer backwards,
		// so wrap around the end of the sample.
		// if (playback_position < 0) playback_position = playback_length - playback_position;
	}
};

struct Evp : Module
{
	CircularBuffer circular_buffer;
	vector<EvpGrain> grain_pool;
	unsigned int spawn_rate_counter = 0;

	enum ParamIds {
		GRAIN_LENGTH_KNOB,
		GRAIN_LENGTH_ATTN_KNOB,
		SPAWN_RATE_KNOB,
		SPAWN_RATE_ATTN_KNOB,
		PLAYBACK_POSITION_KNOB,
		NUM_PARAMS
	};
	enum InputIds {
        AUDIO_INPUT,
		SPAWN_RATE_INPUT,
		GRAIN_LENGTH_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		AUDIO_OUTPUT,
		DEBUG_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	//
	// Constructor
	//
	Evp()
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(PLAYBACK_POSITION_KNOB, 0.0f, 1.0f, 0.0f, "PlaybackPositionKnob");
	}

	json_t *dataToJson() override
	{
		json_t *rootJ = json_object();
		// json_object_set_new(rootJ, "path", json_string(sample.path.c_str()));
		return rootJ;
	}

	void dataFromJson(json_t *rootJ) override
	{
	}

	float calculate_inputs(int input_index, int knob_index, int attenuator_index, float scale)
	{
		float input_value = inputs[input_index].getVoltage() / 10.0;
		float knob_value = params[knob_index].getValue();
		float attenuator_value = params[attenuator_index].getValue();

		return(((input_value * scale) * attenuator_value) + (knob_value * scale));
	}

	void process(const ProcessArgs &args) override
	{
        float audio_input_value = inputs[AUDIO_INPUT].getVoltage();
		circular_buffer.append(audio_input_value);

		unsigned int spawn_rate = calculate_inputs(SPAWN_RATE_INPUT, SPAWN_RATE_KNOB, SPAWN_RATE_ATTN_KNOB, MAX_SPAWN_RATE);
		unsigned int grain_length = calculate_inputs(GRAIN_LENGTH_INPUT, GRAIN_LENGTH_KNOB, GRAIN_LENGTH_ATTN_KNOB, args.sampleRate);

		unsigned int playback_position = inputs[PLAYBACK_POSITION_KNOB].getVoltage() * (float)(MAX_BUFFER_SIZE - 1);

		if((spawn_rate_counter >= spawn_rate) && (grain_length > 0))
		{
			EvpGrain grain(&circular_buffer, grain_length, playback_position);
			grain_pool.push_back(grain);
			spawn_rate_counter = 0;
		}

		while(grain_pool.size() > MAX_NUMBER_OF_GRAINS)
		{
            grain_pool.erase(grain_pool.begin());
		}

		float mix_output = 0;

		for(EvpGrain& grain : grain_pool)
		{
			mix_output += grain.getOutput();
			grain.step();
		}

		outputs[DEBUG_OUTPUT].setVoltage(grain_pool.size());
        outputs[AUDIO_OUTPUT].setVoltage(mix_output);

		circular_buffer.step();
		spawn_rate_counter = spawn_rate_counter + 1;
	}
};


struct EvpWidget : ModuleWidget
{
	EvpWidget(Evp* module)
	{
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/evp_front_panel.svg")));

        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(20, 100)), module, Evp::AUDIO_INPUT));

		// Length
		addParam(createParamCentered<RoundLargeBlackKnob>(mm2px(Vec(44, 68)), module, Evp::GRAIN_LENGTH_KNOB));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10, 68)), module, Evp::GRAIN_LENGTH_INPUT));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(26, 68)), module, Evp::GRAIN_LENGTH_ATTN_KNOB));

		// Spawn rate
		addParam(createParamCentered<RoundLargeBlackKnob>(mm2px(Vec(44, 90)), module, Evp::SPAWN_RATE_KNOB));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10, 90)), module, Evp::SPAWN_RATE_INPUT));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(26, 90)), module, Evp::SPAWN_RATE_ATTN_KNOB));

		addParam(createParamCentered<RoundHugeBlackKnob>(mm2px(Vec(35, 33)), module, Evp::PLAYBACK_POSITION_KNOB));

		// WAV output
		addOutput(createOutput<PJ301MPort>(Vec(200, 324), module, Evp::AUDIO_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(34.236, 124.893)), module, Evp::DEBUG_OUTPUT));
	}
};


Model* modelEvp = createModel<Evp, EvpWidget>("evp");
