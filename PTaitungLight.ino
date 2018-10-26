#define PIN_LIGHT 9

const float VEL=1.0/2000;
const float BLINK_LOW=20;
const float BLINK_HIGH=255;

const float SLEEP_TIME=2000;
const float SMOOTH=0.5; //larger for smooth

int _mode=1; // 0:sleep, 1:processing, 2:poem

bool _lightup;
float _pos_blink;
float _vel_blink;

float _tlast;

float _val_volume;

float _dsleep;


void setup(){
  Serial.begin(19200);
  _tlast=millis();
  setMode(0);
  _val_volume=0;
}

void loop(){

    checkSerial();

    float dt_=millis()-_tlast;

    if(_mode==0){
      float val_=easeInOut(_pos_blink);
      //Serial.println(val_);    
      analogWrite(PIN_LIGHT,val_*(BLINK_HIGH-BLINK_LOW)+BLINK_LOW);
      
      if(_lightup){
        _pos_blink+=_vel_blink*dt_;
        if(_pos_blink>1.0) _lightup=false;            
      }else{
        _pos_blink-=_vel_blink*dt_;
        if(_pos_blink<0) _lightup=true;            
      }
    }else if(_mode==1){
      analogWrite(PIN_LIGHT,_val_volume);      
    }

     
     if(_dsleep<=0) setMode(0);
     else _dsleep-=dt_;
     
    _tlast+=dt_;
    
}


void checkSerial(){
  if(Serial.available()>0){
    unsigned char read_=Serial.read();    
    int num=(int)read_;
    if(num!=0){
      setMode(1);
      _dsleep=SLEEP_TIME;

      
      Serial.println(num);
      _val_volume*=SMOOTH;
      _val_volume+=(float)num*(1.0-SMOOTH);
    }
  }
}

void setMode(int set_){
  if(_mode==set_) return;
  Serial.print("set mode= ");
  Serial.println(set_);
  switch(set_){
    case 0:
      _pos_blink=(float)_val_volume/255.0;
      _vel_blink=VEL;            
      break;
    case 1:      
      _val_volume=easeInOut(_pos_blink)*(BLINK_HIGH-BLINK_LOW)+BLINK_LOW;
      break;
  }
  _mode=set_;
}

float easeInOut(float t){
  if((t/=.5)<1) return .5*t*t*t;
  return .5*((t-=2)*t*t+2);  
}
