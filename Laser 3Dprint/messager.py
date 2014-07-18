from serial import Serial
from threading import Thread
from time import sleep

class messager():
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
        
        self.listen = Thread(target = self._listen)
        #self.listen.setDaemon(True)
        self.listen.start()

    def _listen(self):
        '''This function read the messages from Arduino board.
        '''
        while self.port.isOpen():
            print self.port.readline(),
                    
    def send(self, command):
        self.port.write(command + '\n')
        
    def disconnect(self):
        if None != self.port:
            self.port.close()

if __name__ == '__main__':
    mess = messager('COM4')
    sleep(2) # the delay is necessary for listen thread to startup.
    mess.send("aa")
    sleep(0.5)
    mess.send('aaa')
    sleep(0.5)
    mess.send('aaaa')
    sleep(0.5)
    mess.disconnect()
