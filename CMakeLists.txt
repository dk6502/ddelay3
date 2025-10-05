cmake_minimum_required(VERSION 3.22)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(ddelay3 VERSION 0.0.1)

add_subdirectory(JUCE)
add_subdirectory(dkdsp)

juce_add_plugin(ddelay3
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
    PLUGIN_CODE DDL3

    FORMATS AU VST3 Standalone
    PRODUCT_NAME "DDelay3")

# juce_generate_juce_header(AudioPluginExample)
#
juce_add_binary_data(assets
    SOURCES
    assets/bg.png
    assets/NimbusSanL-Bol.otf
)

target_sources(ddelay3
    PRIVATE
        src/Main.cc)

target_compile_definitions(ddelay3
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)


target_link_libraries(ddelay3
    PRIVATE
        juce::juce_audio_utils
        assets
    PUBLIC
        dkdsp
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
