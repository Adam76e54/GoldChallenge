// Utility: mark a field as OK or error
void markFieldOK(Textfield tf) {
  tf.setColorBackground(color(0, 60, 110));   // normal dark blue
}

void markFieldError(Textfield tf) {
  tf.setColorBackground(color(180, 40, 40));  // red for error
}

// Optional: a helper to mark all time fields in error
void markAllTimeFieldsError(ArrayList<Textfield> timeFields) {
  for (Textfield tf : timeFields) {
    if (tf != null) markFieldError(tf);
  }
}

void updateReferenceData(GPlot plot,
                         ArrayList<Textfield> timeFields,
                         ArrayList<Textfield> speedFields,
                         ArrayList<Float> actualTimes,
                         ArrayList<Float> referenceSpeedsForMSE) {

  ArrayList<Float> durations = new ArrayList<Float>();
  ArrayList<Float> speeds    = new ArrayList<Float>();

  boolean hasError = false;

  // 1. Read and basic-validate input --------------------
  for (int i = 0; i < timeFields.size(); i++) {
    Textfield tf = timeFields.get(i);
    Textfield sf = speedFields.get(i);
    if (tf == null || sf == null) return;

    String tStr = tf.getText();
    String sStr = sf.getText();

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
      float d = Float.parseFloat(tStr.trim());
      float s = Float.parseFloat(sStr.trim());

      if (d <= 0) {
        markFieldError(tf);
        hasError = true;
      } else {
        durations.add(d);
        speeds.add(s);
        if(i != timeFields.size() - 1) speeds.add(s);
      }
    } catch (NumberFormatException ex) {
      markFieldError(tf);
      markFieldError(sf);
      hasError = true;
    }
  }

  if (hasError || durations.isEmpty()) return;
  if(speeds.size() > 0) speeds.add(speeds.get(speeds.size() - 1));

  // 2. Build cumulative times, enforce sum ≈ 60 ---------
  final float T_END = 60.0f;
  final float EPS   = 0.01f;

  ArrayList<Float> times = new ArrayList<Float>();
  float sum = 0.0f;
  times.add(sum);
  for (int i = 0; i < durations.size(); i++) {
    sum += durations.get(i);
    times.add(sum);
    if(i != durations.size() - 1) times.add(sum);
  }
  
  

  // Check total length ~ 60
  if (abs(sum - T_END) > EPS) {
    // Mark all time fields as error if total != 60
    for (Textfield tf : timeFields) {
      if (tf != null) markFieldError(tf);
    }
    return;
  }
  
  ////DEBUG: PRINT POINTS
  //for(int i = 0; i < times.size(); i++){
  //  print("(" + times.get(i) + ", " + speeds.get(i) + ") "); 
  //}

  //print('\n');
  
  // 4. Update plot --------------------------------------
  GPointsArray points = makeGPointsArray(times, speeds);
  plot.getLayer(REFERENCE).setPoints(points);



  if (actualTimes.size() > 0) {
    referenceSpeedsForMSE.clear();
  
    for (int i = 0; i < actualTimes.size(); i++) {
      float t = actualTimes.get(i);
      int idx = 0;
  
      while (idx + 1 < times.size() && times.get(idx + 1) <= t) {
        idx++;
      }
  
      idx = min(idx, speeds.size() - 1);
      referenceSpeedsForMSE.add(speeds.get(idx));
    }
  }
}

boolean floatEqual(Float a, Float b) {
  if (a == null && b == null) return true;
  if (a == null || b == null) return false;
  return a.equals(b);
}
