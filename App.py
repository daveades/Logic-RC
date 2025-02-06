import tkinter as tk
from tkinter import ttk, scrolledtext
import re
import time
from bluetooth import Bluetooth
from serial.tools import list_ports

class RobotInterpreter:
    def __init__(self, bluetooth, status_label):
        self.bluetooth = bluetooth
        self.status_label = status_label
        self.commands = {
            'MOVE FORWARD': ('F', 1.1),  # (command, time multiplier)
            'MOVE BACKWARD': ('B', 1.1),
            'TURN LEFT': ('L', 0.8),
            'TURN RIGHT': ('R', 0.8),
            'STOP': ('S', 0)
        }
        self.variables = {}
        self.current_line = 0
    
    def parse_program(self, code):
        """Parse and execute the program line by line"""
        lines = code.strip().split('\n')
        self.current_line = 0
        
        while self.current_line < len(lines):
            line = lines[self.current_line].strip()
            if not line or line.startswith('#'):  # Skip empty lines and comments
                self.current_line += 1
                continue
                
            if line.startswith('REPEAT'):
                self.handle_loop(lines)
            else:
                self.execute_line(line)
                self.current_line += 1
    
    def handle_loop(self, lines):
        """Handle REPEAT loops"""
        loop_header = lines[self.current_line].strip()
        match = re.match(r'REPEAT (\d+) TIMES', loop_header)
        if not match:
            raise SyntaxError(f"Invalid loop syntax at line {self.current_line + 1}")
        
        iterations = int(match.group(1))
        loop_body = []
        self.current_line += 1
        
        # Collect loop body until END REPEAT
        while self.current_line < len(lines):
            line = lines[self.current_line].strip()
            if line == 'END REPEAT':
                break
            loop_body.append(line)
            self.current_line += 1
            
        if self.current_line >= len(lines):
            raise SyntaxError("Missing END REPEAT statement")
            
        # Execute loop
        for _ in range(iterations):
            for cmd in loop_body:
                if cmd.strip():  # Skip empty lines
                    self.execute_line(cmd)
                    
        self.current_line += 1  # Move past END REPEAT
    
    def execute_line(self, line):
        """Execute a single line of code"""
        parts = line.strip().split()
        if not parts:
            return
            
        if parts[0] == 'WAIT':
            time.sleep(float(parts[1]))
            return
            
        # Handle basic movement commands
        command = ' '.join(parts[:2]) if len(parts) >= 2 else parts[0]
        if command in self.commands:
            cmd_code, time_multiplier = self.commands[command]
            duration = parts[2] if len(parts) > 2 else '1'
            
            # Send command to robot
            bluetooth_command = f"{cmd_code}{duration}"
            self.bluetooth.write(bluetooth_command)
            
            # Calculate and wait for appropriate duration
            if cmd_code != 'S':  # Don't wait for STOP command
                wait_time = float(duration) * time_multiplier
                start_time = time.time()

                while time.time() - start_time < wait_time:
                    if self.bluetooth.ser.in_waiting:
                        response = self.bluetooth.read()
                        print(response)
                        if response == 'O':
                            self.status_label.config(text="Obstacle detected")
                            print("Obstacle detected")
                            return
                        time.sleep(0.1)
            
            return
            
        raise SyntaxError(f"Unknown command: {line}")

class RobotIDE:
    def __init__(self, root):
        self.root = root
        self.root.title("Robot Car Programming IDE")
        self.bluetooth = None
        self.setup_ui()
        self.refresh_ports()  # Add this line
        
    def setup_ui(self):
        # Add port selection dropdown above the buttons
        self.port_frame = ttk.Frame(self.root)
        self.port_frame.pack(padx=10, pady=5)
        
        ttk.Label(self.port_frame, text="Select COM Port:").pack(side=tk.LEFT, padx=5)
        self.port_var = tk.StringVar()
        self.port_dropdown = ttk.Combobox(self.port_frame, textvariable=self.port_var)
        self.port_dropdown.pack(side=tk.LEFT, padx=5)
        
        # Refresh ports button
        ttk.Button(self.port_frame, text="Refresh Ports", command=self.refresh_ports).pack(side=tk.LEFT, padx=5)
        
        # Code Editor
        self.editor = scrolledtext.ScrolledText(self.root, width=50, height=20)
        self.editor.pack(padx=10, pady=5)
        
        # Buttons Frame
        btn_frame = ttk.Frame(self.root)
        btn_frame.pack(padx=10, pady=5)
        
        # Connect Button
        self.connect_btn = ttk.Button(btn_frame, text="Connect Bluetooth", command=self.connect_bluetooth)
        self.connect_btn.pack(side=tk.LEFT, padx=5)
        
        # Run Button
        self.run_btn = ttk.Button(btn_frame, text="Run Program", command=self.run_program)
        self.run_btn.pack(side=tk.LEFT, padx=5)
        
        # Status Label
        self.status_label = ttk.Label(self.root, text="Not Connected")
        self.status_label.pack(pady=5)
        
    def refresh_ports(self):
        ports = [port.device for port in list_ports.comports()]
        self.port_dropdown['values'] = ports
        if ports:
            self.port_dropdown.set(ports[0])
        
    def connect_bluetooth(self):
        selected_port = self.port_var.get()
        if not selected_port:
            self.status_label.config(text="Please select a COM port")
            return
            
        try:
            self.bluetooth = Bluetooth(selected_port)
            self.bluetooth.connect()
            self.status_label.config(text="Connected")
        except Exception as e:
            self.status_label.config(text=f"Connection Error: {str(e)}")

    def run_program(self):
        if not self.bluetooth or not hasattr(self.bluetooth, 'ser') or not self.bluetooth.ser.is_open:
            self.status_label.config(text="Please connect to Bluetooth first")
            return
            
        code = self.editor.get("1.0", tk.END)
        interpreter = RobotInterpreter(self.bluetooth, self.status_label)
        try:
            interpreter.parse_program(code)
            self.status_label.config(text="Program executed successfully")
        except Exception as e:
            self.status_label.config(text=f"Error: {str(e)}")

if __name__ == "__main__":
    root = tk.Tk()
    app = RobotIDE(root)
    root.mainloop()