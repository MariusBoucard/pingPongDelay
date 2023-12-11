## Faust Implementation

First step : create your dsp using faust algorithm
Then, export it's minimal version using faust minimal.cpp architecture
It will create a faustEffect.cpp file, that can be renamed as a header.

Put this dsp into your project

Import the necessairies headers (faust headers needs to be set in juce settings for this project, setup in vscode as well)

then you can import you dsp as shown in this code 
// I have to insert a tag

### Plugin the interface

Severals things to do :

    - Create setters for parameters inside the FaustEffect.h -> Have to think about a way to avoid overwriting this code while updating the faust dsp...
    - set the parameters within the nice range thanks to the update function, getting things from gui as usual
    