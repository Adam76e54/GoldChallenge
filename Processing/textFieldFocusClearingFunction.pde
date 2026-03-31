void clearTextfield(ArrayList<Textfield> textfields, ArrayList<Boolean> wasFocused){
  for(int i = 0; i < textfields.size(); i++){
    Textfield tf = textfields.get(i);
    Boolean nowFocused = tf.isFocus();
    
    if(nowFocused && !wasFocused.get(i)){
      tf.clear();
    }
    
    wasFocused.set(i, nowFocused);
  }
}
