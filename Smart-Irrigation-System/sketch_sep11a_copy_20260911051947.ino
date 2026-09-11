void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
 int rainProbability = 80;  
 int expectPrecipitation = 6; 
 int rainProbability2 = 70;  
 int expectPrecipitation2 = 1;  
 int senzor2 = 50;  
 int value1 = analogRead(A0); 
 int percent1 = map(value1, 0, 1023, 0, 100);  
 int average = (percent1 + senzor2) / 2;
 Serial.print("Average moisture: ");  
 Serial.print(average);   
 Serial.println("%");
 class IrrigationEngine{
  public:
  int computeBaseWaterAmount (int average){
    if(average<40){
      return 1500;
    } else if(average<50){
      return 1200;
    } else if(average<70){
      return 600;
    } else{
      return 0;
    }
  }
 };
 IrrigationEngine engine;
 int baseWaterAmount = 
  engine.computeBaseWaterAmount(average);
 int waterAmount = baseWaterAmount - (expectPrecipitation * rainProbability / 100) - (expectPrecipitation2 * rainProbability2 / 100); 
 if (waterAmount <= 0){    
  waterAmount = 0;    
  digitalWrite(LED_BUILTIN, LOW);     
  Serial.print("Final water amount: ");  
  Serial.print(waterAmount);    
  Serial.println("l");  
 } else {    
  digitalWrite(LED_BUILTIN, HIGH);    
  Serial.print("Final water amount: ");  
  Serial.print(waterAmount);    
  Serial.println(" l");   
 }   
 delay(10000);

}
