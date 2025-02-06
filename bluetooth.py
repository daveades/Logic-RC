import serial
from serial.tools import list_ports

class Bluetooth:
    def __init__(self, port=None, baudrate=9600, timeout=1):
        self.port = port or self.find_hc06_port()
        self.baudrate = baudrate
        print(self.port)
        self.timeout = timeout
        self.ser = None

    def find_hc06_port(self):
        ports = list_ports.comports()
        for port in ports:
            if "HC-06" in port.description:
                return port.device
        return None

    def connect(self):
        try:
            self.ser = serial.Serial(port=self.port,
                                     baudrate=self.baudrate,
                                     timeout=self.timeout
                                     )
            print("connected Successfully")
        except serial.SerialException as e:
            raise ConnectionError(f"Failed to connect to {self.port}: {e}")

    def write(self, data):
        if not self.ser or not self.ser.is_open:
            raise ConnectionError("Bluetooth not connected")
        if isinstance(data, str):
            data = data.encode()
        self.ser.write(data)

    def read(self):
        if not self.ser or not self.ser.is_open:
            raise ConnectionError("Bluetooth not connected")
        return self.ser.readline().decode().strip()
    
    def close(self):
        if self.ser and  self.ser.is_open:
            self.ser.close()
