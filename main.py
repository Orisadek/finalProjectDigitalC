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
    num_option = '1'
    send_byte(ord(num_option), s)  # move to state1
    # ----------------------------------set distance to send -----------------------------------------------------------
    factor_distance = 62  # 1/ [(1/2^20) *17000]
    factor_angle = 11
    dis = 450
    raw_dis = dis * factor_distance  # 1/ [(1/2^20) *17000]
    raw_dis_low = raw_dis % 256
    raw_dis_high = raw_dis / 256
    # ----------------------------------send mask distance--------------------------------------------------------------
    send_byte(int(raw_dis_high), s)
    time.sleep(1)  # Sleep for 3 seconds
    send_byte(int(raw_dis_low), s)
    # ------------------------------------------------------------------------------------------------------------------
    # print(raw_dis_low)
    # print(raw_dis_high)
    # ----------------------------------get the values of the scan------------------------------------------------------
    angles_list = []
    distances_list = []
    receiving_index = 0
    receiving_data = 0
    while not (receiving_data == 255 and receiving_index % 4 == 1):
        if s.in_waiting > 0:
            receiving_data = int.from_bytes(s.read(size=1), "big")  # received MSB byte of distance
            if receiving_index % 4 == 0:
                print(receiving_data, receiving_index, "msb")
                distances_list.append((1 / factor_distance) * receiving_data * 256)
            elif receiving_index % 4 == 1:
                print(receiving_data, receiving_index, "lsb")
                distances_list[len(distances_list) - 1] += (
                        receiving_data * (1 / factor_distance))  # add LSB byte to current received distance
            elif receiving_index % 4 == 2:
                print(receiving_data, receiving_index, "angle msb")
                angles_list.append(receiving_data * 256)  # received MSB byte of angle
            else:
                print(receiving_data, receiving_index, "angle lsb")
                angles_list[len(angles_list) - 1] += receiving_data  # add LSB byte to current received angle
                angles_list[len(angles_list) - 1] = (angles_list[len(angles_list) - 1] - 629) / factor_angle
            receiving_index += 1
    # ------------------------------------------------------------------------------------------------------------------
    print(distances_list)
    print(angles_list)
    # print (len(distances_list), len(angles_list))
    print([(distances_list[i], angles_list[i]) for i in range(0, len(angles_list))])


def telemeter_function(s):
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
    num_option = '2'
    send_byte(ord(num_option), s)  # move to state1
    # ----------------------------------------set angle to send --------------------------------------------------------
    angle = 0
    factor_distance = 62  # 1/ [(1/2^20) *17000]
    factor_angle = 11
    raw_angle = 629 + factor_angle * angle  # 1/ [(1/2^20) *17000]
    raw_angle_low = raw_angle % 256
    raw_angle_high = raw_angle / 256
    # ----------------------------------------send fixed angle----------------------------------------------------------
    send_byte(int(raw_angle_high), s)
    time.sleep(1)  # Sleep for 1 seconds
    send_byte(int(raw_angle_low), s)
    # print(raw_angle_low)
    # print(raw_angle_high)
    # ----------------------------------------get real time distace values----------------------------------------------
    distances_list = []
    receiving_index = 0
    receiving_data = 0
    while not (receiving_data == 255 and receiving_index % 2 == 1):
        if s.in_waiting > 0:
            receiving_data = int.from_bytes(s.read(size=1), "big")  # received MSB byte of distance
            if receiving_index % 2 == 0:
                distances_list.append((1 / factor_distance) * receiving_data * 256)
            elif receiving_index % 2 == 1:
                distances_list[len(distances_list) - 1] += (receiving_data * (1 / factor_distance))  # add LSB byte
                # to current received distance
                print(distances_list[len(distances_list) - 1])
            receiving_index += 1

    print(distances_list)
    print(len(distances_list))


def light_sources_detector(s):
    num_option = '3'
    send_byte(ord(num_option), s)  # move to state1
    ldr1_list = []
    ldr2_list = []
    i = 0
    j = 0
    while j < 1:
        if s.in_waiting > 0:
            r = int.from_bytes(s.read(size=1), "big")  # received MSB byte of distance
            j += 1
    while i < 40:
        if s.in_waiting > 0:
            receiving_data = int.from_bytes(s.read(size=1), "big")  # received MSB byte of distance
            if i % 4 == 0:
                print(receiving_data, i, "ldr1 msb")
                ldr1_list.append(receiving_data )
            elif i % 4 == 1:
                print(receiving_data, i, "ldr1 lsb")
                ldr1_list[len(ldr1_list) - 1] += receiving_data* 256  # add LSB byte to current received distance
            elif i % 4 == 2:
                print(receiving_data, i, "ldr2 angle msb")
                ldr2_list.append(receiving_data)  # received MSB byte of angle
            else:
                print(receiving_data, i, "ldr2 angle lsb")
                ldr2_list[len(ldr2_list) - 1] += receiving_data* 256  # add LSB byte to current received angle
            i += 1
    print(ldr1_list)
    print(ldr2_list)

    print([(ldr1_list[i], ldr2_list[i]) for i in range(0, len(ldr2_list))])
    extended_array_1 = [((ldr1_list[i] + (ldr1_list[i + 1] - ldr1_list[i]) * 0.2 * k) for k in range(0, 5)) for i in
                        range(9)]
    extended_array_2 = [((ldr2_list[i] + (ldr2_list[i + 1] - ldr2_list[i]) * 0.2 * k) for k in range(0, 5)) for i in
                        range(9)]
    # avg_extended = [((extended_array_1[i] + extended_array_2[i]) * 0.5) for i in range (0, len(extended_array_1))]
    # print(avg_extended)


def send_byte(byte_data, s):
    bytes_char = bytes([byte_data])
    s.write(bytes_char)


# s.reset_input_buffer()


def main():
    s = ser.Serial('COM1', baudrate=9600, bytesize=ser.EIGHTBITS,
                   parity=ser.PARITY_NONE, stopbits=ser.STOPBITS_ONE,
                   timeout=1)  # timeout of 1 sec where the read and write operations are blocking,
    # after the timeout the program continues
    # enableTX = True
    # clear buffers
    s.reset_input_buffer()
    s.reset_output_buffer()

    label1 = Label(root, text="choose your option", width=25, font=("Arial", 15))
    label1.grid(row=0, column=1)

    objects_d = Button(root, text="Objects Detector System", width=25, command=lambda: object_detector_gui(s),
                       font=("Arial", 10))
    # Objects_d.pack()
    objects_d.grid(row=1, column=0)

    telemeter = Button(root, text="Telemeter", width=25, command=lambda: telemeter_function(s), font=("Arial", 10))
    # Telemeter.pack()
    telemeter.grid(row=1, column=1)

    light_s = Button(root, text="Light Sources Detector System", width=25, command=lambda: light_sources_detector(s),
                     font=("Arial", 10))
    # Light_S.pack()
    light_s.grid(row=1, column=2)

    script_m = Button(root, text="Script Mode", width=25, font=("Arial", 10))
    # Script_M.pack()
    script_m.grid(row=1, column=3)

    root.mainloop()
    """
    while 1:
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
