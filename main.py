import serial as ser
import tkinter as tk
import time
from functions import *
from telemeter_functions import on_start_state2, on_stop_state2

root = tk.Tk()  # create parent window


def object_detector_gui(s):
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
    num_option = '2'
    send_byte(ord(num_option), s)  # move to state2
    # ----------------------------------------set angle to send --------------------------------------------------------
    angle = 115
    factor_distance = 62  # 1/ [(1/2^20) *17000]
    factor_angle = 11
    raw_angle = 629 + factor_angle * angle  # 1/ [(1/2^20) *17000]
    raw_angle_low = raw_angle % 256
    raw_angle_high = raw_angle / 256
    # ----------------------------------------send fixed angle----------------------------------------------------------
    send_byte(int(raw_angle_high), s)
    time.sleep(1)  # Sleep for 1 seconds
    send_byte(int(raw_angle_low), s)
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

    label1 = tk.Label(root, text="choose your option", width=25, font=("Arial", 15))
    label1.grid(row=0, column=1)

    objects_d = tk.Button(root, text="Objects Detector System", width=25, command=lambda: object_detector_gui(s),
                          font=("Arial", 10))
    objects_d.grid(row=1, column=0)

    telemeter = tk.Button(root, text="Telemeter", width=25, command=lambda: on_start_state2(s, tk), font=("Arial", 10))
    telemeter.grid(row=1, column=1)

    stop_telemeter = tk.Button(root, text="stop telemeter", width=25, command=on_stop_state2,
                               font=("Arial", 10))
    stop_telemeter.grid(row=2, column=2)

    light_s = tk.Button(root, text="Light Sources Detector System", width=25, font=("Arial", 10))
    light_s.grid(row=1, column=2)

    script_m = tk.Button(root, text="Script Mode", width=25, font=("Arial", 10))
    script_m.grid(row=1, column=3)

    root.mainloop()


if __name__ == '__main__':
    main()
