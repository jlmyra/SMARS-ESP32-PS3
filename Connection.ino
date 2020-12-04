

//*******************Check controller Connection********************************
 
void onConnection() {
   
  if (Ps3.isConnected()) {
    Serial.println("Controller connected.");
  }
}

//*******************END Check controller Connection*******************************
