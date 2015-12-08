void parsePID(byte pid, byte data[], String *value, String *unit, String *desc){
  switch (pid) {
    case 0x04: //Calculated engine load value
      *value=String(data[0]*100/255);
      *unit="%";
      *desc="Calculated engine load value";
      break;
    case 0x05: //Engine coolant temperature
      *value=String(data[0]-40);
      *unit="°C";
      *desc="Engine coolant temperature";
      break;
    case 0x06 || 0x07 || 0x08 || 0x09: //Short term fuel % trim
      *value=String((data[0]-128)*100/128);
      *unit="%";
    case 0x06:
      *desc="Long term fuel % trim - Bank 1";
      break;
    case 0x07:
      *desc="Long term fuel % trim - Bank 1";
      break;
    case 0x08:
      *desc="Long term fuel % trim - Bank 2";
      break;
    case 0x09:
      *desc="Long term fuel % trim - Bank 2";
      break;
    case 0x0A:
      *value=String(data[0]*3);
      *unit="kPa";
      *desc="Fuel pressure"
      break;
    case 0x0B:
      *value=String(data[0]);
      *unit="kPa";
      *desc="Intake manifold absolute pressure";
      break;
  }
}
