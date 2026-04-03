float MeanSquaredError(ArrayList<Float> data, ArrayList<Float> reference){
  float mse = 0;
  
  if(data.size() != reference.size()){
    return mse;
  }
  
  for(int i = 0; i < data.size(); i++){
    
    mse += (data.get(i) - reference.get(i)) * (data.get(i) - reference.get(i));
    
  }
  
  return mse;
}
