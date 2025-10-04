cmake_minimum_required(VERSION 3.22)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(Milk VERSION 0.0.1)

add_subdirectory(JUCE)
add_subdirectory(dkdsp)

juce_add_plugin(milk
    # VERSION ...
    # ICON_BIG ...
    # ICON_SMALL ...
    COMPANY_NAME Dylan K.
    # IS_SYNTH TRUE/FALSE
    # NEEDS_MIDI_INPUT TRUE/FALSE
    # NEEDS_MIDI_OUTPUT TRUE/FALSE
    # IS_MIDI_EFFECT TRUE/FALSE
    # EDITOR_WANTS_KEYBOARD_FOCUS TRUE/FALSE
    # COPY_PLUGIN_AFTER_BUILD TRUE/FALSE
    PLUGIN_MANUFACTURER_CODE Dkr6
    PLUGIN_CODE MILK

    FORMATS AU VST3 Standalone
    PRODUCT_NAME "Milk")

# juce_generate_juce_header(AudioPluginExample)

target_sources(milk
    PRIVATE
        src/Main.cc)

target_compile_definitions(milk
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)


target_link_libraries(milk
    PRIVATE
        juce::juce_audio_utils
    PUBLIC
        dkdsp
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
