//
// Voxglitch "Equation Composer" module for VCV Rack
//
// This is a port of my Equation Composer hardware synth the myself, John
// Staskavich, and Hannes Pasqualini built under the company name Microbe Modular


#include "plugin.hpp"
#include "osdialog.h"
#include "dr_wav.h"
#include <vector>
#include "cmath"
#include <memory>

using namespace std;

struct EquationComposer : Module
{
	uint32_t t;
	uint16_t w;

	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		NUM_INPUTS
	};
	enum OutputIds {
		EQUATION_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	//
	// Constructor
	//
	EquationComposer()
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
	}

	json_t *dataToJson() override
	{
		json_t *rootJ = json_object();
		// json_object_set_new(rootJ, "path", json_string(this->path.c_str()));
		return rootJ;
	}

	void dataFromJson(json_t *rootJ) override
	{
	}

	void process(const ProcessArgs &args) override
	{
		t = t + 1;

		// This is my really simple equation
		w = t;

		float output = ((float)(((uint32_t) w) << 4) / 256.0f) * 10;

		outputs[EQUATION_OUTPUT].setVoltage(output);
	}
};

struct EquationComposerWidget : ModuleWidget
{
	EquationComposerWidget(EquationComposer* module)
	{
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/equation_composer_front_panel.svg")));

		// Cosmetic rack screws
		addChild(createWidget<ScrewSilver>(Vec(15, 0)));
		addChild(createWidget<ScrewSilver>(Vec(15, 365)));

		// WAV output
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(34.236, 114.893)), module, EquationComposer::EQUATION_OUTPUT));
	}
};



Model* modelEquationComposer = createModel<EquationComposer, EquationComposerWidget>("equation_composer");
