void controlEvent(ControlEvent e){
  Controller c = e.getController();
  
  if(c instanceof Textfield){
    Textfield tf = (Textfield)c;
    handleTextfield(tf);
  }
  
  if(c instanceof Slider){
     Slider s = (Slider)c;
     handleSlider(s);
  }
  
  if(c instanceof Toggle){
    Toggle t = (Toggle)c;
    handleToggle(t);
  }
}

void handleTextfield(Textfield tf){
    String name = tf.getName();
    float value = Float.parseFloat(tf.getText().trim());
    print(name + comm_DELIMITER + value + comm_END);
    
    if(sam != null && sam.active()){
      sam.write(name + comm_DELIMITER + value + comm_END);
      
    }
}

void handleSlider(Slider s){
  float value = s.getValue();
  String name = s.getName();
  print(name + comm_DELIMITER + value + comm_END);
  if(sam != null && sam.active()){
    sam.write(name + comm_DELIMITER + value + comm_END);
  }
}

void handleToggle(Toggle t){
  Boolean stopped = t.getBooleanValue();
  String value = stopped ? "1" : "0";   
  String name = t.getName();
  print(name + comm_DELIMITER + value + comm_END);

  if(sam != null && sam.active()){
    sam.write(name + comm_DELIMITER + value + comm_END);
  }
}
