void handleToggle(Toggle t){
  Boolean stopped = t.getBooleanValue();
  String value = stopped ? "1" : "0";   
  String name = t.getName();
  print(name + comm_DELIMITER + value + comm_END);

  if(sam != null && sam.active()){
    sam.write(comm_STOP_TOGGLE + comm_DELIMITER + value + comm_END);
  }
}
