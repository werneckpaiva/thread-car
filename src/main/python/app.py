from flask import Flask, render_template, request
from flask_socketio import SocketIO
from flask_socketio import send, emit
import sys
import socket

app = Flask(__name__)
socketio = SocketIO(app)

print("Connecting to car...", end=" ")
socketCar = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
socketCar.connect(("192.168.0.101", 9000))
print("connected")


@app.route('/')
def main():
    return render_template('main.html')

@socketio.on('car_action')
def handle_car_action(obj):
    action_value = obj["action"]
    print("action value: %s" % action_value)
    cmd_value = action_value # + "\n"
    socketCar.send(cmd_value.encode())

if __name__ == '__main__':
    print("Starting server at 0.0.0.0:8000")
    socketio.run(app, host='0.0.0.0', port=8000)
    socketCar.close()
