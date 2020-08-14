struct AutomatedSlider : Module
{
  enum ParamIds {
    NUM_PARAMS
  };
  enum InputIds {
    NUM_INPUTS
  };
  enum OutputIds {
    NUM_OUTPUTS
  };
  enum LightIds {
    NUM_LIGHTS
  };

  //
  // Constructor
  //
  AutomatedSlider()
  {
    config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
  }

  // Autosave settings
  json_t *dataToJson() override
  {
    json_t *json_root = json_object();

    return json_root;
  }

  // Autoload settings
  void dataFromJson(json_t *json_root) override
  {
  }

  void process(const ProcessArgs &args) override
  {
  }
};
