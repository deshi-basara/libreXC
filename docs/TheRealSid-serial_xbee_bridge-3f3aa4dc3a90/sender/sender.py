import sys
import serial

class SerialSender:
    def buildPackage(self, high, low):
        syncbyte = int("55", 16)
        highbyte = int(high, 16)
        lowbyte = int(low, 16)
        endbyte = int("66", 16)

        data = [
            syncbyte, syncbyte, syncbyte,
            highbyte,
            lowbyte,
            highbyte ^ lowbyte,
            endbyte, endbyte, endbyte
        ]

        package = "".join(chr(x) for x in data)
        return package

    def sendData(self, port, baudrate, package):
        try:
            tty = serial.Serial(port=port, baudrate=baudrate)
            print("\n%d bytes written" % tty.write(package))
            print map(lambda x: '0x%02x' % ord(x), package)
        except Exception, e:
            print "\noops, something went wrong D:"
            print(e)

def main():
    sender = SerialSender()
    port, baudrate, high, low = parseArguments()
    package = sender.buildPackage(high, low)
    sender.sendData(port, baudrate, package)

def parseArguments():
    if len(sys.argv) != 5:
        print('\nusage: python %s port baudrate HIGH-Byte LOW-Byte' % sys.argv[0])
        sys.exit(1)
    return sys.argv[1:]

if __name__ == "__main__":
    main()
