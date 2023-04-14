 #include <SPI.h>
#include <ICM_20948.h>
  #define USE_SPI      
#define SPI_PORT SPI // Your desired SPI port.       Used only when "USE_SPI" is defined
  #define CS_PIN 6     // Which pin you connect CS to. Used only when "USE_SPI" is defined
  ICM_20948_SPI myICM; // If using SPI create an ICM_20948_SPI object


String printFormattedFloat(float val, uint8_t leading, uint8_t decimals)
{
  String write_line = ""; 
  float aval = abs(val);
  if (val < 0)
  {
    write_line += "-";
    // SERIAL_PORT.print("-");
  }
  else
  {
    write_line += " ";
    // SERIAL_PORT.print(" ");
  }
  for (uint8_t indi = 0; indi < leading; indi++)
  {
    uint32_t tenpow = 0;
    if (indi < (leading - 1))
    {
      tenpow = 1;
    }
    for (uint8_t c = 0; c < (leading - 1 - indi); c++)
    {
      tenpow *= 10;
    }
    if (aval < tenpow)
    {
      write_line += "0";
    // SERIAL_PORT.print("0");
    }
    else
    {
      break;
    }
  }
  if (val < 0)
  {
    write_line += -val;
    write_line += decimals; 
    // SERIAL_PORT.print(-val, decimals);
  }
  else
  {
    write_line += val; 
    write_line += decimals; 
    // SERIAL_PORT.print(val, decimals);
  }
  return write_line;
} //end printformattedfloat()

String printScaledAGMT(ICM_20948_SPI *sensor){
//  // SERIAL_PORT.print("Gyr (DPS) [ ");
  String write_line = "";
//  write_line += printFormattedFloat(sensor->gyrZ(), 5, 2);
//  // SERIAL_PORT.print(" ], Mag (uT) [ ");
//  write_line += ", "; 
//  // SERIAL_PORT.print(", ");
//  write_line += printFormattedFloat(sensor->magX(), 5, 2);
//  // SERIAL_PORT.print(", ");
//  write_line += ", "; 
  write_line += printFormattedFloat(sensor->temp(), 5, 2);
  // SERIAL_PORT.print(" ]");
  write_line += ", "; 
  // SERIAL_PORT.print(", ");
  return write_line; 
  }