void updateReferenceData(GPlot plot,
                         ArrayList<Textfield> timeFields,
                         ArrayList<Textfield> speedFields) {

  ArrayList<Float> times  = new ArrayList<Float>();
  ArrayList<Float> speeds = new ArrayList<Float>();

  // 1. Read and validate
  for (int i = 0; i < timeFields.size(); i++) {
    Textfield tf = timeFields.get(i);
    Textfield sf = speedFields.get(i);
    if (tf == null || sf == null) return;

    String t = tf.getText();
    String s = sf.getText();

    if (t == null || t.trim().isEmpty()
        || s == null || s.trim().isEmpty()) {
      return;
    }

    times.add(Float.parseFloat(t.trim()));
    speeds.add(Float.parseFloat(s.trim()));
  }

  // 2. Build square (step) curve
  ArrayList<Float> stepTimes  = new ArrayList<Float>();
  ArrayList<Float> stepSpeeds = new ArrayList<Float>();

if (times.size() > 0) {
  float firstT = times.get(0);
  float firstS = speeds.get(0);
  float lastS  = speeds.get(speeds.size() - 1);

  // Horizontal from t = 0 to firstT at firstS
  stepTimes.add(0.0f);
  stepSpeeds.add(firstS);
  stepTimes.add(firstT);
  stepSpeeds.add(firstS);

  // Existing step construction between internal points
  for (int i = 1; i < times.size(); i++) {
    float sPrev = speeds.get(i - 1);
    float tCurr = times.get(i);
    float sCurr = speeds.get(i);

    // horizontal segment
    stepTimes.add(tCurr);
    stepSpeeds.add(sPrev);
    // vertical step
    stepTimes.add(tCurr);
    stepSpeeds.add(sCurr);
  }

  // Horizontal from lastT to t = 60 at lastS
  stepTimes.add(60.0f);
  stepSpeeds.add(lastS);
}

  GPointsArray points = makeGPointsArray(stepTimes, stepSpeeds);
  plot.getLayer(REFERENCE).setPoints(points);


  
  // 5. Update cache
  lastTimes.clear();
  lastTimes.addAll(times);
  lastSpeeds.clear();
  lastSpeeds.addAll(speeds);
}

boolean floatEqual(Float a, Float b) {
  if (a == null && b == null) return true;
  if (a == null || b == null) return false;
  return a.equals(b);
}
