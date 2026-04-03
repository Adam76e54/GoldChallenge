Button makeButton(ControlP5 controller, String name, int x, int y, int w, int h, String label){
  Button button = controller.addButton(name);//attach function name to button
  button.setPosition(x, y)
    .setSize(w,h)
    .setLabel(label)    
    .setFont(createFont("Arial", 14));
;
  return button;
}

Textfield makeTextfield(ControlP5 controller, String name, int x, int y, int w, int h, String label){
  Textfield field = controller.addTextfield(name)
    .setPosition(x, y)
    .setSize(w, h)
    .setAutoClear(false)
    .setInputFilter(ControlP5.FLOAT)
    .setLabel(label)
    .setFont(createFont("Arial", 30))
    .setText("0");
    
    field.getCaptionLabel()
      .setColor(color(0))
      .setFont(createFont("Arial", 14))
      .align(ControlP5.CENTER, ControlP5.BOTTOM_OUTSIDE);

  return field;
}

Textlabel makeTextlabel(ControlP5 controller, String name, int x, int y, String text){
  Textlabel label = controller.addTextlabel(name)
    .setPosition(x, y)
    .setText(text)
    .setColor(color(0))
    .setFont(createFont("Arial", 14));
  
  return label;
}

Slider makeSlider(ControlP5 controller, String name, int x, int y, int w, int h,
                  float start, float end, String label){
  Slider slider = controller.addSlider(name)
    .setPosition(x,y)
    .setSize(w,h)
    .setRange(start, end)
    .setTriggerEvent(Slider.RELEASE);

  // Caption (static text)
  slider.getCaptionLabel()
    .setColor(color(0))
    .setFont(createFont("Arial", 14))
    .setText(label);

  // Value label (numeric value)
  slider.getValueLabel()
    .setColor(color(0))                  // black
    .setFont(createFont("Arial", 12));   // optional: tweak size

  return slider;
}

<T> ScrollableList makeScrollableList(ControlP5 controller, String name, ArrayList<Character> optionNames, ArrayList<T> optionValues, int x, int y, int w, int h, String label){
 ScrollableList list = controller
                     .addScrollableList(name)
                     .setPosition(x,y)
                     .setBarHeight(h)
                     .setSize(w, h * optionNames.size() + h) 
                     .setItemHeight(h)
                     .setFont(createFont("Arial", 16))
                     .setLabel(label);
 
  for(int i = 0; i < optionNames.size(); i++){
     list.addItem(String.valueOf(optionNames.get(i)), optionValues.get(i));
  }
 
 return list;
}

GPlot makeGPlot(String title, int x, int y, int w, int h, String xTitle, String yTitle){
  GPlot plot = new GPlot(this);
  
  plot.setPos(x,y);
  plot.setDim(w,h);
  plot.setTitleText(title);
  plot.getXAxis().setAxisLabelText(xTitle);
  plot.getYAxis().setAxisLabelText(yTitle);
  return plot;
}

GPointsArray makeGPointsArray(ArrayList<Float> x, ArrayList<Float> y){
  GPointsArray array = new GPointsArray();
  
  if(x.size() != y.size() || x.size() == 0 || y.size() == 0){
    return null;
  }
  
  for(int i = 0; i < x.size(); i++){
    array.add(x.get(i), y.get(i));
  }

  return array;
}

Toggle makeToggle(ControlP5 controller, String name,
                  int x, int y, int w, int h, String label){

  Toggle toggle = controller.addToggle(name)
                            .setPosition(x, y)
                            .setSize(w, h)
                            .setCaptionLabel(label);

  toggle.getCaptionLabel()
        .align(ControlP5.CENTER, ControlP5.CENTER)
        .setPaddingX(0)
        .setPaddingY(0)
        .setFont(createFont("Arial", 16));

  // Deep red when ON (stopped = true)
  toggle.setColorForeground(color(150, 0, 0));   // knob/outline when ON
  toggle.setColorActive(color(200, 0, 0));       // fill when ON

  // Deep green when OFF (stopped = false)
  toggle.setColorBackground(color(0, 90, 0));    // background when OFF

  toggle.setValue(true);   // start in "stopped" state (red)

  return toggle;
}
