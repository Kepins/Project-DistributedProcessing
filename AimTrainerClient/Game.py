import random
import select


from Message import Message,Type
import pygame
from operator import itemgetter
RADIUS=20
MENU=50
YELLOW=(255,255,0)
WHITE=(255,255,255)
BLACK=(0,0,0)
GREEN=(0,255,0)
READY_WIDTH=120
READY_HEIGHT=40
WIDTH=1024
HEIGHT=720
READY_Y=HEIGHT/2-READY_HEIGHT/2
READY_X=WIDTH/2-READY_WIDTH/2
MESSAGE_BUFFER= 2000

class Game:

    def __init__(self,s):
        pygame.init()
        self.socket=s
        self.results = []
        self.width = WIDTH
        self.height = HEIGHT
        self.screen = pygame.display.set_mode((self.width, self.height))
        self.font = pygame.font.Font("freesansbold.ttf", 15)
        pygame.display.set_caption("Aim Trainer")
        self.turn = 0
        self.number = -1
        self.n_players=0
        self.x=0
        self.y=0
        self.ready=False

    def read_message(self, mes):
        messages = Message.deserialize(mes)
        change_function = False
        for mess in messages:
            type = mess.type
            if type==Type.ok:
                if self.number==-1:
                    self.number=mess.id
                    self.add_player(mess)
                    change_function=True
            if type==Type.add_player:
                self.add_player(mess)
            if type==Type.start:
                change_function=True
            if type==Type.draw:
                self.x=mess.x
                self.y=mess.y
                self.turn=mess.id

                self.display()
            if type==Type.point:
                self.add_point(mess.id)
            if type == Type.end:
                id = mess.id
                self.end(id)
        return change_function

    def add_point(self,n):
        for result in self.results:
            if result[0]==n:
                result[1]+=1
                break

    def inform_start(self):
        pygame.draw.rect(self.screen, WHITE, pygame.Rect(0, 0, self.width, self.height))
        text_surf = self.font.render("Game will start in 5 seconds", True, BLACK)
        self.screen.blit(text_surf, (self.width/2-100, self.height/2-6))
        pygame.display.flip()
        while True:
            timeout = 0.01  # in seconds
            ready_sockets, _, _ = select.select(
                [self.socket], [], [], timeout
            )
            if ready_sockets:
                mes = self.socket.recv(MESSAGE_BUFFER)
                self.game(mes)
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    exit()


    def start(self):
        while(True):
            timeout = 0.01  # in seconds
            ready_sockets, _, _ = select.select(
                [self.socket], [], [], timeout
            )
            if ready_sockets:
                mes = self.socket.recv(MESSAGE_BUFFER)
                if self.read_message(mes)==True:
                    self.menu()
                    return 1
                else:
                    return -1

    def add_player(self,message):
        id=message.id
        self.n_players+=1.
        self.results.append([id,0])

    def send_ready(self):
        message=Message(Type.ready)
        self.socket.send(message.serialize().encode("utf-8"))

    def menu(self):
        if self.ready is not True:
            self.display_ready()
            while self.ready is not True:
                timeout = 0.01  # in seconds
                ready_sockets, _, _ = select.select(
                    [self.socket], [], [], timeout
                )
                if ready_sockets:
                    mes = self.socket.recv(MESSAGE_BUFFER)
                    self.read_message(mes)
                for event in pygame.event.get():
                    if event.type == pygame.QUIT:
                        return False
                    elif event.type == pygame.MOUSEBUTTONUP and event.button == 1:
                        if self.check_click_ready() is True:
                            self.send_ready()
                            self.ready=True
                            break
        while True:
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    exit()
            self.display_wait()
            timeout = 0.01  # in seconds
            ready_sockets, _, _ = select.select(
                [self.socket], [], [], timeout
            )
            if ready_sockets:
                mes = self.socket.recv(MESSAGE_BUFFER)
                if self.read_message(mes) is True:
                    self.inform_start()

    def display_wait(self):
        pygame.draw.rect(self.screen, WHITE, pygame.Rect(0, 0, self.width-20, self.height-6))
        text_surf = self.font.render("Wait for the rest of the players", True, BLACK)
        self.screen.blit(text_surf, (self.width/2-100 , self.height/2-6 ))
        pygame.display.flip()

    def display_ready(self):
        pygame.draw.rect(self.screen, WHITE, pygame.Rect(0, 0, self.width, self.height))
        pygame.draw.rect(self.screen, GREEN, pygame.Rect(READY_X,READY_Y , READY_WIDTH, READY_HEIGHT))
        text_surf = self.font.render("Ready?", True, BLACK)
        self.screen.blit(text_surf, (READY_X+35,READY_Y+10))
        pygame.display.flip()


    def check_click_ready(self):
        x, y = pygame.mouse.get_pos()
        if x>=READY_X and x<=READY_X+READY_WIDTH or y>=READY_Y and y<=READY_HEIGHT+READY_Y:
            return True
        return False

    def get_my_result(self):
        for result in self.results:
            if result[0]==self.number:
                return result

    def display(self):
        pygame.draw.rect(self.screen,WHITE,pygame.Rect(0,0,self.width,self.height))
        pygame.draw.rect(self.screen, BLACK, pygame.Rect(0, 0, self.width, MENU-RADIUS))
        color = [random.randint(0,255),random.randint(0,255),random.randint(0,255)]
        pygame.draw.circle(self.screen, color, (self.x,self.y), RADIUS)
        self.sort_results()
        my_result = self.get_my_result()
        text_surf = self.font.render(f"Your result: {my_result[1]}", True, WHITE)
        self.screen.blit(text_surf, (10, 15))
        counter = 1
        for result in self.results:
            if result[0] != self.number:
                text_surf = self.font.render(f"Player {result[0]}: {result[1]}", True, WHITE)
                self.screen.blit(text_surf, (20+counter*100, 15))
                counter += 1
                if counter == 8: #wyswietlamy tylko 8 najlepszych wynikow
                    break
        pygame.display.flip()

    def sort_results(self):
        self.results=sorted(self.results, key=itemgetter(1), reverse=True)

    def game(self,mes):
        self.read_message(mes)
        while True:
            timeout=0.01
            ready_sockets, _, _ = select.select(
                [self.socket], [], [], timeout
            )
            if ready_sockets:
                mes = self.socket.recv(MESSAGE_BUFFER)
                self.read_message(mes)
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    return False
                elif event.type == pygame.MOUSEBUTTONUP and event.button == 1:
                    self.send_shot()

    def send_shot(self):
        x, y = pygame.mouse.get_pos()
        turn = self.turn
        message = Message(Type.click, x, y, turn)
        self.socket.send(message.serialize().encode("utf-8"))

    def end(self, n):
        self.socket.close()
        win = False
        if n == self.number:
            win = True
        pygame.draw.rect(self.screen, WHITE, pygame.Rect(0, 0, self.width, self.height))
        if win is True:
            text_surf = self.font.render("You won! Click to finish", True, BLACK)
        else:
            text_surf = self.font.render(f"Player {n} won. Click to finish", True, BLACK)
        self.screen.blit(text_surf, (self.width/2-80, self.height/2-6))
        pygame.display.flip()
        while True:
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    exit(0)
                elif event.type == pygame.MOUSEBUTTONUP and event.button == 1:
                    exit(0)


