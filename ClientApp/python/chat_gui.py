import socket
import threading
import tkinter as tk
from tkinter.scrolledtext import ScrolledText
from tkinter import simpledialog


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

    full_msg = f"{USERNAME}:{msg}"
    client_socket.send(full_msg.encode("utf-8"))
    message_entry.delete(0, tk.END)

client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client_socket.connect((HOST, PORT))

window = tk.Tk()
window.title("Chat Room Client")
USERNAME = simpledialog.askstring("Username", "Enter your name:", parent=window)
if USERNAME is None or USERNAME.strip() == "":
    USERNAME = "Anonymous"
chat_box = ScrolledText(window, width=60, height = 20, state = "normal")
chat_box.pack(padx = 10, pady=10)
chat_box.tag_config("alert", foreground="green", font=("Helvetica", 10, "italic"))

message_entry = tk.Entry(window, width = 50)
message_entry.pack(side=tk.LEFT, padx = 10)
message_entry.bind("<Return>", send_message)

send_button = tk.Button(window, text="Send", command=send_message)
send_button.pack(side=tk.LEFT)

thread = threading.Thread(target=receive_messages, daemon=True)
thread.start()

window.mainloop()

