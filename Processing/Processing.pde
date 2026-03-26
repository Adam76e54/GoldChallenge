import processing.net.*;
import controlP5.*;
import java.util.Arrays;
import java.util.ArrayList;
import grafica.*;

void setup(){
  size(1200, 800);
  noStroke();
  rectMode(CENTER);
  
  ////connect to sam
  //sam = new Client(this, IP, PORT);
  //if( sam != null && sam.active()){
  //  //sam.write("Connected");//this is actually essential. WiFiServer::available() only pulls in client who have data waiting so we need to write something to be seen
  //}
  
  panel = new ControlP5(this);
  
  plot = makeGPlot("Chart", 250, 200, 600, 400, "Time (s)", "Speed (cm/s)");

  plot.addLayer(REFERENCE, new GPointsArray());
  plot.getLayer(REFERENCE).setLineColor(color(0, 114, 189));
  plot.getLayer(REFERENCE).setLineWidth(2);
  
  plot.setXLim(0, 60);
  plot.setYLim(0, 50);
  
  int timesX = 100, timesY = 10;
  int size = 60;
  
  for(int i = 0; i < sizeOfArrays; i++){
    String name = "Time" + i;
    String label = "" + i;
    
    Textfield field = makeTextfield(panel, name, timesX + i*size, timesY, size, size, label);
    
    timeTextfields.add(field);
  }
  
  for(int i = 0; i < sizeOfArrays; i++){
    String name = "Speed" + i;
    String label = "" + i;
    
    Textfield field = makeTextfield(panel, name, timesX + i*size, timesY + 2*size, size, size, label);
    
    speedTextfields.add(field);
  }
  
  for(int i = 0; i < 3; i++){
    String name = "", label = "";
    
    switch(i){
      case 0:
        name = "KP";
        label = "P";
      break;
      
      case 1:
        name = "KI";
        label = "I";
      break;
      
      case 2:
        name = "KD";
        label = "D";
      break;
    }  
    
    Textfield field = makeTextfield(panel, name, timesX + i*size, timesY + 4*size, size, size, label);
    
    
    PIDTextfields.add(field);
  }

  timeTextlabel = makeTextlabel(panel, "__TimeTextlabel", 15, timesY + 5, "Times");
  speedTextlabel = makeTextlabel(panel, "__SpeedTextlabel", 15, timesY + 5 + 2*size, "Speeds");
  PIDTextlabel = makeTextlabel(panel, "__PIDTextlabel", 15, timesY + 5 + 4*size, "Constants");

  leftIR = makeTextlabel(panel, "LeftIR", 1000, 50, "Left IR sensor = 0");
  rightIR = makeTextlabel(panel, "RightIR", 1000, 80, "Right IR sensor = 0");
  
  leftSlider = makeSlider(panel, "LeftSlider", 1020, 120, 30, 300, 0, 1, "Left");  
  rightSlider = makeSlider(panel, "RightSlider", 1070, 120, 30, 300, 0, 1, "Right");  
  
  stopToggle = makeToggle(panel, "StopToggle", 540, 100, 150, 70, "Stop/Start");
}

void draw(){
  //if(sam == null || !sam.active()){
  //  if(frameCount % 120 == 0){
  //    sam = new Client(this, IP, PORT);
  //  }
  //}
  
  background(255);
  fill(0);
  textSize(32);
  textAlign(CENTER,CENTER);
  
  background(255);
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
  plot.endDraw();
  
  updateReferenceData(plot, timeTextfields, speedTextfields);
}
