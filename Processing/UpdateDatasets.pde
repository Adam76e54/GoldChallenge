// Utility: mark a field as OK or error
void markFieldOK(Textfield tf) {
  tf.setColorBackground(color(0, 60, 110));   // normal dark blue
}

void markFieldError(Textfield tf) {
  tf.setColorBackground(color(180, 40, 40));  // red for error
}

void updateReferenceData(GPlot plot,
                         ArrayList<Textfield> timeFields,
                         ArrayList<Textfield> speedFields) {

  ArrayList<Float> times  = new ArrayList<Float>();
  ArrayList<Float> speeds = new ArrayList<Float>();

  boolean hasError = false;

  // 1. Read and validate input --------------------
  for (int i = 0; i < timeFields.size(); i++) {
    Textfield tf = timeFields.get(i);
    Textfield sf = speedFields.get(i);
    if (tf == null || sf == null) return;

    String tStr = tf.getText();
    String sStr = sf.getText();

    // Reset colours before validation
    markFieldOK(tf);
    markFieldOK(sf);

    if (tStr == null || tStr.trim().isEmpty()
        || sStr == null || sStr.trim().isEmpty()) {
      markFieldError(tf);
      markFieldError(sf);
      hasError = true;
      continue;
    }

    try {
      float t = Float.parseFloat(tStr.trim());
      float s = Float.parseFloat(sStr.trim());
      times.add(t);
      speeds.add(s);
    } catch (NumberFormatException ex) {
      markFieldError(tf);
      markFieldError(sf);
      hasError = true;
    }
  }

  // If any error, do not update plot
  if (hasError || times.isEmpty()) return;

  // Ensure strictly increasing times
  for (int i = 1; i < times.size(); i++) {
    if (times.get(i) <= times.get(i - 1)) {
      // Mark both fields involved
      markFieldError(timeFields.get(i - 1));
      markFieldError(timeFields.get(i));
      hasError = true;
    }
  }
  if (hasError) return;

  // 2. Build square (step) curve ------------------
  final float T_END = 60.0f;

  ArrayList<Float> stepTimes  = new ArrayList<Float>();
  ArrayList<Float> stepSpeeds = new ArrayList<Float>();

  float s0 = speeds.get(0);
  float t0 = times.get(0);

  // From t = 0 at first speed
  stepTimes.add(0.0f);
  stepSpeeds.add(s0);

  // Vertical step at first time
  stepTimes.add(t0);
  stepSpeeds.add(s0);

  // Subsequent points
  for (int i = 1; i < times.size(); i++) {
    float sPrev = speeds.get(i - 1);
    float tCurr = times.get(i);
    float sCurr = speeds.get(i);

    stepTimes.add(tCurr);
    stepSpeeds.add(sPrev);  // horizontal
    stepTimes.add(tCurr);
    stepSpeeds.add(sCurr);  // vertical
  }

  // Final horizontal to end time
  float sLast = speeds.get(speeds.size() - 1);
  stepTimes.add(T_END);
  stepSpeeds.add(sLast);

  // 3. Update plot --------------------------------
  GPointsArray points = makeGPointsArray(stepTimes, stepSpeeds);
  plot.getLayer(REFERENCE).setPoints(points);

  // 4. Update cache -------------------------------
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
