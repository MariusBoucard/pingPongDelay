/* ------------------------------------------------------------
name: "EchoMonoToStereo"
Code generated with Faust 2.68.1 (https://faust.grame.fr)
Compilation options: -a ../faustMinimal.h -lang cpp -ct 1 -cn EchoMonoToStereo -es 1 -mcd 16 -single -ftz 0
------------------------------------------------------------ */

#ifndef  __EchoMonoToStereo_H__
#define  __EchoMonoToStereo_H__

#include <cmath>
#include <cstring>

#include "faust/gui/MapUI.h"
#include "faust/gui/meta.h"
#include "faust/dsp/dsp.h"

// BEGIN-FAUSTDSP


#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif 

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <math.h>

#ifndef FAUSTCLASS 
#define FAUSTCLASS EchoMonoToStereo
#endif

#ifdef __APPLE__ 
#define exp10f __exp10f
#define exp10 __exp10
#endif

#if defined(_WIN32)
#define RESTRICT __restrict
#else
#define RESTRICT __restrict__
#endif


class EchoMonoToStereo : public dsp {
	
 private:
	
	FAUSTFLOAT fHslider0;
	int IOTA0;
	int fSampleRate;
	float fConst0;
	FAUSTFLOAT fHslider1;
	float fRec0[524288];
	FAUSTFLOAT fHslider2;
	
 public:
	EchoMonoToStereo() {}

	void metadata(Meta* m) { 
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/tabulateNd", "Copyright (C) 2023 Bart Brouns <bart@magnetophon.nl>");
		m->declare("basics.lib/version", "1.11.1");
		m->declare("compile_options", "-a ../faustMinimal.h -lang cpp -ct 1 -cn EchoMonoToStereo -es 1 -mcd 16 -single -ftz 0");
		m->declare("delays.lib/name", "Faust Delay Library");
		m->declare("delays.lib/version", "1.1.0");
		m->declare("filename", "EchoMonoToStereo.dsp");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.6.0");
		m->declare("misceffects.lib/echo:author", "Romain Michon");
		m->declare("misceffects.lib/name", "Misc Effects Library");
		m->declare("misceffects.lib/version", "2.1.0");
		m->declare("name", "EchoMonoToStereo");
		m->declare("platform.lib/name", "Generic Platform Library");
		m->declare("platform.lib/version", "1.3.0");
	}

	virtual int getNumInputs() {
		return 1;
	}
	virtual int getNumOutputs() {
		return 2;
	}
	
	static void classInit(int sample_rate) {
	}
	
	virtual void instanceConstants(int sample_rate) {
		fSampleRate = sample_rate;
		fConst0 = 0.001f * std::min<float>(1.92e+05f, std::max<float>(1.0f, float(fSampleRate)));
	}
	
	virtual void instanceResetUserInterface() {
		fHslider0 = FAUSTFLOAT(0.4f);
		fHslider1 = FAUSTFLOAT(1e+03f);
		fHslider2 = FAUSTFLOAT(0.0f);
	}
	
	virtual void instanceClear() {
		IOTA0 = 0;
		for (int l0 = 0; l0 < 524288; l0 = l0 + 1) {
			fRec0[l0] = 0.0f;
		}
	}
	
	virtual void init(int sample_rate) {
		classInit(sample_rate);
		instanceInit(sample_rate);
	}
	
	virtual void instanceInit(int sample_rate) {
		instanceConstants(sample_rate);
		instanceResetUserInterface();
		instanceClear();
	}
	
	virtual EchoMonoToStereo* clone() {
		return new EchoMonoToStereo();
	}
	
	virtual int getSampleRate() {
		return fSampleRate;
	}
    
    virtual void setParamValue(std::string valuename , float value) {
        if (valuename == "delay") {
            fHslider1 = FAUSTFLOAT(value);
        }
        if (valuename == "feedback") {
            fHslider0 = FAUSTFLOAT(value);
        }
        if (valuename == "delta") {
            fHslider2 = FAUSTFLOAT(value);
        }
    }

	
	virtual void buildUserInterface(UI* ui_interface) {
		ui_interface->openVerticalBox("EchoMonoToStereo");
		ui_interface->addHorizontalSlider("Delay", &fHslider1, FAUSTFLOAT(1e+03f), FAUSTFLOAT(0.0f), FAUSTFLOAT(2e+03f), FAUSTFLOAT(1.0f));
		ui_interface->addHorizontalSlider("Delta", &fHslider2, FAUSTFLOAT(0.0f), FAUSTFLOAT(0.0f), FAUSTFLOAT(2e+03f), FAUSTFLOAT(1.0f));
		ui_interface->addHorizontalSlider("FeedBack", &fHslider0, FAUSTFLOAT(0.4f), FAUSTFLOAT(0.0f), FAUSTFLOAT(0.99f), FAUSTFLOAT(0.1f));
		ui_interface->closeBox();
	}
	
	virtual void compute(int count, FAUSTFLOAT** RESTRICT inputs, FAUSTFLOAT** RESTRICT outputs) {
		FAUSTFLOAT* input0 = inputs[0];
		FAUSTFLOAT* output0 = outputs[0];
		FAUSTFLOAT* output1 = outputs[1];
		float fSlow0 = float(fHslider0);
		float fSlow1 = fConst0 * float(fHslider1);
		int iSlow2 = int(std::min<float>(fSlow1, std::max<float>(0.0f, fSlow1))) + 1;
		int iSlow3 = int(fConst0 * float(fHslider2));
		for (int i0 = 0; i0 < count; i0 = i0 + 1) {
			fRec0[IOTA0 & 524287] = float(input0[i0]) + fSlow0 * fRec0[(IOTA0 - iSlow2) & 524287];
			float fTemp0 = fRec0[(IOTA0 - iSlow3) & 524287];
			output0[i0] = FAUSTFLOAT(fTemp0);
			output1[i0] = FAUSTFLOAT(fTemp0);
			IOTA0 = IOTA0 + 1;
		}
	}

};

// END-FAUSTDSP

#endif
