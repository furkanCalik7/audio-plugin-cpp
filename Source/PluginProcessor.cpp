/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SimpleMBCompAudioProcessor::SimpleMBCompAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
		.withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
		.withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
	)
#endif
{
	using namespace juce;
	using namespace Parameters;
	const auto& params= GetParams();

	lowBandComprosser.attackParameter = dynamic_cast<AudioParameterFloat*>(apvts.getParameter(params.at(Parameters::A_L_B)));
	jassert(lowBandComprosser.attackParameter != nullptr);
	midBandComprosser.attackParameter = dynamic_cast<AudioParameterFloat*>(apvts.getParameter(params.at(Parameters::A_M_B)));
	jassert(midBandComprosser.attackParameter != nullptr);
	highBandComprosser.attackParameter = dynamic_cast<AudioParameterFloat*>(apvts.getParameter(params.at(Parameters::A_H_B)));
	jassert(highBandComprosser.attackParameter != nullptr);

	lowBandComprosser.releaseParameter = dynamic_cast<AudioParameterFloat*>(apvts.getParameter(params.at(Parameters::R_L_B)));
	jassert(lowBandComprosser.releaseParameter != nullptr);
	midBandComprosser.releaseParameter = dynamic_cast<AudioParameterFloat*>(apvts.getParameter(params.at(Parameters::R_M_B)));
	jassert(midBandComprosser.releaseParameter != nullptr);
	highBandComprosser.releaseParameter = dynamic_cast<AudioParameterFloat*>(apvts.getParameter(params.at(Parameters::R_H_B)));
	jassert(highBandComprosser.releaseParameter != nullptr);

	lowBandComprosser.ratioParameter = dynamic_cast<AudioParameterChoice*>(apvts.getParameter(params.at(Parameters::RT_L_B)));
	jassert(lowBandComprosser.ratioParameter != nullptr);
	midBandComprosser.ratioParameter = dynamic_cast<AudioParameterChoice*>(apvts.getParameter(params.at(Parameters::RT_M_B)));
	jassert(midBandComprosser.ratioParameter != nullptr);
	highBandComprosser.ratioParameter = dynamic_cast<AudioParameterChoice*>(apvts.getParameter(params.at(Parameters::RT_H_B)));
	jassert(highBandComprosser.ratioParameter != nullptr);
	
	lowBandComprosser.tresholdParameter = dynamic_cast<AudioParameterFloat*>(apvts.getParameter(params.at(Parameters::TD_L_B)));
	jassert(lowBandComprosser.tresholdParameter != nullptr);
	midBandComprosser.tresholdParameter = dynamic_cast<AudioParameterFloat*>(apvts.getParameter(params.at(Parameters::TD_M_B)));
	jassert(midBandComprosser.tresholdParameter != nullptr);
	highBandComprosser.tresholdParameter = dynamic_cast<AudioParameterFloat*>(apvts.getParameter(params.at(Parameters::TD_H_B)));
	jassert(highBandComprosser.tresholdParameter != nullptr);
	

	lowBandComprosser.byPassedParameter = dynamic_cast<AudioParameterBool*>(apvts.getParameter(params.at(Parameters::By_L_B)));
	jassert(lowBandComprosser.byPassedParameter != nullptr);
	midBandComprosser.byPassedParameter = dynamic_cast<AudioParameterBool*>(apvts.getParameter(params.at(Parameters::By_M_B)));
	jassert(midBandComprosser.byPassedParameter != nullptr);
	highBandComprosser.byPassedParameter = dynamic_cast<AudioParameterBool*>(apvts.getParameter(params.at(Parameters::By_H_B)));
	jassert(highBandComprosser.byPassedParameter != nullptr);

	lowBandComprosser.SoloParameter = dynamic_cast<AudioParameterBool*>(apvts.getParameter(params.at(Parameters::S_L_B)));
	jassert(lowBandComprosser.SoloParameter != nullptr);
	midBandComprosser.SoloParameter = dynamic_cast<AudioParameterBool*>(apvts.getParameter(params.at(Parameters::S_M_B)));
	jassert(midBandComprosser.SoloParameter != nullptr);
	highBandComprosser.SoloParameter = dynamic_cast<AudioParameterBool*>(apvts.getParameter(params.at(Parameters::S_H_B)));
	jassert(highBandComprosser.SoloParameter != nullptr);

	lowBandComprosser.muteParameter = dynamic_cast<AudioParameterBool*>(apvts.getParameter(params.at(Parameters::M_L_B)));
	jassert(lowBandComprosser.muteParameter != nullptr);
	midBandComprosser.muteParameter = dynamic_cast<AudioParameterBool*>(apvts.getParameter(params.at(Parameters::M_M_B)));
	jassert(midBandComprosser.muteParameter != nullptr);
	highBandComprosser.muteParameter = dynamic_cast<AudioParameterBool*>(apvts.getParameter(params.at(Parameters::M_H_B)));
	jassert(highBandComprosser.muteParameter != nullptr);
	

	lMCrossOver = dynamic_cast<AudioParameterFloat*>(apvts.getParameter(params.at(Parameters::L_M_CR_F)));
	jassert(lMCrossOver != nullptr);

	mHCrossOver = dynamic_cast<AudioParameterFloat*>(apvts.getParameter(params.at(Parameters::M_H_CR_F)));
	jassert(mHCrossOver != nullptr);
	

	// Low and high pass type is set

	LP1.setType(dsp::LinkwitzRileyFilterType::lowpass);
	LP2.setType(dsp::LinkwitzRileyFilterType::lowpass);

	HP1.setType(dsp::LinkwitzRileyFilterType::highpass);
	HP2.setType(dsp::LinkwitzRileyFilterType::highpass);

	AP2.setType(dsp::LinkwitzRileyFilterType::allpass);
}

SimpleMBCompAudioProcessor::~SimpleMBCompAudioProcessor() {
}

//==============================================================================
const juce::String SimpleMBCompAudioProcessor::getName() const {
	return JucePlugin_Name;
}

bool SimpleMBCompAudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
    return true;
#else
	return false;
#endif
}

bool SimpleMBCompAudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
    return true;
#else
	return false;
#endif
}

bool SimpleMBCompAudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
    return true;
#else
	return false;
#endif
}

double SimpleMBCompAudioProcessor::getTailLengthSeconds() const {
	return 0.0;
}

int SimpleMBCompAudioProcessor::getNumPrograms() {
	return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
	// so this should be at least 1, even if you're not really implementing programs.
}

int SimpleMBCompAudioProcessor::getCurrentProgram() {
	return 0;
}

void SimpleMBCompAudioProcessor::setCurrentProgram(int index) {
}

const juce::String SimpleMBCompAudioProcessor::getProgramName(int index) {
	return {};
}

void SimpleMBCompAudioProcessor::changeProgramName(int index, const juce::String& newName) {
}

//==============================================================================
void SimpleMBCompAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
	using namespace juce;
	dsp::ProcessSpec spec;
	spec.maximumBlockSize = samplesPerBlock;
	spec.numChannels = getTotalNumOutputChannels();
	spec.sampleRate = sampleRate;

	for( auto& com : cmds_) {
		com.setReady(spec);
	}
	LP1.prepare(spec);
	LP2.prepare(spec);
	HP2.prepare(spec);
	HP1.prepare(spec);
	AP2.prepare(spec);

	
	for (auto& buffer : filterBuffers) {
		buffer.setSize(spec.numChannels, samplesPerBlock);
	}

}

void SimpleMBCompAudioProcessor::releaseResources() {
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SimpleMBCompAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
#else
	// This is the place where you check if the layout is supported.
	// In this template code we only support mono or stereo.
	// Some plugin hosts, such as certain GarageBand versions, will only
	// load plugins that support stereo bus layouts.
	if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
		&& layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
		return false;

	// This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
	if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
		return false;
#endif

	return true;
#endif
}
#endif

void SimpleMBCompAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
	juce::ScopedNoDenormals noDenormals;
	auto totalNumInputChannels = getTotalNumInputChannels();
	auto totalNumOutputChannels = getTotalNumOutputChannels();

	// In case we have more outputs than inputs, this code clears any output
	// channels that didn't contain input data, (because these aren't
	// guaranteed to be empty - they may contain garbage).
	// This is here to avoid people getting screaming feedback
	// when they first compile a plugin, but obviously you don't need to keep
	// this code if your algorithm always overwrites all the output channels.
	for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
		buffer.clear(i, 0, buffer.getNumSamples());

	for(auto& comp : cmds_) {
		comp.updateCmpSettings();
	}
	
	

	for (auto& filterBuffer : filterBuffers) {
		filterBuffer = buffer;
	}
	auto cutoff = lMCrossOver->get();
	LP1.setCutoffFrequency(cutoff);
	HP1.setCutoffFrequency(cutoff);

	auto cutoff2 = mHCrossOver->get();

	HP2.setCutoffFrequency(cutoff2);
	LP2.setCutoffFrequency(cutoff2);
	AP2.setCutoffFrequency(cutoff2);

	auto filterBlock0Buffer = juce::dsp::AudioBlock<float>(filterBuffers[0]);
	auto filterBlock1Buffer = juce::dsp::AudioBlock<float>(filterBuffers[1]);
	auto filterBlock2Buffer = juce::dsp::AudioBlock<float>(filterBuffers[2]);

	auto filterBlock0Context = juce::dsp::ProcessContextReplacing<float>(filterBlock0Buffer);
	auto filterBlock1Context = juce::dsp::ProcessContextReplacing<float>(filterBlock1Buffer);
	auto filterBlock2Context = juce::dsp::ProcessContextReplacing<float>(filterBlock2Buffer);


	LP1.process(filterBlock0Context);
	AP2.process(filterBlock0Context);

	HP1.process(filterBlock1Context);
	filterBuffers[2] = filterBuffers[1];
	LP2.process(filterBlock1Context);
	HP2.process(filterBlock2Context);

	cmds_[0].processBuffer(filterBuffers[0]);
	cmds_[1].processBuffer(filterBuffers[1]);
	cmds_[2].processBuffer(filterBuffers[2]);

	auto numberOfSamplers = buffer.getNumSamples();
	auto nOfChannels = buffer.getNumChannels();

	buffer.clear();

	auto AreBandSolo = false;
	for( auto& comp : cmds_) {
		if(comp.SoloParameter->get()) {
			AreBandSolo = true;
			break;
		}
	}
	
	auto addFilterBand = [numberOfChannels = nOfChannels, nSamplers = numberOfSamplers](
		auto& inputBuffer, const auto& source) {
		for (auto i = 0; i < numberOfChannels; i++) {
			inputBuffer.addFrom(i, 0, source, i, 0, nSamplers);
		}
	};

	if(AreBandSolo) {
		for(int i = 0; i < cmds_.size(); i++) {
			if(cmds_[i].SoloParameter->get()) {
				addFilterBand(buffer, filterBuffers[i]);
			}
		}
	} else {
		for (int i = 0; i < cmds_.size(); i++) {
			if(!cmds_[i].muteParameter->get()) {
				addFilterBand(buffer, filterBuffers[i]);
			}
		}
	}
	
	addFilterBand(buffer, filterBuffers[0]);
	addFilterBand(buffer, filterBuffers[1]);
	addFilterBand(buffer, filterBuffers[2]);

}

//==============================================================================
bool SimpleMBCompAudioProcessor::hasEditor() const {
	return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SimpleMBCompAudioProcessor::createEditor() {
	//return new SimpleMBCompAudioProcessorEditor (*this);
	return new juce::GenericAudioProcessorEditor(*this);
}


//==============================================================================
void SimpleMBCompAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
	using namespace juce;
	MemoryOutputStream outputSt(destData, true);
	apvts.state.writeToStream(outputSt);

}

void SimpleMBCompAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {

	using namespace juce;
	auto tree = ValueTree::readFromData(data, sizeInBytes);
	if (tree.isValid()) {
		apvts.replaceState(tree);
	}
}

juce::AudioProcessorValueTreeState::ParameterLayout SimpleMBCompAudioProcessor::createParameterLayout() {
	APVTS::ParameterLayout layout;
	using namespace juce;
	using namespace Parameters;
	const auto& params = GetParams();
	// Threshold parameter implementation
	
	
	
	layout.add(std::make_unique<AudioParameterFloat>(params.at(Parameters::TD_L_B), params.at(Parameters::TD_L_B),
	                                                 NormalisableRange<float>(-60, 12, 1, 1), 0));
	layout.add(std::make_unique<AudioParameterFloat>(params.at(Parameters::TD_M_B), params.at(Parameters::TD_M_B),
	                                                 NormalisableRange<float>(-60, 12, 1, 1), 0));
	layout.add(std::make_unique<AudioParameterFloat>(params.at(Parameters::TD_H_B), params.at(Parameters::TD_H_B),
	                                                 NormalisableRange<float>(-60, 12, 1, 1), 0));
	auto attackReleaseRange = NormalisableRange<float>(5, 500, 1, 1);

	layout.add(std::make_unique<AudioParameterFloat>(params.at(Parameters::A_L_B), params.at(Parameters::A_L_B),
	                                                 attackReleaseRange, 50));
	layout.add(std::make_unique<AudioParameterFloat>(params.at(Parameters::A_M_B), params.at(Parameters::A_M_B),
	                                                 attackReleaseRange, 50));
	layout.add(std::make_unique<AudioParameterFloat>(params.at(Parameters::A_H_B), params.at(Parameters::A_H_B),
	                                                 attackReleaseRange, 50));

	layout.add(std::make_unique<AudioParameterFloat>(params.at(Parameters::R_L_B), params.at(Parameters::R_L_B),
	                                                 attackReleaseRange, 250));
	layout.add(std::make_unique<AudioParameterFloat>(params.at(Parameters::R_M_B), params.at(Parameters::R_M_B),
	                                                 attackReleaseRange, 250));
	layout.add(std::make_unique<AudioParameterFloat>(params.at(Parameters::R_H_B), params.at(Parameters::R_H_B),
	                                                 attackReleaseRange, 250));

	auto choices = std::vector<double>{1, 1.5, 2, 3, 4, 5, 6, 7, 8, 9, 10, 15, 20, 50, 100};
	StringArray stringArray;
	for (auto choice : choices) {
		stringArray.add(String(choice, 1));
	}
	layout.add(std::make_unique<AudioParameterChoice>(params.at(Parameters::RT_L_B), params.at(Parameters::RT_L_B),
	                                                  stringArray, 3));
	layout.add(std::make_unique<AudioParameterChoice>(params.at(Parameters::RT_M_B), params.at(Parameters::RT_M_B),
	                                                  stringArray, 3));
	layout.add(std::make_unique<AudioParameterChoice>(params.at(Parameters::RT_H_B), params.at(Parameters::RT_H_B),
	                                                  stringArray, 3));
	layout.add(
		std::make_unique<AudioParameterBool>(params.at(Parameters::By_L_B), params.at(Parameters::By_L_B), false));
	layout.add(													   
		std::make_unique<AudioParameterBool>(params.at(Parameters::By_M_B), params.at(Parameters::By_M_B), false));
	layout.add(													   
		std::make_unique<AudioParameterBool>(params.at(Parameters::By_H_B), params.at(Parameters::By_H_B), false));

	layout.add(
		std::make_unique<AudioParameterBool>(params.at(Parameters::M_L_B), params.at(Parameters::M_L_B), false));
	layout.add(													   
		std::make_unique<AudioParameterBool>(params.at(Parameters::M_M_B), params.at(Parameters::M_M_B), false));
	layout.add(													   
		std::make_unique<AudioParameterBool>(params.at(Parameters::M_H_B), params.at(Parameters::M_H_B), false));

	layout.add(
		std::make_unique<AudioParameterBool>(params.at(Parameters::S_L_B), params.at(Parameters::S_L_B), false));
	layout.add(
		std::make_unique<AudioParameterBool>(params.at(Parameters::S_M_B), params.at(Parameters::S_M_B), false));
	layout.add(
		std::make_unique<AudioParameterBool>(params.at(Parameters::S_H_B), params.at(Parameters::S_H_B), false));
	
	
	layout.add(std::make_unique<AudioParameterFloat>(params.at(Parameters::L_M_CR_F), params.at(Parameters::L_M_CR_F),
	                                                 NormalisableRange<float>(20, 999, 1, 1), 300));
	layout.add(std::make_unique<AudioParameterFloat>(params.at(Parameters::M_H_CR_F), params.at(Parameters::M_H_CR_F),
	                                                 NormalisableRange<float>(1000, 20000, 1, 1), 2000));


	return layout;
}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
	return new SimpleMBCompAudioProcessor();
}
