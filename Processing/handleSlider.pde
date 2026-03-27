void handleSlider(Slider s){
  float value = s.getValue();
  String name = s.getName();
  print(name + comm_DELIMITER + value + comm_END);
  if(sam != null && sam.active()){
    sam.write(name + comm_DELIMITER + value + comm_END);
  }
}
