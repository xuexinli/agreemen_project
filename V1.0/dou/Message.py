class Message(object):
    def __init__(self, From, To, Time, Rand, Mac = 0):
        self.From = From
        self.To = To
        self.Time = Time
        self.Rand = Rand
        self.Mac = Mac
    
    def __str__(self):
        return '$'.join([self.From, self.To, str(self.Time), str(self.Rand), str(self.Mac)])

    @staticmethod
    def strToMsg(msg):
        info = msg.split('$')
        return Message(info[0], info[1], float(info[2]), float(info[3]), int(info[4]))

