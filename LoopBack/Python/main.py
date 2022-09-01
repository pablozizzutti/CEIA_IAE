# General import
import serial
import logging

# logging configuration
logging.basicConfig(level=logging.DEBUG)

# Constants
BAUDRATE = 9600
TIMEOUT = 2
PORT = '/dev/ttyACM0'
MSG_TEST = 'Buen dia, soy la placa Nucleo F746ZG'

# String inicialization
completeRX = ""


# Nucleo F746ZG Loopback communication
with serial.Serial(PORT, BAUDRATE, timeout=TIMEOUT) as serComm:
  
  # Loop for the iteration of each character of the test message
  for i, char in enumerate(MSG_TEST):

    logging.info(f"Char to send: {char}: Char number: {i}")
    
    # Write character to Nucleo
    serComm.write(char.encode("ascii"))    
    
    # Response readed
    charReaded = serComm.read() 

    # Char decode and add to complete RX string
    completeRX += charReaded.decode("ascii")


logging.info(f"Complete data received: {completeRX}: Qty chars: {i+1}")