# Import the required libraries
from functions import *

state2 = True
distance = 0
received_data = 0
factor_distance = 62  # 1/ [(1/2^20) *17000]
factor_angle = 11


# Define a function to print the text in a loop
def telemeter_function_2(tk, s):
    global distance
    global received_data

    if state2:
        if s.in_waiting > 0:
            received_data = int.from_bytes(s.read(size=1), "big")  # received MSB byte of distance
            if not receiving_index:
                distance = (1 / factor_distance) * received_data * 256
            else:
                distance += (received_data * (1 / factor_distance))  # add LSB byte
                # to current received distance
                print(distance)
        tk.after(10, telemeter_function_2)


# Define a function to start the loop
def on_start_state2(s, tk):
    global state2
    global distance
    global received_data
    distance = 0
    received_data = 0
    state2 = True
    num_option = '2'
    send_byte(ord(num_option), s)  # move to state2
    # ----------------------------------------set angle to send --------------------------------------------------------
    angle = 115
    raw_angle = 629 + factor_angle * angle  # 1/ [(1/2^20) *17000]
    raw_angle_low = raw_angle % 256
    raw_angle_high = raw_angle / 256
    # ----------------------------------------send fixed angle----------------------------------------------------------
    send_byte(int(raw_angle_high), s)
    time.sleep(1)  # Sleep for 1 seconds
    send_byte(int(raw_angle_low), s)
    tk.after(10, telemeter_function_2(s, tk))


# Define a function to stop the loop
def on_stop_state2():
    global state2
    state2 = False
