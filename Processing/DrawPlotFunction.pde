void drawPlot(){
  plot.beginDraw();
  plot.drawBox();
  plot.drawBox();
  plot.drawGridLines(2);  
  plot.drawXAxis();
  plot.drawYAxis();
  plot.drawTitle();
  plot.drawPoints();
  plot.setGridLineColor(color(220));  
  plot.getLayer(REFERENCE).drawLines();
  plot.getLayer(ACTUAL).drawLines();
  plot.endDraw();
}
