import time
from copy import deepcopy
from PIL import Image
import numpy as np
import glob


#  vraca putanju do slike broja num
def imgNumberPath(num):
    return digits + str(num) + ".png"


# IDEJA: Nadjemo prvi beli piksel, nadjemo velicinu jednog kvadrata, 9 puta iteriramo i poredimo sa brojevima
def findSquareSize():
    rows, cols = sudokuTable.shape[:2]
    squareLength = [-1, -1, -1]  # pocX,pocY,duzina
    for i in range(rows):
        for j in range(cols):
            if sudokuTable[i][j][0] == 255:
                if squareLength[0] == -1:
                    squareLength[0] = i
                    squareLength[1] = j
            elif squareLength[0] != -1:
                squareLength[2] = j - squareLength[1]
                return squareLength
    return squareLength


# Vraca prvo belo polje na koje naidje u redu
def findFirstWhiteY(beginX, beginY):
    rows, cols = sudokuTable.shape[0:2]
    for j in range(beginY, cols):
        if sudokuTable[beginX][j][0] == 255:
            return j


# Vraca prvo belo polje na koje naidje u novom redu
def findFirstWhiteX(beginX, beginY):
    rows, cols = sudokuTable.shape[0:2]
    for i in range(beginX, rows):
        if sudokuTable[i][beginY][0] == 255:
            return i


# Pravi listu skaliranih numpy matrica cifara od 1 do 9
def makeNumberLists():
    numbers = []
    size = squareSize[2] - 2 * squareSize[3]  # visina - 2*border
    for i in range(1, 10):
        with Image.open(imgNumberPath(i)).convert("LA") as img:
            img.thumbnail((size, size))
            numbers.append(np.array(img))

    return numbers


# Poredi samo crni deo np1 i np2 np1: broj np2: tabela
# -2 nije ista dimenzija, -1 polje je skroz belo, 0 - ne poklapa se, 1 - poklapa se
def compareNumpys(np1, np2):
    skrozBeo = True
    if np1.shape != np2.shape:
        print("NIJE ISTA DIMENZIJA!")
        return -2
    # uSliku(np1)
    # uSliku(np2)
    rows = cols = np1.shape[0]
    for i in range(rows):
        for j in range(cols):
            if np2[i][j][0] == 0:  # crn u tabeli
                skrozBeo = False
                if np1[i][j][1] == 0:  # transparentan u broju
                    return 0

    if skrozBeo:
        return -1
    return 1


def uSliku(np1):
    image2 = Image.fromarray(np1)
    image2.show()


# Pravi sudoku matricu
def createSudokuMatrix():
    matrix = []
    size = squareSize[2]  # velicina kvadrata
    b = squareSize[3]  # velicina bordera
    iterX, iterY = squareSize[0], squareSize[1]
    for i in range(9):
        matrix.append([])
        for j in range(9):
            for k in range(9):
                rez = compareNumpys(digitTables[k],
                                    sudokuTable[iterX + b:iterX + size - b, iterY + b:iterY + size - b, :])
                if rez == 1:
                    matrix[i].append(k + 1)
                    break
                elif rez == -1:
                    matrix[i].append(0)
                    break

            iterY += size  # prebacujemo se na kraj crne
            iterY = findFirstWhiteY(iterX, iterY)
        iterY = squareSize[1]
        iterX += size
        iterX = findFirstWhiteX(iterX, iterY)
    return matrix


# Nalazi prvu crnu u polju (sluzi za sklarianje slike broja)
def findFirstBlack(beginX, beginY, size):
    for i in range(beginX, beginX + size):
        for j in range(beginY, beginY + size):
            if sudokuTable[i][j][0] == 0:
                return i - beginX
    return -1


# Nalazi border od kojeg pocinje broj
# squareSize : pocX,pocY,duzina
def findSquareBorder():
    size = squareSize[2]  # velicina kvadrata
    iterX, iterY = squareSize[0], squareSize[1]
    for i in range(9):
        for j in range(9):
            res = findFirstBlack(iterX, iterY, size)
            if res > -1:
                return res
            iterY += size  # prebacujemo se na kraj crne
            iterY = findFirstWhiteY(iterX, iterY)
        iterY = squareSize[1]
        iterX += size
        iterX = findFirstWhiteX(iterX, iterY)
    return -1


def printSudoku():
    for i in range(9):
        for j in range(9):
            print(sudokuMatrix[i][j], end="\n" if j == 8 else ",")


def findRotation():
    global digitTables
    global sudokuTable
    global squareSize

    squareSize.append(0)
    for _ in range(4):
        size = squareSize[2]  # velicina kvadrata
        b = findSquareBorder()  # border
        squareSize[3] = b
        digitTables = makeNumberLists()  # numpy brojeva

        jmp = False
        temp = 0  # ako dodje do 3 znaci da je losa rotacija jer nismo uboli broj, a 9 i 6 su mogli da dodju
        iterX, iterY = squareSize[0], squareSize[1]
        for i in range(9):
            for j in range(9):
                for k in range(9):
                    if k == 5 or k == 8:  # 6 ce biti 9 kad se za 180 stepeni okrene i obrnuto
                        continue

                    rez = compareNumpys(digitTables[k],
                                        sudokuTable[iterX + b:iterX + size - b, iterY + b:iterY + size - b, :])
                    if rez == 1:
                        return
                    elif rez == -1:
                        break
                else:
                    temp += 1
                    if temp == 3:
                        jmp = True
                        break
                iterY += size  # prebacujemo se na kraj crne
                iterY = findFirstWhiteY(iterX, iterY)
            if jmp:
                break
            iterY = squareSize[1]
            iterX += size
            iterX = findFirstWhiteX(iterX, iterY)

        sudokuTable = np.rot90(sudokuTable)

    return


# Smanjujemo matricu da ostane samo tabela, pocetak se nalazi u
def cropMatrix():
    global squareSize
    global sudokuTable
    rows, cols = sudokuTable.shape[:2]
    whitePixels = np.where(sudokuTable == [255, 0])
    firstX = whitePixels[0][0]
    firstY = whitePixels[1][0]
    lastX = whitePixels[0][-1]
    lastY = whitePixels[1][-1]
    sudokuTable = sudokuTable[firstX:lastX+1, firstY:lastY+1, :]
    squareSize[0] = 0
    squareSize[1] = 0

    rows, cols = sudokuTable.shape[:2]
    squareLength = [-1, -1, -1]  # pocX,pocY,duzina
    for i in range(rows):
        for j in range(cols):
            if sudokuTable[i][j][0] == 0:
                squareSize[2] = j - squareSize[1]
                return
    return


# Resavanje sudokua -----------------------------------

def read(matrix):
    for i in range(9):
        for j in range(9):
            if matrix[i][j] == 0:
                matrix[i][j] = set(range(1,10))

    return matrix


def sudokuSolve(matrix):
    solvable = tryToAdd(matrix)

    if done(matrix):
        return matrix

    for i in range(9):
        for j in range(9):
            cell = matrix[i][j]
            if isinstance(cell, set):
                for value in cell:
                    new_state = deepcopy(matrix)
                    new_state[i][j] = value
                    solved = sudokuSolve(new_state)
                    if solved is not None:
                        return solved
                return None


def tryToAdd(matrix):
    while True:
        solvable, new_unit = checkIfPossible(matrix)
        if not solvable:
            return False
        if not new_unit:
            return True


def done(matrix):
    for row in matrix:
        for cell in row:
            if isinstance(cell, set):
                return False
    return True


def checkIfPossible(state):
    new_units = False

    # proveravamo u redu
    for i in range(9):
        row = state[i]
        values = set(
            [x for x in row if not isinstance(x, set)])  # pravimo set od svih vrednosti koje vec postoje u redu
        for j in range(9):
            if isinstance(state[i][j], set):
                state[i][j] -= values
                if len(state[i][j]) == 1:
                    val = state[i][j].pop()
                    state[i][j] = val
                    values.add(val)
                    new_units = True
                elif len(state[i][j]) == 0:
                    return False, None

    # proveravamo u koloni
    for j in range(9):
        column = [state[x][j] for x in range(9)]
        values = set([x for x in column if not isinstance(x, set)])
        for i in range(9):
            if isinstance(state[i][j], set):
                state[i][j] -= values
                if len(state[i][j]) == 1:
                    val = state[i][j].pop()
                    state[i][j] = val
                    values.add(val)
                    new_units = True
                elif len(state[i][j]) == 0:
                    return False, None

    # proveravamo u susednim
    for x in range(3):
        for y in range(3):
            values = set()
            for i in range(3 * x, 3 * x + 3):
                for j in range(3 * y, 3 * y + 3):
                    if not isinstance(state[i][j], set):
                        values.add(state[i][j])
            for i in range(3 * x, 3 * x + 3):
                for j in range(3 * y, 3 * y + 3):
                    if isinstance(state[i][j], set):
                        state[i][j] -= values
                        if len(state[i][j]) == 1:
                            val = state[i][j].pop()
                            state[i][j] = val
                            values.add(val)
                            new_units = True
                        elif len(state[i][j]) == 0:
                            return False, None

    return True, new_units


if __name__ == "__main__":
    # start = time.process_time()

    rootDir = input()
    tableImage = glob.glob(rootDir + "/*.png")[0]
    digits = (glob.glob(rootDir + "/*/"))[0]  # cifre 1-9.png

    image = Image.open(tableImage)
    # image.thumbnail((64, 64)) # resize

    sudokuTable = np.array(image.convert("LA"))  # slika samo 2 komponente

    squareSize = [0, 0, 0] # pocX,pocY,duzina, border se dodaje u findRotation()
    cropMatrix()
    # squareSize.append(findSquareBorder())  # border od kojeg pocinje broj
    # print(squareSize)
    # digitTables = makeNumberLists()
    digitTables = []  # vrednost se dobija u findRotation()
    findRotation()
    #  image2 = Image.fromarray(sudokuTable)
    #  image2.show()
    sudokuMatrix = createSudokuMatrix()
    printSudoku()
    sudokuMatrix = read(sudokuMatrix)
    sudokuMatrix = sudokuSolve(sudokuMatrix)
    printSudoku()

    # print(time.process_time() - start)
#  C:\Users\pavle\PycharmProjects\pythonProject1\set\01