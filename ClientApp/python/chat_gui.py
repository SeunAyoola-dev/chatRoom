import socket
import threading
import tkinter as tk
from tkinter.scrolledtext import ScrolledText


HOST = "127.0.0.1"
PORT = 2000
def receive_messages():
    while True:
        try:
            message = client_socket.recv(1024).decode("utf-8")
            chat_box.insert(tk.END, f"{message}\n")
            chat_box.see(tk.END)
        except:
            break

def send_message(event=None):
    msg = message_entry.get()
    if msg.strip() == "":
        return

    client_socket.send(msg.encode("utf-8"))
    message_entry.delete(0, tk.END)

client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client_socket.connect((HOST, PORT))

window = tk.Tk()
window.title("Chat Room Client")
chat_box = ScrolledText(window, width=60, height = 20, state = "normal")
chat_box.pack(padx = 10, pady=10)

message_entry = tk.Entry(window, width = 50)
message_entry.pack(side=tk.LEFT, padx = 10)
message_entry.bind("<Return>", send_message)

send_button = tk.Button(window, text="Send", command=send_message)
send_button.pack(side=tk.LEFT)

thread = threading.Thread(target=receive_messages, daemon=True)
thread.start()

window.mainloop()

