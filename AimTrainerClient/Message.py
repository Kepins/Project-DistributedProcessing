import enum


class Type(enum.IntEnum):
    ready=0
    click=1
    ok=2
    start=3
    already_running=4
    point=5
    draw=6
    add_player=7
    end=8


class Message:

    def __init__(self,type=0,x=0,y=0,id=0):
        self.type=type
        self.x=x
        self.y=y
        self.id=id

    @staticmethod
    def del_semicolon(message):
        new_message = ''
        for i in range(len(message)):
            if message[i] == 59 or message[i] == 32: #when byte is ';' or ' '
                new_message += ' '
            else:
                new_message += str(message[i] - 48)
        return new_message

    @staticmethod
    def deserialize(message):
        numbers = []
        message = Message.del_semicolon(message)
        for z in message.split():
            numbers.append(int(z))
        messages = []
        counter = 0
        while counter < len(numbers):
            x = 0
            y = 0
            id = 0
            type = numbers[counter]
            if type == Type.ok or type == Type.point or type == Type.add_player or type == Type.end:
                id = numbers[counter + 1]
                counter += 1
            if type == Type.draw:
                x = numbers[counter + 1]
                y = numbers[counter + 2]
                id = numbers[counter + 3]
                counter += 3
            messages.append(Message(type, x, y, id))
            counter += 1
        return messages



    def serialize(self):
        message=''
        if self.type== Type.ready:
            message+=str(0)+';'
        if self.type == Type.click:
            message += str(1) + ' '+str(self.x) + ' '+str(self.y)+' '+str(self.id)+';'
        return message

