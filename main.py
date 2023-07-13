from tkinter import *
import serial as ser
import time

root = Tk()  # create parent window

# use Button and Label widgets to create a simple TV remote

# turn_on = Button(root, text="ON", command=turnOnTV)
# turn_on.pack()

def object_detector_gui(s):
    """
    out_txt = ""
    frame = Tk()
    frame.title("TextBox Input")
    frame.geometry('20x200')

    def printInput():
        global out_txt
        out_txt = inputtxt.get(1.0, "end-1c")
        print(out_txt,"1")
    # TextBox Creation
    inputtxt = Text(frame, height=5,  width=20)
    inputtxt.pack()
    # Button Creation
    printButton = Button(frame, text="Print", command=printInput)
    printButton.pack()


      #  lbl.config(text="Provided Input: " + inp)
    """
    angles_list = []
    distances_list = []
    recivieng_index = 0
    recivieng_data = 0
    send_byte(49, s)  # move to state1
    dis = 450
    raw_dis = dis * 62   # 1/ [(1/2^20) *17000]
    raw_dis_low = raw_dis % 256
    raw_dis_high = raw_dis / 256
    send_byte(int(raw_dis_high), s)
    time.sleep(1)  # Sleep for 3 seconds
    send_byte(int(raw_dis_low), s)
    print(raw_dis_low)
    print(raw_dis_high)
    while (not (recivieng_data == 255 and recivieng_index % 4 == 1)):
        if  s.in_waiting > 0:
            recivieng_data = int.from_bytes(s.read(size=1), "big")  # recived MSB byte of distance
            if(recivieng_index % 4 == 0 ):
                print(recivieng_data, recivieng_index,"msb")
                distances_list.append((1/62) * recivieng_data * 256)
            elif (recivieng_index % 4 == 1):
                print(recivieng_data, recivieng_index, "lsb")
                distances_list[len(distances_list) - 1] += (recivieng_data * (1/62)) # add LSB byte to current received distance
            elif (recivieng_index % 4 == 2):
                print(recivieng_data, recivieng_index, "angle msb")
                angles_list.append(recivieng_data * 256)    # recived MSB byte of angle
            else:
                print(recivieng_data, recivieng_index, "angle lsb")
                angles_list[len(angles_list) - 1] += recivieng_data  # add LSB byte to current received angle
                angles_list[len(angles_list) - 1] = (angles_list[len(angles_list) - 1] -629) / 11
            recivieng_index += 1

    print(distances_list)
    print (angles_list)
    print (len(distances_list), len(angles_list))
    print ([(distances_list[i],angles_list[i]) for i in range(0,len(angles_list))])



def telemeter(s):
    """
    out_txt = ""
    frame = Tk()
    frame.title("TextBox Input")
    frame.geometry('20x200')

    def printInput():
        global out_txt
        out_txt = inputtxt.get(1.0, "end-1c")
        print(out_txt,"1")
    # TextBox Creation
    inputtxt = Text(frame, height=5,  width=20)
    inputtxt.pack()
    # Button Creation
    printButton = Button(frame, text="Print", command=printInput)
    printButton.pack()


      #  lbl.config(text="Provided Input: " + inp)
    """
    angles_list = []
    distances_list = []
    recivieng_index = 0
    recivieng_data = 0
    send_byte(50, s)  # move to state1
    angle = 115
    raw_angle = 629 + 11 * angle;   # 1/ [(1/2^20) *17000]
    raw_angle_low = raw_angle % 256
    raw_angle_high = raw_angle / 256
    send_byte(int(raw_angle_high), s)
    time.sleep(1)  # Sleep for 3 seconds
    send_byte(int(raw_angle_low), s)
    print(raw_angle_low)
    print(raw_angle_high)
    while (not (recivieng_data == 255 and recivieng_index % 4 == 1)):
        if  s.in_waiting > 0:
            recivieng_data = int.from_bytes(s.read(size=1), "big")  # recived MSB byte of distance
            if(recivieng_index % 4 == 0 ):
               # print(recivieng_data, recivieng_index,"msb")
                distances_list.append((1/62) * recivieng_data * 256)
            elif (recivieng_index % 4 == 1):
                #print(recivieng_data, recivieng_index, "lsb")
                distances_list[len(distances_list) - 1] += (recivieng_data * (1/62)) # add LSB byte to current received distance
                print(distances_list[len(distances_list) - 1] )
            elif (recivieng_index % 4 == 2):
                #print(recivieng_data, recivieng_index, "angle msb")
                angles_list.append(recivieng_data * 256)    # recived MSB byte of angle
            else:
                #print(recivieng_data, recivieng_index, "angle lsb")
                angles_list[len(angles_list) - 1] += recivieng_data  # add LSB byte to current received angle
                angles_list[len(angles_list) - 1] = (angles_list[len(angles_list) - 1] -629) / 11
            recivieng_index += 1

    print(distances_list)
    print (angles_list)
    print (len(distances_list), len(angles_list))
def send_byte(byte_data, s):
    bytesChar = bytes([byte_data])
    s.write(bytesChar)
   # s.reset_input_buffer()



def main():
    s = ser.Serial('COM1', baudrate=9600, bytesize=ser.EIGHTBITS,
                   parity=ser.PARITY_NONE, stopbits=ser.STOPBITS_ONE,
                   timeout=1) # timeout of 1 sec where the read and write operations are blocking,
    # after the timeout the program continues
    enableTX = True
    # clear buffers
    s.reset_input_buffer()
    s.reset_output_buffer()


    label1 = Label(root, text="choose your option", width=25, font=("Arial", 15))
    label1.grid(row=0, column=1)

    Objects_d = Button(root, text="Objects Detector System", width=25, command=lambda:object_detector_gui(s), font=("Arial", 10))
    # Objects_d.pack()
    Objects_d.grid(row=1, column=0)

    Telemeter = Button(root, text="Telemeter", width=25,command=lambda:telemeter(s) ,font=("Arial", 10))
    # Telemeter.pack()
    Telemeter.grid(row=1, column=1)

    Light_S = Button(root, text="Light Sources Detector System", width=25, font=("Arial", 10))
    # Light_S.pack()
    Light_S.grid(row=1, column=2)

    Script_M = Button(root, text="Script Mode", width=25, font=("Arial", 10))
    # Script_M.pack()
    Script_M.grid(row=1, column=3)

    root.mainloop()



    while 1:
        """
        while s.in_waiting > 0:  # while the input buffer isn't empty
            enableTX = False
            char = s.read(size=1)  # read 1 char from the input buffer
            str.append(char.decode("ascii"))

            if s.in_waiting == 0:
                enableTX = True  # enable transmission to echo the received data

        while s.out_waiting > 0 or enableTX:
            inChar = input("Enter menu option:")
            bytesChar = bytes(inChar, 'ascii')
            s.write(bytesChar)
            s.reset_input_buffer()
        """




if __name__ == '__main__':
    main()
