void updateActualData(GPlot plot,
                         ArrayList<Float> actualTimes,
                         ArrayList<Float> actualSpeeds){

  GPointsArray points = makeGPointsArray(actualTimes, actualSpeeds);
  
  if(points != null) plot.getLayer(ACTUAL).setPoints(points);
                           
}
