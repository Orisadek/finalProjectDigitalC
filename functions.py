import time
import tkinter as tk
import serial as ser


def send_byte(byte_data, s):
    bytes_char = bytes([byte_data])
    s.write(bytes_char)


