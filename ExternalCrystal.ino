volatile boolean timer2_interrupt_flag = false;

void setupExternalCrystal(void){
  ASSR = _BV(AS2);      // use the external crystal as an asynchronous clock source
  TCNT2 = 0;
  while(ASSR & _BV(TCN2UB));  
  TCCR2A = _BV(WGM21);  // prescaler=64, enable CTC mode
  while(ASSR & _BV(TCR2AUB));
  OCR2A = 128;          // compare match every 128 ticks (32768 / 64 = 512Hz; 128 ticks / 512 Hz = 0.25s)
  while(ASSR & _BV(OCR2AUB));  
  TCCR2B = _BV(CS22);   // start the timer running 
  while(ASSR & _BV(TCR2BUB));  
  TIMSK2 = _BV(OCIE2A); // enable compare match ISR  
}

void testExternalCrystal(void){
  static boolean task_enabled_first_time = true; 
  static uint8_t led_state = 0;
  
  if(testExternalCrystal_enabled){
    if(task_enabled_first_time){
      Serial.println(F("External Crystal Test Started."));
      setupExternalCrystal();
      task_enabled_first_time = false; 
    }
    
    if(timer2_interrupt_flag){
      led_state ^= 1;
      pinMode(6, OUTPUT);   // set LED to output
      digitalWrite(6, led_state);
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
        timer2_interrupt_flag = false;
      }
    }      
  }
  else{
    task_enabled_first_time = true;
    TIMSK2 = 0; // disable compare match ISR    
  }
}

ISR(TIMER2_COMPA_vect){
  timer2_interrupt_flag = true;
}
