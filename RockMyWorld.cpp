import os
import json


# Ulaz: string [year,month,day] (1960-2022 / 60-22)
def processDate(date):
    year = int(date[0])
    if year <= 22:
        year = 2000 + year
    elif year <= 99:
        year = 1900 + year

    return [year, int(date[1]), int(date[2])]


# Rezultujuci format: {year}/{month}/{day}/{country}/{city}/{venue}.json
def formatData(data):  # {year}/{month}/{day}/{country}/{city}/{venue}.json
    if len(data) == 6:
        newDate = processDate(data[:3])
        for i in range(3):
            data[i] = newDate[i]
    else:
        if '0' <= data[2][0] <= '9':  # {country}/{city}/{year_month_day}/{venue}.json
            date = processDate(data[2].split("_"))
            data = [date[0], date[1], date[2], data[0], data[1], data[3]]
        else:  # {year_month_day}/{country}/{city}/{venue}.json
            date = processDate(data[0].split("_"))
            data = [date[0], date[1], date[2], data[1], data[2], data[3]]

    for i in range(3, 6):
        data[i] = data[i].replace('-', '_')
    data[3] = data[3].replace("the_", "", 1)
    return data


# Vraca sve .json fajlove
def getFilePaths(dir):
    paths = []
    for root, dirs, files in os.walk(dir):
        for file in files:
            if file.endswith('.json'):
                paths.append(os.path.relpath(os.path.join(root, file), dir))
    return paths


# dodaje is_indie = false ako ne postoji, a u attendance dodaje srednju vrednost
# fileData : podaci o svim fajlovima {year}/{month}/{day}/{country}/{city}/{venue}.json
# allConcert : svi json fajlovi (niz nizova recnika)
def fixDicts():
    venues = {}
    totalAtt = 0
    numConcerts = 0
    attendance = "attendance"

    for i in range(len(allConcerts)):
        data = fileData[i]
        concerts = allConcerts[i]

        newConcerts = []
        for cDict in concerts:
            if "band_name" in cDict:
                newConcerts.append(cDict)
        allConcerts[i] = newConcerts
        concerts = allConcerts[i]
        for concert in concerts:
            if "is_indie" not in concert:
                concert["is_indie"] = False
            if attendance in concert:
                totalAtt += concert[attendance]
                numConcerts += 1
                venueName = str(data[3:6])
                if venueName not in venues:
                    venues[venueName] = [0, 0]
                venues[venueName][0] += concert[attendance]
                venues[venueName][1] += 1

    for i in range(len(allConcerts)):
        data = fileData[i]
        concerts = allConcerts[i]
        for concert in concerts:
            if attendance not in concert:
                venueName = str(data[3:6])
                if numConcerts == 0:
                    concert[attendance] = 0
                elif venueName in venues:
                    concert[attendance] = venues[venueName][0] / venues[venueName][1]
                else:
                    concert[attendance] = totalAtt / numConcerts


# fileData : podaci o svim fajlovima {year}/{month}/{day}/{country}/{city}/{venue}.json
# allConcert : svi json fajlovi (niz nizova recnika)
def taskB():
    numCountries = 0
    countries = {}
    for i in range(len(allConcerts)):
        data = fileData[i]
        concerts = allConcerts[i]

        if data[3] in countries:
            continue

        if len(concerts) > 0:
            countries[data[3]] = True
            numCountries += 1

    return numCountries


# fileData : podaci o svim fajlovima {year}/{month}/{day}/{country}/{city}/{venue}.json
# allConcert : svi json fajlovi (niz nizova recnika)
def taskC():
    cityName = ""
    currMax = -1
    citiesConcertCount = {}

    for i in range(len(allConcerts)):
        data = fileData[i]
        concerts = allConcerts[i]

        if data[4] not in citiesConcertCount:
            citiesConcertCount[data[4]] = 0
        citiesConcertCount[data[4]] += len(concerts)

    for city, numConc in citiesConcertCount.items():
        if numConc > currMax:
            cityName = city
            currMax = numConc
        elif numConc == currMax and city < cityName:
            cityName = city

    return cityName


# fileData : podaci o svim fajlovima {year}/{month}/{day}/{country}/{city}/{venue}.json
# allConcert : svi json fajlovi (niz nizova recnika)
def taskD():
    cityIndieAttendance = {}
    for i in range(len(allConcerts)):
        data = fileData[i]
        concerts = allConcerts[i]

        for concert in concerts:
            if concert["is_indie"]:
                if data[4] not in cityIndieAttendance:
                    cityIndieAttendance[data[4]] = 0
                cityIndieAttendance[data[4]] += concert["attendance"]

    sortedDescending = sorted(cityIndieAttendance.items(), key=lambda x: x[1], reverse=True)
    return [x[0] for x in sortedDescending[:3]]  # imena prva 3 grada


# fileData : podaci o svim fajlovima {year}/{month}/{day}/{country}/{city}/{venue}.json
# allConcert : svi json fajlovi (niz nizova recnika)
def taskE():
    indieAtLocationAndDate = {}
    for i in range(len(allConcerts)):
        data = fileData[i]
        concerts = allConcerts[i]

        for concert in concerts:
            if concert["is_indie"]:
                indieAtLocationAndDate[str(data)] = True
                break

    bandAA = {}  # average attendance
    for i in range(len(allConcerts)):
        data = fileData[i]
        concerts = allConcerts[i]

        if str(data) in indieAtLocationAndDate:
            for concert in concerts:
                bandName = concert["band_name"]
                if bandName not in bandAA:
                    bandAA[bandName] = [0, 0]
                bandAA[bandName][0] += concert["attendance"]
                bandAA[bandName][1] += 1

    for lst in bandAA.values():
        if lst[1] > 0:
            lst[0] = lst[0]/lst[1]
    sortedDescending = sorted(bandAA.items(), key=lambda x: x[1][0], reverse=True)
    return [x[0] for x in sortedDescending[:3]]  # imena prva 3 grada


if __name__ == "__main__":
    rootDir = input()
    filePaths = getFilePaths(rootDir)
    taskA = len(filePaths)
    fileData = []

    for filePath in filePaths:
        path = rootDir + "\\" + filePath
        if os.stat(path).st_size == 0:
            filePaths.remove(filePath)

    for i in range(len(filePaths)):
        fileData.append(formatData(filePaths[i].split("\\")))

    # print(*fileData, sep="\n")

    allConcerts = []
    for i in range(len(filePaths)):
        path = rootDir + "\\" + filePaths[i]

        with open(path) as file:
            c = file.read(1)
            file.seek(0)
            if c != '[':  # smesta fajl u recnik
                conc = []
                for line in file:
                    conc.append(json.loads(line))
            else:
                conc = json.loads(file.read())

        allConcerts.append(conc)

    # print(*allConcerts, sep="\n")
    fixDicts()
    # print("------------------------")
    # print(*allConcerts, sep="\n")

    print(taskA)  # A
    print(taskB())
    print(taskC())
    print(*taskD(), sep=",")
    print(*taskE(), sep=",")

# C:\Users\pavle\PycharmProjects\pythonProject1\Jsonovi
# C:\Users\pavle\PycharmProjects\pythonProject1\Jsonovi2
# C:\homework-publish\rockmyworld\public\sets\6
