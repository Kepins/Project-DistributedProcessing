import sys
import socket
from Game import Game


PORT=5000

if __name__ == "__main__":
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    print(sys.argv[1])
    s.connect((sys.argv[1],PORT))
    g = Game(s)
    if g.start() == -1:
        print("Not this time:(")
