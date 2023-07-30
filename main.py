

import tkinter as tk
import serial as ser
import time
#from trance_to_hex import *
#######

import numpy as np
import matplotlib
matplotlib.use('TkAgg')
import matplotlib.pyplot as plt
import keyboard
from matplotlib.widgets import Button
from matplotlib.backends.backend_tkagg import (
    FigureCanvasTkAgg, NavigationToolbar2Tk)


def object_detector_gui(s):
    plt.close("all")
    ##############figure 1####################
    fig = plt.figure(facecolor='k')
    fig.canvas.toolbar.pack_forget()
    fig.canvas.manager.set_window_title("Radar")
    mgn = plt.get_current_fig_manager()
    mgn.window.state('zoomed')
    ax = fig.add_subplot(1, 1, 1, polar=True, facecolor='#006b70')
    ax.tick_params(axis='both', colors='w')
    r_max = 450
    ax.set_ylim([0.0, r_max])
    ax.set_xlim([0.0, np.pi])
    ax.set_position([-0.05, -0.05, 1.1, 1.05])  ## What
    ax.set_rticks(np.linspace(0.0, r_max, 5))
    ax.set_thetagrids(np.linspace(0.0, 180, 20))
    angles = np.arange(0, 181, 1)
    theta = angles * (np.pi / 180)
    pols, = ax.plot([], linestyle='', marker='o', markerfacecolor='r',
                    markeredgecolor='w', markeredgewidth=1.0, markersize=6.0, alpha=0.5)

    line1 = ax.plot([], color='w', linewidth=3.0)
    fig.canvas.draw()
    axbackground = fig.canvas.copy_from_bbox(ax.bbox)
    #########################################
    # --------------------------------------------For Gui---------------------
    s.reset_input_buffer()
    s.reset_output_buffer()
    fig.canvas.flush_events()
    dists = np.ones((len(angles)))
    #------------------------------------------------------------------------
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
                print("distance:", distances_list[len(distances_list) - 1], "Angle", angles_list[len(angles_list) - 1] )
                #----for GUI
                dists[int(angles_list[len(angles_list) - 1])] = distances_list[len(distances_list) - 1]
                line1[0].set_data(np.repeat((angles_list[len(angles_list) - 1] * (np.pi/180)) , 2) , np.linspace(0.0, r_max,2))
                pols.set_data(theta, dists)
                fig.canvas.restore_region(axbackground)
                ax.draw_artist(pols)
                ax.draw_artist(line1[0])
                fig.canvas.blit(ax.bbox)
                #-----
            receiving_index += 1

    # ------------------------------------------------------------------------------------------------------------------
    print(distances_list)
    print(angles_list)
    # print (len(distances_list), len(angles_list))
    print([(distances_list[i], angles_list[i]) for i in range(0, len(angles_list))])






def telemeter_function(s):
    plt.close("all")
    ##############figure 1####################
    fig = plt.figure(facecolor='k')
    fig.canvas.toolbar.pack_forget()
    fig.canvas.manager.set_window_title("Radar")
    mgn = plt.get_current_fig_manager()
    mgn.window.state('zoomed')
    ax = fig.add_subplot(1, 1, 1, polar=True, facecolor='#006b70')
    ax.tick_params(axis='both', colors='w')
    r_max = 450
    ax.set_ylim([0.0, r_max])
    ax.set_xlim([0.0, np.pi])
    ax.set_position([-0.05, -0.05, 1.1, 1.05])  ## What
    ax.set_rticks(np.linspace(0.0, r_max, 5))
    ax.set_thetagrids(np.linspace(0.0, 180, 20))
    angles = np.arange(0, 181, 1)
    theta = angles * (np.pi / 180)
    pols, = ax.plot([], linestyle='', marker='o', markerfacecolor='r',
                    markeredgecolor='w', markeredgewidth=1.0, markersize=6.0, alpha=0.5)

    line1 = ax.plot([], color='w', linewidth=3.0)
    fig.canvas.draw()
    axbackground = fig.canvas.copy_from_bbox(ax.bbox)
    #########################################
    #--------------------------------------------For Gui---------------------
    s.reset_input_buffer()
    s.reset_output_buffer()
    fig.canvas.flush_events()
    dists = np.ones((len(angles)))
    #-----------------------------------
    num_option = '2'
    send_byte(ord(num_option), s)  # move to state1
    # ----------------------------------------set angle to send --------------------------------------------------------
    angle = 90
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

    while True:
        if s.in_waiting > 0:
            receiving_data = int.from_bytes(s.read(size=1), "big")  # received MSB byte of distance
            if receiving_index % 2 == 0:
                distances_list.append((1 / factor_distance) * receiving_data * 256)
            elif receiving_index % 2 == 1:
                distances_list[len(distances_list) - 1] += (receiving_data * (1 / factor_distance))  # add LSB byte
                # to current received distance
                print(distances_list[len(distances_list) - 1])
            receiving_index += 1

            dists[int(angle)] = distances_list[len(distances_list) - 1]
            line1[0].set_data(np.repeat((angle * (np.pi / 180)), 2),
                              np.linspace(0.0, r_max, 2))
            pols.set_data(theta, dists)
            fig.canvas.restore_region(axbackground)
            ax.draw_artist(pols)
            ax.draw_artist(line1[0])
            fig.canvas.blit(ax.bbox)
            fig.canvas.flush_events()

            if keyboard.is_pressed('e'):
                print("Exit Telemeter")

                fig.canvas.flush_events()
                send_byte(ord('0'), s) # go to state0
                break




def get_init_arrays(s):
    ldr1_list = []
    ldr2_list = []
    i = 0
    j = 0
    while j < 1:
        if s.in_waiting > 0:
            r = int.from_bytes(s.read(size=1), "big")  # received MSB byte of distance
            j += 1
    while i < 40:  # config environment reciving
        if s.in_waiting > 0:
            receiving_data = int.from_bytes(s.read(size=1), "big")  # received MSB byte of distance
            if i % 4 == 0:
                print(receiving_data, i, "ldr1 config lsb")
                ldr1_list.append(receiving_data)
            elif i % 4 == 1:
                print(receiving_data, i, "ldr1 config msb")
                ldr1_list[len(ldr1_list) - 1] += receiving_data * 256  # add LSB byte to current received distance
            elif i % 4 == 2:
                print(receiving_data, i, "ldr2  config lsb")
                ldr2_list.append(receiving_data)  # received MSB byte of angle
            else:
                print(receiving_data, i, "ldr2 config msb")
                ldr2_list[len(ldr2_list) - 1] += receiving_data * 256  # add LSB byte to current received angle
            i += 1
    print(ldr1_list)
    print(ldr2_list)
    ldr1_init_arr = []
    ldr2_init_arr = []
    for i in range(9):
        space1 = (ldr1_list[i+1] - ldr1_list[i]) / 5
        space2 = (ldr2_list[i + 1] - ldr2_list[i]) / 5
        for j in range(5):
            ldr1_init_arr.append(ldr1_list[i] + j * space1)
            ldr2_init_arr.append(ldr2_list[i] + j * space2)



    print (len(ldr1_init_arr))
    print (len(ldr2_init_arr))
    print(ldr1_init_arr,"ldr1_init_arr",ldr2_init_arr,"ldr2_init_arr")
    return [(ldr1_init_arr[i] + ldr2_init_arr[i]) * 0.5 for i in range(45)]


def light_sources_detector(s):
    plt.close("all")
    ##############figure 2####################
    fig2 = plt.figure(facecolor='k')
    fig2.canvas.toolbar.pack_forget()
    fig2.canvas.manager.set_window_title("Radar")
    mgn = plt.get_current_fig_manager()
    mgn.window.state('zoomed')
    ax2 = fig2.add_subplot(1, 1, 1, polar=True, facecolor='#006b70')
    ax2.tick_params(axis='both', colors='w')
    r_max2 = 45
    ax2.set_ylim([0.0, r_max2])
    ax2.set_xlim([0.0, np.pi])
    ax2.set_position([-0.05, -0.05, 1.1, 1.05])  ## What
    ax2.set_rticks(np.linspace(0.0, r_max2, 5))
    ax2.set_thetagrids(np.linspace(0.0, 180, 20))
    pols2, = ax2.plot([], linestyle='', marker='o', markerfacecolor='y',
                      markeredgecolor='w', markeredgewidth=1.0, markersize=6.0, alpha=0.5)

    line2 = ax2.plot([], color='w', linewidth=3.0)
    fig2.canvas.draw()

    axbackground = fig2.canvas.copy_from_bbox(ax2.bbox)
    #------------------------------------------------------------------------
    s.reset_input_buffer()
    # --------------------------------------------For Gui---------------------
    fig2.canvas.flush_events()
    angles = np.arange(0, 181, 1)
    theta = angles * (np.pi / 180)
    dists = np.ones((len(angles)))
    # -----------------------------------
    num_option = '3'
    send_byte(ord(num_option), s)  # move to state1
    init_array = get_init_arrays(s)
    ldr1 = []
    ldr2 = []
    samples_list = []
    angles_list = []
    distances = []
    factor_angle = 11
    receiving_index = 0
    receiving_data = 0
    while not (receiving_data == 255 and receiving_index % 6 == 1):
        if s.in_waiting > 0:
            receiving_data = int.from_bytes(s.read(size=1), "big")  # received MSB byte of distance
            if receiving_index % 6 == 0:
                print(receiving_data, receiving_index, "ldr1 msb data")
                ldr1.append( receiving_data * 256)
            elif receiving_index % 6 == 1:
                print(receiving_data, receiving_index, "ldr1 lsb data")
                ldr1[len(ldr1) - 1] += (receiving_data)  # add LSB byte to current received distance
            elif receiving_index % 6 == 2:
                print(receiving_data, receiving_index, "ldr2 msb data")
                ldr2.append(receiving_data * 256)  # received MSB byte of angle

            elif receiving_index % 6 == 3:
                print(receiving_data, receiving_index, "ldr2 lsb data")
                ldr2[len(ldr2) - 1] += receiving_data  # add LSB byte to current received angle
               # print("ldr1:", ldr1[len(ldr1) - 1], "ldr2", ldr2[len(ldr2) - 1])
                samp = (ldr1[len(ldr1) - 1] + ldr2[len(ldr2) - 1]) / 2
                samples_list.append(samp)
                dist_index =np.array([abs((init_array[i] - samp)) for i in range(45)]).argmin()
                print("min: ", min([(init_array[i] - samp) for i in range(45)]))
                print("min index: ", dist_index )
                distance = (dist_index + 1)
                distances.append(distance)

            elif receiving_index % 6 == 4:
                print(receiving_data, receiving_index, "angle msb")
                angles_list.append(receiving_data * 256)  # received MSB byte of angle
            else:
                print(receiving_data, receiving_index, "angle lsb")
                angles_list[len(angles_list) - 1] += receiving_data  # add LSB byte to current received angle
                angles_list[len(angles_list) - 1] = (angles_list[len(angles_list) - 1] - 629) / factor_angle
                print( angles_list[len(angles_list) - 1],  " angles_list[len(angles_list) - 1]")
                # ----for GUI-------
                dists[int(angles_list[len(angles_list) - 1])] = distances[len(distances) - 1]
                line2[0].set_data(np.repeat((angles_list[len(angles_list) - 1] * (np.pi / 180)), 2),
                                  np.linspace(0.0, r_max2, 2))
                pols2.set_data(theta, dists)
                fig2.canvas.restore_region(axbackground)
                ax2.draw_artist(pols2)
                ax2.draw_artist(line2[0])
                fig2.canvas.blit(ax2.bbox)
                #---------

            receiving_index += 1

    print(distances)
    print(angles_list)
    print(init_array, "init_array")
    print([(distances[i], angles_list[i]) for i in range(0, len(angles_list))])


def send_byte(byte_data, s):
    bytes_char = bytes([byte_data])
    s.write(bytes_char)


def translate_files(file_name):
    file = open(file_name, 'r')
    Lines = file.readlines()
    instructions = []
    inst_translate = {
        "inc_lcd": "01",
        "dec_lcd": "02",
        "rra_lcd": "03",
        "set_delay": "04",
        "clear_lcd": "05",
        "servo_deg": "06",
        "servo_scan": "07",
        "sleep": "08"
    }

    for line in Lines:
        x = line.split()
        instruction_arr = [inst_translate[x[0]]]
        if len(x) > 1:
            args = x[1].split(",")
            for arg in args:
                hex_val_int =int(arg)
                if(hex_val_int < 16):
                    hex_val = "0" + hex(hex_val_int)[2:] #[2:] to ignore 0x
                else:
                    hex_val = hex(hex_val_int)[2:]
                instruction_arr.append(hex_val)
        instruction = "".join(instruction_arr)
        instructions.append(instruction)
    return instructions


def send_file(s, file_name):
    file_arr = translate_files(file_name)
    print(file_arr)
    for inst in file_arr:
        for i in range(0, len(inst) -1 ,2):
            command =int(inst[i:i+2], 16)
            print(command)
            send_byte(command, s)
            time.sleep(0.1)
            #print(len(inst))
        for i in range (len(inst) // 2 , 4):   #complete to four bytes
            send_byte(254, s)
            time.sleep(0.1)

    send_byte(255, s)  # EOF msb
    time.sleep(0.1)
    send_byte(255, s)  # EOF lsb
    time.sleep(0.1)

def send_all_files(s):
    num_option = '4'
    send_byte(ord(num_option), s)  # move to state4
    send_file(s,"Script1.txt")
    send_file(s, "Script2.txt")
    send_file(s, "Script3.txt")


            ###############COMUNICATION#####################################
s = ser.Serial('COM1', baudrate=9600, bytesize=ser.EIGHTBITS,
                parity=ser.PARITY_NONE, stopbits=ser.STOPBITS_ONE,
                timeout=1)  # timeout of 1 sec where the read and write operations are blocking,
# after the timeout the program continues
# enableTX = True
# clear buffers
s.reset_input_buffer()
s.reset_output_buffer()
###############################################################

"""
##############figure 2####################
fig2 = plt.figure(facecolor='k')
fig2.canvas.toolbar.pack_forget()
fig2.canvas.manager.set_window_title("Radar")
mgn = plt.get_current_fig_manager()
mgn.window.state('zoomed')
ax2 = fig2.add_subplot(1, 1, 1, polar=True, facecolor='#006b70')
ax2.tick_params(axis='both', colors='w')
r_max2 = 45
ax2.set_ylim([0.0, r_max2])
ax2.set_xlim([0.0, np.pi])
ax2.set_position([-0.05, -0.05, 1.1, 1.05])  ## What
ax2.set_rticks(np.linspace(0.0, r_max2, 5))
ax2.set_thetagrids(np.linspace(0.0, 180, 20))
pols2, = ax2.plot([], linestyle='', marker='o', markerfacecolor='y',
                    markeredgecolor='w', markeredgewidth=1.0, markersize=6.0, alpha=0.5)

line2 = ax2.plot([], color='w', linewidth=3.0)
fig2.canvas.draw()

axbackground = fig.canvas.copy_from_bbox(ax.bbox)
##############TKinter###########################
"""
root = tk.Tk()  # create parent window
label1 = tk.Label(root, text="choose your option", width=25, font=("Arial", 15))
label1.grid(row=0, column=1)
objects_d = tk.Button(root, text="Objects Detector System", width=25, command=lambda: object_detector_gui(s),
                          font=("Arial", 10))
# Objects_d.pack()
objects_d.grid(row=1, column=0)

telemeter = tk.Button(root, text="Telemeter", width=25, command=lambda: telemeter_function(s), font=("Arial", 10))
# Telemeter.pack()
telemeter.grid(row=1, column=1)

light_s = tk.Button(root, text="Light Sources Detector System", width=25, command=lambda: light_sources_detector(s),
                        font=("Arial", 10))
# Light_S.pack()
light_s.grid(row=1, column=2)

script_m = tk.Button(root, text="Send Files", width=25 , command=lambda:send_all_files(s), font=("Arial", 10))
# Script_M.pack()
script_m.grid(row=1, column=3)
plt.show(block = False)
root.mainloop()