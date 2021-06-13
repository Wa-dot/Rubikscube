import initialisation_rubik
from tkinter import *
import time
import os
import serial
import picamera

s = serial.Serial('/dev/ttyUSB0',112500)
root = Tk()
imgs_resultats = []
imgs = []
index = 0
buttons_images = []
buttons = []
isOpen = False
    
def init():
    global index
    
    imgs.clear()
    for i in range(7):
        
        imgs.append(PhotoImage(file=str(i)+'.png').subsample(2,2))
        
    for j in range(4):
        
        buttons_images.append(PhotoImage(file='1'+str(j)+'.png').subsample(2,2))

    
    imgs.append(PhotoImage(file='7.gif').subsample(2,2))
    
    buttons.append(Button(root,bg='#FFE631',fg='#FFE631',borderwidth=0,width=75,height=75, image=buttons_images[0],command=begin))
    
    buttons.append(Button(root,bg='#FFE631',fg='#FFE631',borderwidth=0,width=75,height=75, image=buttons_images[1],command=continu))
    
    buttons.append(Button(root,bg='#FFE631',fg='#FFE631',borderwidth=0,width=75,height=75, image=buttons_images[2],command=stop))
    
    buttons.append(Button(root,bg='#FFE631',fg='#FFE631',borderwidth=0,width=75,height=75, image=buttons_images[3],command=stop))
    
    index = 0
    buttons[0] = Button(root,bg='#FFE631',fg='#FFE631',borderwidth=0,width=100 ,height=100, image=buttons_images[0],command=begin)
    buttons[0].place(x=w/2.5,y=h/2.3)
    root.update()

def window():
    global index
    
    canvas.itemconfigure(mainbg, image = imgs[index])
    canvas.image = mainbg
    root.update()
    

def begin():
    global index
    index=7
    buttons[0].destroy()
    window()
    InitCube()

def stop():
    global index
    if index == 5:
        index=6
    if index==2:
        index=4
    for i in range(4):
        buttons[i].destroy()
    window()
    OuverturePinces()
    time.sleep(1)
    init()
    window()
    
def continu():
    global index
    index=5
    
    buttons[3] = Button(root,bg='#FFE631',fg='#FFE631',borderwidth=0,width=75,height=75, image=buttons_images[3],command=stop)
    buttons[3].place(x=w/1.25,y=h/2.65)
    window()
    Resolution()

def analyse():
    global index
    index=2
    time.sleep(2)
    buttons[1] = Button(root,bg='#FFE631',fg='#FFE631',borderwidth=0,width=75,height=75, image=buttons_images[1],command=AffichagePhotos)
    buttons[1].place(x=w/3.1,y=h/2.2)
    buttons[2] = Button(root,bg='#FFE631',fg='#FFE631',borderwidth=0,width=75,height=75, image=buttons_images[2],command=stop)
    buttons[2].place(x=w/2,y=h/2.2)
    window()

def InitProgramme():
    #connection avec ESP + tkinter
    buttons[0] = Button(root,bg='#FFE631',fg='#FFE631',borderwidth=0,width=75,height=75, image=buttons_images[0],command=begin)
    buttons[0].place(x=w+w/2,y=h/3)
    root.update()
    

def InitCube():
    #quand tk inter est ok
    global isOpen
    if isOpen == True:
        FermeturePinces()
    initialisation_rubik.Visualisation()
    analyse()

def FermeturePinces():
    global isOpen
    isOpen =False
    s.write("1F,2F".encode('ascii'))
    time.sleep(.300)
    s.write("S".encode('ascii'))

def OuverturePinces():
    global isOpen
    isOpen =True
    s.write("1O,2O".encode('ascii'))
    time.sleep(.300)
    s.write("S".encode('ascii'))

def Resolution():
    #waitingforit
    print("test")

    s.write("2R,2R,1T,1O,1R,2R,2R,1F,1R,1R".encode('ascii'))
    time.sleep(.300)
    s.write("S".encode('ascii'))

def AffichagePhotos():
    print("affichagephotos")
    global index
    buttons[1].destroy()
    buttons[2].destroy()
    for i in range(1,7):
        imgs.append(PhotoImage(file="/home/pi/Documents/Photos_pour_le_solver/Face_Cube_"+str(i)+'.png').zoom(3))
        index=7+i
        window()
        time.sleep(1)
    
    
    continu()

    



    
if __name__ == '__main__':
    w=480
    h=320
    root.geometry("480x320")
    root.attributes('-fullscreen', True)
    
    #print(buttons)
    
    
    
    canvas = Canvas(root,width = w, height = h, bg='#FFFFFF')
    init()
    mainbg = canvas.create_image(w/2,h/2,image = imgs[index])
    canvas.image = mainbg
    canvas.pack()
    
    root.mainloop()
    


