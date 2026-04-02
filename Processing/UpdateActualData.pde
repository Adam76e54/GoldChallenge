void updateActualData(GPlot plot,
                         ArrayList<Float> actualTimes,
                         ArrayList<Float> actualSpeeds){

  GPointsArray points = makeGPointsArray(actualTimes, actualSpeeds);
  
  plot.getLayer(ACTUAL).setPoints(points);
                           
}
