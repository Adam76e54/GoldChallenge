void setupClient(Client sam){
  sam = new Client(this, IP, PORT);
  if(sam.active()){
    sam.write("Connected");//this is actually essential. WiFiServer::available() only pulls in client who have data waiting so we need to write something to be seen
  }
}

void keep(Client sam){
  //if (sam == null || !sam.active()) {
  //  if (frameCount % 120 == 0) {
  //    sam = new Client(this, IP, PORT);
  //    if (sam != null && sam.active()) {
  //      sam.write("hi\n");
  //    }
  //  }
  //}

}
