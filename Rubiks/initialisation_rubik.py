import serial
import picamera
import time

def Visualisation():
    s = serial.Serial('/dev/ttyUSB0',112500)
    print(s)
    i=1
    for i in range(1,7):
        print(i)
        if i == 1:
            s.write("S".encode('ascii'))
        if i == 2:
            s.write("1O,2R".encode('ascii'))
            time.sleep(.300)
            s.write("S".encode('ascii'))
        if i == 3:
            s.write("2R".encode('ascii'))
            time.sleep(.300)
            s.write("S".encode('ascii'))
        if i == 4:
            s.write("2R".encode('ascii'))
            time.sleep(.300)
            s.write("S".encode('ascii'))
        if i == 5:
            s.write("1F,2O,2R,1R".encode('ascii'))
            time.sleep(.300)
            s.write("S".encode('ascii'))
        if i == 6:
            s.write("1R,1R,2F".encode('ascii'))
            time.sleep(.300)
            s.write("S".encode('ascii'))
        

        ans = s.readline().decode('ascii').rstrip()
        #print(ans)
        while ans != "E":
            ans = s.readline().decode('ascii').rstrip()
            #print(ans)

        if ans =="E":
            with picamera.PiCamera() as camera:
                camera.resolution = (60, 100)
                camera.capture("/home/pi/Documents/Photos_pour_le_solver/Face_Cube_"+str(i)+".png") #place pour mettre la photo en prenant i
            ans = "done"+str(i)
            print(ans)
    s.write("1O,1T,1F".encode('ascii'))
    time.sleep(.300)
    s.write("S".encode('ascii'))

if __name__=='__main__':
    print("go")
    Visualisation()
