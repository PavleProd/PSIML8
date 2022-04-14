from PIL import Image
import numpy as np
import queue


def checkColor(color):
    if color == 255:
        return 0
    elif color == 0:
        return 1
    elif color == 34:
        return 2
    elif color == 128:
        return 3
    elif color == 82:
        return 4
    elif color == 215:
        return 5


# 'empty': 0 255 belo
# 'wall': 1 0 crno
# 'roomba': 2 34 crveno
# 'station': 3 128 zeleno
# 'furniture': 4 82 braon
# 'dirt': 5 215 zuto
def checkWhichColor(x, y):
    global tileWidth, tileHeight
    for i in range(xDelimeters[x][0], xDelimeters[x][1]):
        for j in range(yDelimeters[y][0], yDelimeters[y][1]):
            color = roomTable[i][j]
            res = checkColor(color)
            if res:
                return res

    return 0


# Nalazi delimetre izmedju svakog crnog polja
def findDelimeters():
    global roomWidth, roomHeight, tileWidth, tileHeight, roomTable
    global xDelimeters, yDelimeters

    iterY = 0
    for j in range(roomWidth - 1):
        while roomTable[0][iterY] != 0:
            iterY += 1
        first = iterY
        while roomTable[0][iterY] != 255:
            iterY += 1
        second = iterY
        yDelimeters.append((first, second))

    iterX = 0
    for j in range(roomHeight - 1):
        while roomTable[iterX][0] != 0:
            iterX += 1
        first = iterX
        while roomTable[iterX][0] != 255:
            iterX += 1
        second = iterX
        xDelimeters.append((first, second))
    xDelimeters.append((0, 0))
    yDelimeters.append((0, 0))


# Pravi matricu kao u stavci pod a)
def makeMatrix():
    global roomTable  # matrica sa pikselima
    global xDelimeters, yDelimeters
    global roomWidth, roomHeight
    matrix = []
    for i in range(roomHeight):
        matrix.append([])
        for j in range(roomWidth):
            if i == 0 or i == roomHeight - 1 or j == roomWidth - 1 or j == 0:
                matrix[i].append(1)  # zid
            else:
                matrix[i].append(checkWhichColor(i, j))

    return matrix


def uSliku(np1):
    image2 = Image.fromarray(np1)
    image2.show()


# Cropuje tabelu tako da ostane samo soba bez beline izvan
def cropTable():
    global roomTable
    blackPixels = np.where(roomTable == 0)
    fX = blackPixels[0][0]
    fY = blackPixels[1][0]
    lX = blackPixels[0][-1]
    lY = blackPixels[1][-1]
    roomTable = roomTable[fX:lX + 1, fY:lY + 1]


# Nalazi duzinu stranice jednog polja
def findSideWidth():
    cols = roomTable.shape[1]
    for j in range(cols):
        if roomTable[0][j] == 255:
            return j
    return 0


# Nalazi duzinu stranice jednog polja
def findSideHeight():
    rows = roomTable.shape[0]
    for i in range(rows):
        if roomTable[i][0] == 255:
            return i
    return 0


def findElem(matrix, elem):
    for i in range(len(matrix)):
        for j in range(len(matrix[i])):
            if matrix[i][j] == elem:
                return i, j


# 'empty': 0 255 belo
# 'wall': 1 0 crno
# 'roomba': 2 34 crveno
# 'station': 3 128 zeleno
# 'furniture': 4 82 braon
# 'dirt': 5 215 zuto
# Koristi se BFS za pretragu najkraceg puta
def task2():
    global roomMatrix
    rows = len(roomMatrix)
    cols = len(roomMatrix[0])

    # prethodnik, pisace obrnuto da bi smo posle samo ubacili u listu
    last = [['' for _ in range(cols)] for _ in range(rows)]
    visited = [[False for _ in range(cols)] for _ in range(rows)]  # posecen
    roomba = findElem(roomMatrix, 2)
    station = findElem(roomMatrix, 3)

    q = queue.Queue()
    q.put(roomba)
    while not q.empty():
        x, y = q.get()
        visited[x][y] = True
        if roomMatrix[x][y] == 1 or roomMatrix[x][y] == 4:
            last[x][y] = 'x'
            continue
        if x > 0 and not visited[x-1][y]:
            q.put((x-1, y))
            last[x-1][y] = 'u'
        if x < rows - 1 and not visited[x+1][y]:
            q.put((x+1, y))
            last[x+1][y] = 'd'
        if y > 0 and not visited[x][y-1]:
            q.put((x, y-1))
            last[x][y-1] = 'l'
        if y < cols - 1 and not visited[x][y+1]:
            q.put((x, y+1))
            last[x][y+1] = 'r'

    res = []
    curr = station
    while curr != roomba:
        x, y = curr
        res.append(last[x][y])  # pisalo je obrnuto
        if last[x][y] == 'd':
            curr = (x-1, y)
        elif last[x][y] == 'u':
            curr = (x+1, y)
        elif last[x][y] == 'l':
            curr = (x, y+1)
        else:
            curr = (x, y-1)

    res.reverse()
    return res


if __name__ == "__main__":
    inputImagePath, C, Cmax = input().split(' ')
    C = int(C)
    Cmax = int(Cmax)
    print([C, Cmax])
    print('Task 1')

    roomIMG = Image.open(inputImagePath)
    roomTable = np.array(roomIMG)[:, :, 1]
    cropTable()
    tileWidth = findSideWidth()
    tileHeight = findSideHeight()
    # print(tileLength)

    imgH, imgW = roomTable.shape[:2]
    roomHeight = imgH // tileHeight
    roomWidth = imgW // tileWidth
    xDelimeters = []
    yDelimeters = []
    findDelimeters()

    roomMatrix = makeMatrix()
    print([roomHeight, roomWidth])
    print(*roomMatrix, sep="\n")

    print("Task 2")
    print(task2())
    print("Task 3")
    print([])
    print("Task 4")
    print([])
    print("Task 5")
    print([0])

# Zelena Koordinata
# 34 - CRVENA
# 215 - ZUTA
# 128 - ZELENA
# 82 - BRAON
# 0 - CRNO
# 255 - BELO

# C:\Users\pavle\PycharmProjects\pythonProject1\set\room_7.png 2 2
