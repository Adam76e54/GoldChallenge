import processing.net.*;
import controlP5.*;
import java.util.Arrays;
import java.util.ArrayList;
import grafica.*;

void setup(){
  size(1200, 800);
  noStroke();
  rectMode(CENTER);
  
  //connect to sam
  if(sam != null) setupClient(sam);
  
  panel = new ControlP5(this);
  
  plot = makeGPlot("Chart", 250, 200, 600, 400, "Time (s)", "Speed (cm/s)");

  plot.addLayer(REFERENCE, new GPointsArray());
  plot.getLayer(REFERENCE).setLineColor(color(0, 114, 189));
  plot.getLayer(REFERENCE).setLineWidth(2);
  
  plot.setXLim(0, 60);
  plot.setYLim(0, 50);
  
  int timesX = 100, timesY = 10;
  int size = 60;
  
  turningTextfield = makeTextfield(panel, 
                                   Character.toString(comm_TURNING_FACTOR),
                                   1040, 
                                   500, 
                                   size, size, "Turning Factor");
                                   
  for(int i = 0; i < sizeOfArrays; i++){
    String label = "" + i;
    
    Textfield field = makeTextfield(panel, 
                                    Character.toString(comm_TIMES.get(i)), 
                                    timesX + i*size, 
                                    timesY, 
                                    size, size, label);
    
    timeTextfields.add(field);
  }
  
  for(int i = 0; i < sizeOfArrays; i++){
    String label = "" + i;
    
    Textfield field = makeTextfield(panel, 
                                    Character.toString(comm_SPEEDS.get(i)), 
                                    timesX + i*size, 
                                    timesY + 2*size, 
                                    size, size, label);
    
    speedTextfields.add(field);
  }
  
  for(int i = 0; i < 3; i++){
    String name = Character.toString(PID[i]), label = "";
    
    switch(i){
      case 0:
        label = "P";
      break;
      
      case 1:
        label = "I";
      break;
      
      case 2:
        label = "D";
      break;
    }  
    
    Textfield field = makeTextfield(panel, 
                                    name, 
                                    timesX + i*size, 
                                    timesY + 4*size, 
                                    size, size, label);
    
    
    PIDTextfields.add(field);
    
    setupCommandHandlers();
  }

  timeTextlabel = makeTextlabel(panel, "__TimeTextlabel", 15, timesY + 5, "Times");
  speedTextlabel = makeTextlabel(panel, "__SpeedTextlabel", 15, timesY + 5 + 2*size, "Speeds");
  PIDTextlabel = makeTextlabel(panel, "__PIDTextlabel", 15, timesY + 5 + 4*size, "Constants");

  leftIR = makeTextlabel(panel, Character.toString(comm_LEFT_IR_IN), 1000, 50, "Left IR sensor = 0");
  rightIR = makeTextlabel(panel, Character.toString(comm_RIGHT_IR_IN), 1000, 80, "Right IR sensor = 0");
  
  leftSlider = makeSlider(panel, Character.toString(comm_LEFT_IR), 1020, 120, 30, 300, 0, 1, "Left");  
  leftSlider.setValue(0.3);
  rightSlider = makeSlider(panel, Character.toString(comm_RIGHT_IR), 1090, 120, 30, 300, 0, 1, "Right");  
  rightSlider.setValue(0.3);
  
  stopToggle = makeToggle(panel, Character.toString(comm_STOP_TOGGLE), 540, 100, 150, 70, "Stop/Start");
  
  setupCommandHandlers();
}

void draw(){
  keep(sam);
  
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
  
  clearTextfield(timeTextfields, timeFocuses);
  clearTexfield(speedTextfields, speedFocuses);
  clearTextfield(PIDTextfields, PIDFocuses);
  read(sam);
  updateReferenceData(plot, timeTextfields, speedTextfields);
}
