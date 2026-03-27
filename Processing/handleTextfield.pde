void handleTextfield(Textfield tf){
    String name = tf.getName();
    float value = Float.parseFloat(tf.getText().trim());
    print(name + comm_DELIMITER + value + comm_END);
    
    if(sam != null && sam.active()){
      sam.write(name + comm_DELIMITER + value + comm_END);
      
    }
}
