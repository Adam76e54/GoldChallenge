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
