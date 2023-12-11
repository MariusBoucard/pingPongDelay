# Template Faust, Juce, AudioGraph VST Program

This program is a default template to help you implement a beautifull gui on the top of a faust algorithm to create dsp, bundled in an audiograph from juce. It allows modular implementation, and so developping almost any purpose.

![Application screenshot](./Source/Resources/screenshot.PNG)


## JUCE
    This program has been made using juce framework and foleys librairy for GUI.

## Still Todo
    Refactor the audioProcessor class to make it more far from the application entrypoint.
    As well, we will need to create interface for parameters and setters and all
    Define an abstract standard

## Use it
    To use this code and play with, install juce, add the relevant foleys librairy gui module and clone this repository.
    Then every thing should work.

## Issues encountered :

    Allow the binary_data in foleys's module parameter, other one no one can load images.

## Installation

To install the compressor VST program, simply download the appropriate version for your operating system and follow the installation instructions for your DAW.

## Usage

Once installed, the compressor VST program can be used like any other VST plugin in your DAW. Simply add it to a track or bus and adjust the parameters to your liking.

## License

This program is licensed under the [MIT License](LICENSE).

### Todo 
Make a small program / see in the c++ faust compiler ?? 
To modify the file and add the relevant setters directly in the file to prevent having extra work to do