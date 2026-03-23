import processing.net.*;
import controlP5.*;

void setup(){
  size(1200, 800);
  noStroke();
  rectMode(CENTER);
  
  ////connect to sam
  //sam = new Client(this, IP, PORT);
  //if( sam != null && sam.active()){
  //  //sam.write("Connected");//this is actually essential. WiFiServer::available() only pulls in client who have data waiting so we need to write something to be seen
  //}
  
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
  
}
