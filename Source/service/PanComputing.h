#include <JuceHeader.h>

class PanComputing
{
public:
  static struct ParametersPan
  {
    float bpm =100;
    float ppqPosition = 0;
    float ppqMesure = 0;
    float timeSecond = 1000.0f;
    int timeSigDenominator  = 4;
    int timeSigNumerator = 4;

    std::string panType;
    float pingPongTime = 1000.0f;
    std::string pingPongNote = "1/4";
    float offset = 0.0f;
    bool inTime;
    int revertPan = 0;
  };
static float computePan(ParametersPan parameters){
    
    // TODO ADD PARAMETER FOR PAN TIME
    double beatTime = 60.0f / parameters.bpm;
    double timeMesure = parameters.ppqMesure * beatTime;
    double pan = 0.0f;


        double delta = 0; 
        if(parameters.inTime){
        delta = (parameters.timeSecond - timeMesure) / (parameters.pingPongTime/1000.0f);
        delta = delta + parameters.offset;
         delta = fmod(delta, 1.0f);

        } else {
    
            // A LA BASE ON EST SUR 1/4;
            // COMPORTEMENT QUE EN  4/4;
            // TODO ADD OTHER TIME SIGNATURE

            delta = (parameters.ppqPosition - parameters.ppqMesure) / parameters.timeSigNumerator;
            std::cout << "DELTA : " << delta << "PING PONG NOTE : " << parameters.pingPongNote << std::endl;

            // Ajout du décalage : 
            delta = delta + parameters.offset;
            delta = fmod(delta, 1.0f);
            if(parameters.pingPongNote == "1/4"){
                std::cout << "kre "<< std ::endl;
            }
            
        if (parameters.pingPongNote == "1/2")
        {
            // TODO : Keep Record of last mesure

            delta = delta / 2;
        }

        else if (parameters.pingPongNote == "1/8")
        {
            delta = delta* 2 ;
        }
        else if (parameters.pingPongNote == "1/16")
        {
            delta = delta * 4;
        }
        else if (parameters.pingPongNote == "1/32")
        {
            delta = delta * 8;
        }
        delta = std::fmod(delta, 1.0f); // Perform floating-point modulo operation
        

        }

        if (delta > 0.5f)
        {
            delta = 1.0f - delta;
        }

        jassert(delta <= 1.0f && delta >= 0.0f);
    if (parameters.panType == "Linear")
    {
        pan = delta * 4.0f - 1.0f;

        jassert(pan <= 1.0f && pan >= -1.0f);
    }
    else if (parameters.panType == "Sinus")
    {
        // double delta = std::fmod(((parameters.pingPongTime / 1000.0f)) * 0.5f + parameters.timeSecond, (parameters.pingPongTime / 1000.0f)) / (parameters.pingPongTime / 1000.0f);
        pan = std::sin(delta * 2.0f * juce::MathConstants<float>::pi);
    }
    else if (parameters.panType == "MadSin")
    {
        // VERSION SIN RANDOM
        pan = std::sin(parameters.timeSecond * 2.0f * juce::MathConstants<float>::pi);
    }

    if(parameters.revertPan == 1){
        pan = -pan;
    } 
    return pan;
}
};