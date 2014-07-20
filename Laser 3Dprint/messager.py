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
            print _("Cannot open the port:" + str(port))
            raise

        # every send action must be followed by _listen action
        self.sendable = False # whether or not the send function can work.
        
        self.listen = Thread(target = self._listen)
        #self.listen.setDaemon(True)
        self.listen.start()

    def _listen(self):
        '''This function read the messages from Arduino board.'''
        sleep(0.1)
        while self.port.isOpen():
            recv = self.port.readline()
            if recv.startswith('ERROR'):
                print recv,
            elif recv.startswith('DONE'):
                print recv,
                self.sendable = True
            else:
                print recv,
                

    def _checksum(self, command):
        '''Compute the checksum of command string'''
        return reduce(lambda x, y: x ^ y, map(ord, command))
    
    def send(self, command):
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


if __name__ == '__main__':
    messager = Messager('COM4')
    
    # in the future, only the string "HOME" and float data will be sent.
    messager.send("-1.55")
    #messager.send("HOME")

    sleep(0.1)
    messager.disconnect()
