HashMap<Character, CommandHandler> handlers = new HashMap<Character, CommandHandler>();

// - CONTROL OBJECTS - 
Textlabel leftIR, 
          rightIR, 
          timeTextlabel, 
          speedTextlabel,
          PIDTextlabel;

ControlP5 panel;

Textfield turningTextfield;

final int sizeOfArrays = 5;
ArrayList<Textfield> timeTextfields = new ArrayList<Textfield>(sizeOfArrays);
ArrayList<Textfield> speedTextfields = new ArrayList<Textfield>(sizeOfArrays);
ArrayList<Textfield> PIDTextfields = new ArrayList<Textfield>(3);

ArrayList<Boolean> timeFocuses = new ArrayList<Boolean>(
  Arrays.asList(false, false, false, false, false)
);
ArrayList<Boolean> speedFocuses = new ArrayList<Boolean>(
  Arrays.asList(false, false, false, false, false)
);
ArrayList<Boolean> PIDFocuses = new ArrayList<Boolean>(
  Arrays.asList(false, false, false, false, false)
);



Slider leftSlider, rightSlider;

Toggle stopToggle;
// - NETWORK OBJECTS -
Client sam;

final int PORT = 80;
final String IP = "192.168.4.1";

// - GRAPH OBJECTS -
GPlot plot;
GPointsArray points;

final String REFERENCE = "Reference";
final String ACTUAL = "Actual";

ArrayList<Float> actualTimes = new ArrayList<Float>();
ArrayList<Float> actualSpeeds = new ArrayList<Float>();

ArrayList<Float> lastTimes  = new ArrayList<Float>();
ArrayList<Float> lastSpeeds = new ArrayList<Float>();
