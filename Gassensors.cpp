#include "Gassensors.h"
//#define DEBUG
Gassensors::Gassensors(uint8_t pin, uint8_t type,uint8_t _delay, uint8_t _iterations) {
  _pin = pin;
  _type = type;
  READ_SAMPLE_INTERVAL=_delay;
  READ_SAMPLE_TIMES=_iterations;
}
/*void Gassensors::setParameters(uint8_t _delay, uint8_t _iterations)
{
  READ_SAMPLE_INTERVAL=_delay;
  READ_SAMPLE_TIMES=_iterations;
}
*/
  /****************** MQResistanceCalculation ****************************************
  Input:   raw_adc - raw value read from adc, which represents the voltage
  Output:  the calculated sensor resistance
  Remarks: The sensor and the load resistor forms a voltage divider. Given the voltage
         across the load resistor and its resistance, the resistance of the sensor
         could be derived.
************************************************************************************/
float Gassensors::MQResistanceCalculation(int raw_adc)
{
  /*Serial.print(F("printing raw adc in mqresistancecalc: "));
  Serial.println(raw_adc);
  Serial.print(F("printing RL_value in mqresistancecalc: "));
  Serial.println(RL_VALUE);
  Serial.print(F("printing Rs value in mqresistancecalc: "));
  Serial.println(((float)RL_VALUE * (1023 - raw_adc) / raw_adc));*/
  return ( ((float)RL_VALUE * (1023 - raw_adc) / raw_adc));
}
/*****************************  MQRead *********************************************
  Input:   mq_pin - analog channel
  Output:  Rs of the sensor
  Remarks: This function use MQResistanceCalculation to caculate the sensor resistenc (Rs).
         The Rs changes as the sensor is in the different consentration of the target
         gas. The sample times and the time interval between samples could be configured
         by changing the definition of the macros.
************************************************************************************/
float Gassensors::MQRead(int mq_pin)
{
  int i;
  float rs = 0;

  for (i = 0; i < READ_SAMPLE_TIMES; i++) {
   // Serial.print(F("printing raw adc in mqread: "));
   // Serial.println(analogRead(mq_pin));
    rs += MQResistanceCalculation(analogRead(mq_pin));
    delay(READ_SAMPLE_INTERVAL);
  //  Serial.print(F("printing rs value in mqread: "));
   // Serial.println(rs);    
  }
 
  rs = rs / READ_SAMPLE_TIMES;
  //Serial.print(F("rs value in mq read:  "));
 // Serial.println(rs);
  return rs;
}

/*****************************  MQGetGasPercentage **********************************
  Input:   rs_ro_ratio - Rs divided by Ro
         gas_id      - target gas type
  Output:  ppm of the target gas
  Remarks: This function uses different equations representing curves of each gas to
         calculate the ppm (parts per million) of the target gas(or mg/L (milligrams
         per litre) in case of MQ3 sensor.
************************************************************************************/
int Gassensors::MQGetGasPercentage(float rs_ro_ratio, int gas_id )
{
  switch (_type) {

    case MQ2:
    //Serial.println("calculating mq2 co gases"); 
	 #ifdef DEBUG
              Serial.print("rs_ro_ratio:  ");                                                        //debug statements that display step by step calculation of ppm
              Serial.println(rs_ro_ratio);
			  #endif
              /* Serial.print("log of rs_ro_ratio: ");
               Serial.println(log10(rs_ro_ratio));
               Serial.print("3.891* log of rs_ro_ratio: ");
               Serial.println((-3.891 * (log10(rs_ro_ratio))));
               Serial.print("pow of 10, 3.891* log of rs_ro_ratio: ");
               Serial.println(pow(10, ((-3.891 * (log10(rs_ro_ratio))))));
               Serial.print("2.75+pow of 10, 3.891* log of rs_ro_ratio: ");
               Serial.println((pow(10, ((-3.891 * (log10(rs_ro_ratio))) + 2.750))));*/
     // Serial.print(F("calculating mq2 gases: xxxxxx     "));
      //Serial.print(gas_id);
         //   Serial.print(F("gas_id      "));

         if ( accuracy == 0 ) {
		  if ( gas_id == GAS_HYDROGEN ) {
			return (pow(10,((-2.109*(log10(rs_ro_ratio))) + 2.983)));
		  } else if ( gas_id == GAS_LPG ) {
			return (pow(10,((-2.123*(log10(rs_ro_ratio))) + 2.758)));
		  } else if ( gas_id == GAS_METHANE ) {
			return (pow(10,((-2.622*(log10(rs_ro_ratio))) + 3.635)));
		  } else if ( gas_id == GAS_CARBON_MONOXIDE ) {
			return (pow(10,((-2.955*(log10(rs_ro_ratio))) + 4.457)));
		  } else if ( gas_id == GAS_ALCOHOL ) {
			return (pow(10,((-2.692*(log10(rs_ro_ratio))) + 3.545)));
		  } else if ( gas_id == GAS_SMOKE ) {
			return (pow(10,((-2.331*(log10(rs_ro_ratio))) + 3.596)));
		  } else if ( gas_id == GAS_PROPANE ) {
			return (pow(10,((-2.174*(log10(rs_ro_ratio))) + 2.799)));
		  }    
		} 

		  else if ( accuracy == 1 ) {
			if ( gas_id == GAS_HYDROGEN ) {
			return (pow(10,((-2.109*(log10(rs_ro_ratio))) + 2.983)));
		  } else if ( gas_id == GAS_LPG ) {
			return (pow(10,((-2.123*(log10(rs_ro_ratio))) + 2.758)));
		  } else if ( gas_id == GAS_METHANE ) {
			return (pow(10,((-2.622*(log10(rs_ro_ratio))) + 3.635)));
		  } else if ( gas_id == GAS_CARBON_MONOXIDE ) {
			return (pow(10,((-2.955*(log10(rs_ro_ratio))) + 4.457)));
		  } else if ( gas_id == GAS_ALCOHOL ) {
			return (pow(10,((-2.692*(log10(rs_ro_ratio))) + 3.545)));
		  } else if ( gas_id == GAS_SMOKE ) {
			return (pow(10,(-0.976*pow((log10(rs_ro_ratio)), 2) - 2.018*(log10(rs_ro_ratio)) + 3.617)));
		  } else if ( gas_id == GAS_PROPANE ) {
			return (pow(10,((-2.174*(log10(rs_ro_ratio))) + 2.799)));
		  }
		}    
      break;
 
    case MQ3:
      //Serial.print(F("calculating mq3 gases"));
        if ( accuracy == 1 ) {
        if ( gas_id == GAS_ALCOHOL && rs_ro_ratio<=2.31 && rs_ro_ratio>=0.113 ) {
          return (pow(10, ((-1.487 * (log10(rs_ro_ratio))) - 0.401)));
        }
      }
      break;

    case MQ4:
      //Serial.print(F("calculating mq4 gases"));
	   #ifdef DEBUG
               Serial.print("rs_ro_ratio:  ");                                                        //debug statements that display step by step calculation of ppm
               Serial.println(rs_ro_ratio);
			   #endif
        if ( accuracy == 1 ) {
        if ( gas_id == GAS_METHANE && rs_ro_ratio<=1.76 && rs_ro_ratio>=0.43 ) {
          return (pow(10, ((-2.849 * (log10(rs_ro_ratio))) + 2.997)));
        }
      }
      break;

    case MQ5:
      //Serial.print(F("calculating mq5 gases"));
	   #ifdef DEBUG
               Serial.print("rs_ro_ratio:  ");                                                        //debug statements that display step by step calculation of ppm
               Serial.println(rs_ro_ratio);
			   #endif
        if ( accuracy == 0 ) {
		  if ( gas_id == GAS_HYDROGEN ) {
			return (pow(10,((-3.986*(log10(rs_ro_ratio))) + 3.075)));
		  } else if ( gas_id == GAS_LPG ) {
			return (pow(10,((-2.513*(log10(rs_ro_ratio))) + 1.878)));
		  } else if ( gas_id == GAS_METHANE ) {
			return (pow(10,((-2.554*(log10(rs_ro_ratio))) + 2.265 )));
		  } else if ( gas_id == GAS_CARBON_MONOXIDE ) {
			return (pow(10,((-6.900*(log10(rs_ro_ratio))) + 6.241)));
		  } else if ( gas_id == GAS_ALCOHOL ) {
			return (pow(10,((-4.590*(log10(rs_ro_ratio))) + 4.851)));
		  }   
		} 

		  else if ( accuracy == 1 ) {
			if ( gas_id == GAS_HYDROGEN ) {
			return (pow(10,(-22.89*pow((log10(rs_ro_ratio)), 3) + 8.873*pow((log10(rs_ro_ratio)), 2) - 3.587*(log10(rs_ro_ratio)) + 2.948)));
		  } else if ( gas_id == GAS_LPG ) {
			return (pow(10,((-2.513*(log10(rs_ro_ratio))) + 1.878)));
		  } else if ( gas_id == GAS_METHANE ) {
			return (pow(10,(-0.428*pow((log10(rs_ro_ratio)), 2) - 2.867*(log10(rs_ro_ratio)) + 2.224)));
		  } else if ( gas_id == GAS_CARBON_MONOXIDE ) {
			return (pow(10,(1401*pow((log10(rs_ro_ratio)), 4) - 2777*pow((log10(rs_ro_ratio)), 3) + 2059*pow((log10(rs_ro_ratio)), 2) - 682.5*(log10(rs_ro_ratio)) + 88.81)));
		  } else if ( gas_id == GAS_ALCOHOL ) {
			return (pow(10,(14.90*pow((log10(rs_ro_ratio)), 3) - 19.26*pow((log10(rs_ro_ratio)), 2) + 3.108*(log10(rs_ro_ratio)) + 3.922)));
		  }
		}    
      break;

    case MQ6:
      //Serial.print(F("calculating mq6 gases"));
	  #ifdef DEBUG
               Serial.print("rs_ro_ratio:  ");                                                        //debug statements that display step by step calculation of ppm
               Serial.println(rs_ro_ratio);
	  #endif
      if ( accuracy == 1 ) {
        if ( gas_id == GAS_LPG && rs_ro_ratio<=2.05 && rs_ro_ratio>=0.39) {
          return (pow(10, ((-2.351 * (log10(rs_ro_ratio))) + 3.014)));
        }
       }
      break;

    case MQ7:
      //Serial.print(F("calculating mq7 gases"));
      if ( accuracy == 1 ) {
        if ( gas_id == GAS_CARBON_MONOXIDE && rs_ro_ratio<= 1.59 && rs_ro_ratio>= 0.09) {
          return (pow(10, ((-1.525 * (log10(rs_ro_ratio))) + 1.994)));
        } 
      }
      break;

    case MQ8:
     //Serial.print(F("calculating mq8 gases"));
      if ( accuracy == 1 ) {
        if ( gas_id == GAS_HYDROGEN && rs_ro_ratio<=0.25 && rs_ro_ratio>=0.04) {
          return (pow(10, ((-2.568 * (log10(rs_ro_ratio))) + 0.360)));
        }
      }
      break;

    case MQ9:
      //Serial.print(F("calculating mq9 gases"));
	   #ifdef DEBUG
               Serial.print("rs_ro_ratio:  ");                                                        //debug statements that display step by step calculation of ppm
               Serial.println(rs_ro_ratio);
			   #endif
      if ( accuracy == 1 ) {
        if ( gas_id == GAS_LPG && rs_ro_ratio<=2.06 && rs_ro_ratio>=0.33) {
          return (pow(10, ((-2.132 * (log10(rs_ro_ratio))) + 2.992)));
        } 
        else if ( gas_id == GAS_CARBON_MONOXIDE && rs_ro_ratio<=1.64 && rs_ro_ratio>=0.78) {
          return (pow(10, ((-2.199 * (log10(rs_ro_ratio))) + 2.766 )));
        } 
        else if ( gas_id == GAS_METHANE && rs_ro_ratio<=3.12 && rs_ro_ratio>=0.70) {
          return (pow(10, (-0.670 * pow((log10(rs_ro_ratio)), 2) - 2.399 * (log10(rs_ro_ratio)) + 3.650)));
        }
      }
	  else if ( accuracy == 0 ) {
		if ( gas_id == GAS_LPG ) {
			return (pow(10,((-2.132*(log10(rs_ro_ratio))) + 2.992)));
		} else if ( gas_id == GAS_CARBON_MONOXIDE ) {
			return (pow(10,((-2.199*(log10(rs_ro_ratio))) + 2.766 )));
		} else if ( gas_id == GAS_METHANE ) {
			return (pow(10,((-2.636*(log10(rs_ro_ratio))) + 3.646)));
		}   
	  } 
      break;

    case MQ135:
      //Serial.print(F("calculating mq135 gases: xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx     "));
     // Serial.print(rs_ro_ratio);
    //  //Serial.print(gas_id);
        //          //Serial.print("gas_id      ");
		 #ifdef DEBUG
               Serial.print("rs_ro_ratio:  ");                                                        //debug statements that display step by step calculation of ppm
               Serial.println(rs_ro_ratio);
			   #endif
        if ( accuracy == 1 ) {
         /*Serial.println("calculating mq135 co gases"); 
               Serial.print("rs_ro_ratio:  ");                                                        //debug statements that display step by step calculation of ppm
               Serial.println(rs_ro_ratio);
               Serial.print("log of rs_ro_ratio: ");
               Serial.println(log10(rs_ro_ratio));
               Serial.print("3.891* log of rs_ro_ratio: ");
               Serial.println((-3.891 * (log10(rs_ro_ratio))));
               Serial.print("pow of 10, 3.891* log of rs_ro_ratio: ");
               Serial.println(pow(10, ((-3.891 * (log10(rs_ro_ratio))))));
               Serial.print("2.75+pow of 10, 3.891* log of rs_ro_ratio: ");
               Serial.println((pow(10, ((-3.891 * (log10(rs_ro_ratio))) + 2.750))));*/
        if ( gas_id == GAS_CARBON_DIOXIDE && rs_ro_ratio<=2.3 && rs_ro_ratio>=0.3) {
          return (pow(10, ((-2.890 * (log10(rs_ro_ratio))) + 2.055)));
        } 
      }
      break;
	
	case MQ137:
     //Serial.print(F("calculating mq8 gases"));
      if ( accuracy == 0 ) {
        if ( gas_id == GAS_AMMONIUM) {
          return (pow(10, (-log10(rs_ro_ratio)) + 1.8572)/1.0791);
        }
      }
      break;
	  
	case MQ136:
     //Serial.print(F("calculating mq8 gases"));
      if ( accuracy == 0 ) {
        if ( gas_id == GAS_H2S) {
          return (pow(10, (-log10(rs_ro_ratio)) - 0.1954)/0.2731);
        }
      }
      break;
  }
  return 0;
}

float Gassensors::rs_ro_ratio(uint8_t pin)
{
  switch (_type) {
    case MQ2:
      /*{Serial.print("   rs2 value now: ");
      float x = MQRead(pin);
      Serial.print(x);
      Serial.print("    r02 value now:  ");
      Serial.print(_Ro2);
       Serial.print("   rs_ro2_ratio now: ");
      Serial.print(x/_Ro2);
      return x/_Ro2;}
      //Serial.print(MQRead(pin) / _Ro2);*/
      return MQRead(pin) / _Rovalues[0];
      break;

    case MQ3:
      return MQRead(pin) / _Rovalues[1];
      break;

    case MQ4:
      /*{
      Serial.print("rs_ro4_ratio now");
      float x = MQRead(pin)/_Ro4;
      Serial.print(x);}*/
      return MQRead(pin) / _Rovalues[2];
      break;

    case MQ5:
      return MQRead(pin) / _Rovalues[3];
      break;

    case MQ6:
      return MQRead(pin) / _Rovalues[4];
      break;

    case MQ7:
      return MQRead(pin) / _Rovalues[5];
      break;

    case MQ8:
      return MQRead(pin) / _Rovalues[6];
      break;

    case MQ9:
      return MQRead(pin) / _Rovalues[7];
      break;
	
	case MQ137:
      return MQRead(pin) / _Rovalues[9];
      break;
	
	case MQ136:
      return MQRead(pin) / _Rovalues[10];
      break;
	  
    case MQ135:
      //Serial.println(F("rs_ro135_ratio"));
       //Serial.println(MQRead(pin));
     // Serial.println(MQRead(pin) / _Rovalues[8]);
      return MQRead(pin)/_Rovalues[8];
      break;

    default:
	 #ifdef DEBUG
      Serial.print(F("rs_ro default case executed"));
	  #endif
	  break;

  }
}
  int Gassensors::gasppm(uint8_t pin, uint8_t type, int mqgases_id)
  {
    return (MQGetGasPercentage(rs_ro_ratio(pin), mqgases_id ));
  }

  int Gassensors::printallgases(uint8_t type)
  {//Serial.println(_Rovalues[_type-2]);
	int conc=0;
    switch (_type) {
        case MQ2:
        {int mq2gases[]={0,1,2,3,4,5,6};
		conc=gasppm(_pin, _type, mq2gases[5]);
		 #ifdef DEBUG
        Serial.println(F("MQSENSOR 2 output"));
        Serial.print(F("SMOKE:"));
        Serial.print(conc);
        Serial.print( F(" PPM") );
        Serial.print(F("    "));
        Serial.print(F("\n"));
		#endif
		return conc;}
        break;

        case MQ3:
        {int mq3gases[] = {4,7,2,8,1,3};
		conc=gasppm(_pin, _type, mq3gases[0]);
		 #ifdef DEBUG
        Serial.println(F("MQSENSOR 3 output"));
        Serial.print(F("ALCOHOL:"));
        Serial.print(gasppm(_pin, _type, mq3gases[0]));
        Serial.print(F( " mg/L" ));
        Serial.print(F("    "));
        Serial.print(F("\n"));
		#endif
		return conc;}
        break;

      case MQ4:
        {int mq4gases[] = {1,2,0,3,4,5};
		conc=gasppm(_pin, _type, mq4gases[1]);
		 #ifdef DEBUG
        Serial.println(F("MQSENSOR 4 output"));
        Serial.print(F("METHANE:"));
        Serial.print(gasppm(_pin, _type, mq4gases[1]));
        Serial.print(F( " PPM" ));
        Serial.print(F("    "));
        Serial.print(F("\n"));
		#endif
		return conc;}
        break;

        case MQ5:
        {int mq5gases[] = {0,1,2,3,4};
		conc=gasppm(_pin, _type, mq5gases[1]);
		 #ifdef DEBUG
        Serial.println(F("MQSENSOR 5 output"));
        Serial.print(F("LPG:"));
        Serial.print(conc);
        Serial.print(F( " PPM" ));
        Serial.print(F("    "));
        Serial.print(F("\n"));
		#endif
		return conc;}
        break;

      case MQ6:
        {int mq6gases[] = {1,0,2,3,4};
		conc=gasppm(_pin, _type, mq6gases[0]);
		 #ifdef DEBUG
        Serial.println(F("MQSENSOR 6 output"));
        Serial.print(F("LPG:"));
        Serial.print(gasppm(_pin, _type, mq6gases[0]));
        Serial.print(F( " PPM" ));
        Serial.print(F("\n"));
		#endif
		return conc;}
        break;

      case MQ7:
        {int mq7gases[] = {3,0,1,2,4};
		conc=gasppm(_pin, _type, mq7gases[0]);
		 #ifdef DEBUG
        Serial.println(F("MQSENSOR 7 output"));
        Serial.print(F("CARBON_MONOXIDE:"));
        Serial.print(gasppm(_pin, _type, mq7gases[0]));
        Serial.print(F( " PPM" ));
        Serial.print(F("\n"));
		#endif
		return conc;}
        break;

      case MQ8:
        {int mq8gases[] = {0,4,3,2};
		conc=gasppm(_pin, _type, mq8gases[0]);
		 #ifdef DEBUG
        Serial.println(F("MQSENSOR 8 output"));
        Serial.print(F("HYDROGEN:"));
        Serial.print(gasppm(_pin, _type, mq8gases[0]));
        Serial.print(F( " PPM" ));
        Serial.print(F("\n"));
		#endif
		return conc;}
        break;
		
	  case MQ9:
        {int mq9gases[] = {1,3,2};
		conc=gasppm(_pin, _type, mq9gases[1]);
		 #ifdef DEBUG
        Serial.println(F("MQSENSOR 9 output"));
        Serial.print(F("CARBON_MONOXIDE :"));
        Serial.print(conc);
        Serial.print(F( " PPM" ));
        Serial.print(F("\n"));
		#endif
		return conc;}
        break;


      case MQ135:
        {int mq135gases[] = {9,3,4,10,11,12};
		 #ifdef DEBUG
        Serial.println(F("MQSENSOR 135 output"));
        Serial.print(F("CARBON_DIOXIDE:"));
        Serial.print(gasppm(_pin, _type, mq135gases[0]));
        Serial.print(F( " PPM" ));
        Serial.print(F("    "));
        Serial.print(F("\n"));
		#endif
		return conc;}
        break;
		
	  case MQ137:
        {int mq137gases[] = {10};
		conc=gasppm(_pin, _type, mq137gases[0]);
		 #ifdef DEBUG
        Serial.println(F("MQSENSOR 137 output"));
        Serial.print(F("AMMONIUM:"));
        Serial.print(gasppm(_pin, _type, mq137gases[0]));
        Serial.print(F( " PPM" ));
        Serial.print(F("    "));
        Serial.print(F("\n"));
		#endif
		return conc;}
        break;
		
	case MQ136:
        {int mq136gases[] = {13};
		conc=gasppm(_pin, _type, mq136gases[0]);
		 #ifdef DEBUG
        Serial.println(F("MQSENSOR 136 output"));
        Serial.print(F("H2S:"));
        Serial.print(gasppm(_pin, _type, mq136gases[0]));
        Serial.print(F( " PPM" ));
        Serial.print(F("    "));
        Serial.print(F("\n"));
		#endif
		return conc;}
        break;
    }
  }

