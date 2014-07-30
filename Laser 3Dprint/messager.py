from serial import Serial
from threading import Thread
from time import sleep

class Messager():
    def __init__(self, port, baud = 115200):
        '''Initializes a messager instance.
        Arguments:
        port: Name of the serial port connect to Arduino board;
        baud: Baudrate of the serial port, 115200 default.
        '''
        try:
            self.port = Serial(port, baud, timeout = 5)
        except:
            print ("Cannot open the port:" + str(port))
            raise

        # every send action must be followed by _listen action
        self.sendable = False # whether or not the send function can work.
        
        self.listen = Thread(target = self._listen)
        #self.listen.setDaemon(True)
        self.listen.start()

    def _listen(self):
        '''This function read the messages from Arduino board.'''
        print 'wait a few seconds...'
        sleep(1)
        while self.port.isOpen():
            recv = self.port.readline()
            print recv,
            if recv.startswith('ERROR'):
                #print recv,
                self.sendable = True
            elif recv.startswith('READY'):
                #print recv,
                self.sendable = True
            else:
                #print recv,
                self.sendable = True
                

    def _checksum(self, command):
        '''Compute the checksum of command string'''
        return reduce(lambda x, y: x ^ y, map(ord, command))
    
    def _send(self, command):
        '''Send a Command string to Serial port.
        the argument command is a string.'''
        while not self.sendable:
            sleep(0.001)
        command = command + '*' + str(self._checksum(command))
        self.port.write(command + '\n')
        self.sendable = False
        
    def disconnect(self):
        '''Close the Serial port of messager.'''
        if None != self.port:
            self.port.close()

    def home(self):
        '''Home the substrate motion stage'''
        self._send("HOME")

    def move(self, dist):
        '''Move a distence'''
        self._send("MOVE" + str(dist))

    def get_position(self):
        '''Get the current position of motion stage'''
        self._send("GETP")

    def set_temperature(self, temp):
        '''set the target temperature for heater.'''
        self._send("SETT")

    def get_temperature(self):
        '''get the current temperature of heater'''
        self._send("GETT")

    def reset_stat(self):
        '''Reset the status of Arduino firmware.'''
        self._send("RSET")

if __name__ == '__main__':
    messager = Messager('COM4')
    
    messager._send("-1.55")
    messager.home()
    #messager.move(5.53)
    
    while not messager.sendable:
        print "wait.",
        sleep(1)
        pass
    messager.disconnect()
