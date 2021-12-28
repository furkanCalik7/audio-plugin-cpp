/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

struct ComprosserBand {
public:

	juce::AudioParameterFloat* attackParameter{nullptr};
	juce::AudioParameterFloat* tresholdParameter{nullptr};
	juce::AudioParameterFloat* releaseParameter{nullptr};
	juce::AudioParameterChoice* ratioParameter{nullptr};
	juce::AudioParameterBool* byPassedParameter{nullptr};
	juce::AudioParameterBool* muteParameter{nullptr};
	juce::AudioParameterBool* SoloParameter{nullptr};

	void setReady(const juce::dsp::ProcessSpec& s) {
		cmp.prepare(s);
	};

	void updateCmpSettings() {
		using namespace juce;

		cmp.setAttack(attackParameter->get());
		cmp.setRelease(releaseParameter->get());
		cmp.setThreshold(tresholdParameter->get());
		cmp.setRatio(ratioParameter->getCurrentChoiceName().getFloatValue());
	};

	void processBuffer(juce::AudioBuffer<float>& b) {
		using namespace juce;

		auto aBlock = dsp::AudioBlock<float>(b);
		auto context = dsp::ProcessContextReplacing<float>(aBlock);
		context.isBypassed = byPassedParameter->get();
		cmp.process(context);
	};
private:
	juce::dsp::Compressor<float> cmp;
};


namespace Parameters {
	enum FreqInts {
		L_M_CR_F,
		M_H_CR_F,

		TD_L_B,
		TD_M_B,
		TD_H_B,

		A_L_B,
		A_M_B,
		A_H_B,

		R_L_B,
		R_M_B,
		R_H_B,

		RT_L_B,
		RT_M_B,
		RT_H_B,

		By_L_B,
		By_M_B,
		By_H_B,

		M_L_B,
		M_M_B,
		M_H_B,

		S_L_B,
		S_M_B,
		S_H_B,
		
		Gain_in,
		Gain_out	
	};

	inline const std::map<FreqInts, juce::String>& GetParams() {
		static std::map<FreqInts, juce::String> parameters = {
			{L_M_CR_F, "Low-Mid Crossover Freq"},
			{M_H_CR_F, "Mid-High Crossover Freq"},
			{TD_L_B, "Threshold low band"},
			{TD_M_B, "Threshold mid band"},
			{TD_H_B, "Threshold highm band"},
			{A_L_B, "Attack low band"},
			{A_M_B, "Attack mid band"},
			{A_H_B, "Attack high band"},
			{R_L_B, "Release low band"},
			{R_M_B, "Release mid band"},
			{R_H_B, "Release high band"},
			{RT_L_B, "Ratio low band"},
			{RT_M_B, "Ratio mid band"},
			{RT_H_B, "Ratio high band"},
			{By_L_B, "Bypassed low band"},
			{By_M_B, "Bypassed mid band"},
			{By_H_B, "Bypassed high band"},
			
			{S_L_B, "Solo low band"},
			{S_M_B, "Solo mid band"},
			{S_H_B, "Solo high band"},
			
			{M_L_B, "Mute low band"},
			{M_M_B, "Mute mid band"},
			{M_H_B, "Mute high band"},
			{Gain_in, "Gain in"},
			{Gain_out, "Gain out"}

		};

		return parameters;
	}

}

//==============================================================================
/**
*/
class SimpleMBCompAudioProcessor : public juce::AudioProcessor {
public:
	//==============================================================================
	SimpleMBCompAudioProcessor();
	~SimpleMBCompAudioProcessor() override;

	//==============================================================================
	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
	void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
	bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

	void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

	//==============================================================================
	juce::AudioProcessorEditor* createEditor() override;
	bool hasEditor() const override;

	//==============================================================================
	const juce::String getName() const override;

	bool acceptsMidi() const override;
	bool producesMidi() const override;
	bool isMidiEffect() const override;
	double getTailLengthSeconds() const override;

	//==============================================================================
	int getNumPrograms() override;
	int getCurrentProgram() override;
	void setCurrentProgram(int index) override;
	const juce::String getProgramName(int index) override;
	void changeProgramName(int index, const juce::String& newName) override;

	//==============================================================================
	void getStateInformation(juce::MemoryBlock& destData) override;
	void setStateInformation(const void* data, int sizeInBytes) override;
	using APVTS = juce::AudioProcessorValueTreeState;
	static APVTS::ParameterLayout createParameterLayout();

	APVTS apvts{*this, nullptr, "Parameters", createParameterLayout()};


private:
	std::array<ComprosserBand, 3> cmds_;
	ComprosserBand& lowBandComprosser = cmds_[0];
	ComprosserBand& midBandComprosser = cmds_[1];
	ComprosserBand& highBandComprosser = cmds_[2];

	juce::dsp::LinkwitzRileyFilter<float> LP1, AP2, HP1, LP2, HP2;


	juce::AudioParameterFloat* lMCrossOver{nullptr};
	juce::AudioParameterFloat* mHCrossOver{nullptr};
	// We are using two seperate buffers to destroy the all frequency due to the effect of high pass and low pass
	std::array<juce::AudioBuffer<float>, 3> filterBuffers;
	juce::dsp::Gain<float> iGain, oGain;
	juce::AudioParameterFloat* iGainParam{ nullptr };
	juce::AudioParameterFloat* oGainParam{ nullptr };
	//==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleMBCompAudioProcessor)
};
