interface CommandHandler {
  void handle(String payload); 
}

void read(Client sam) {
// read() we call this inside draw() to get info back from sam
  if (sam == null || !sam.active()) return;

  String line = sam.readStringUntil('\n');
  if (line == null) return;

  //print(line);
  line = trim(line);

  int firstColon = line.indexOf(':');
  if (firstColon == -1 || firstColon == line.length() - 1) return;

  String cmd = line.substring(0, firstColon);
  String payload = trim(line.substring(firstColon + 1));

  CommandHandler h = handlers.get(cmd.charAt(0));
  
  if (h != null) {
    h.handle(payload);
  } else {
     println("Unknown command: " + cmd);
  }
}


void setupCommandHandlers(){
// setupCommandHandlers() we call this in setup() to define our comms
// It's basically just placing functions in a char -> function hashtable
// It makes read() more legible because it's just fethcing a handler from a table and executing it

  handlers.put(comm_LEFT_IR, new CommandHandler() {
    public void handle(String payload){
      // we can put function in here and have the hash figure it out in void read()
      leftIR.setText("Left IR sensor = " + payload);
    }
  });
  
  handlers.put(comm_RIGHT_IR, new CommandHandler() {
    public void handle(String payload){
      // we can put function in here and have the hash figure it out in void read()
      rightIR.setText("Right IR sensor = " + payload);
    }
  });
  
  handlers.put(comm_ACTUAL_TIME, new CommandHandler(){
    public void handle(String payload){
      
      int colon = payload.indexOf(':');
      if (colon <= 0 || colon >= payload.length() - 1) return;
      
      String strIdx = payload.substring(0, colon);
      String strValue = payload.substring(colon + 1);
      
      int idx = parseInt(strIdx);
      float value = parseFloat(strValue);
      
      if(idx < actualTimes.size()){
        actualTimes.set(idx, value); 
      } else {
        actualTimes.add(value); 
      }
      
    }
  });
  
  handlers.put(comm_ACTUAL_SPEED, new CommandHandler(){
    public void handle(String payload){

      int colon = payload.indexOf(':');
      if (colon <= 0 || colon >= payload.length() - 1) return;
      
      String strIdx = payload.substring(0, colon);
      String strValue = payload.substring(colon + 1);
      
      int idx = parseInt(strIdx);
      float value = parseFloat(strValue);
      
      if(idx < actualTimes.size()){
        actualSpeeds.set(idx, value); 
      } else {
        actualSpeeds.add(value); 
      }
      
    }
  });
  
}
