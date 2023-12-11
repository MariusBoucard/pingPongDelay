import("stdfaust.lib");

// Parameters
delaytime = hslider("delaytime", 0.5, 0, 1, 0.01);
feedback = hslider("feedback", 0.5, 0, 1, 0.01);

// Delay lines
delaylineL = de.delay(192000, delaytime * 192000);
delaylineR = de.delay(192000, delaytime * 192000);

// Ping-pong delay
process = stereoize(_ <: _, _) with {
    stereoize = par(i, 2, route);
    route(x) = x <: ((1 - feedback) * _), (feedback * _ : delaylineL), (feedback * _ : delaylineR);
};