import os 
import json 

def getConstantData(filePath):
    try:
        with open(filePath, 'r') as file:
            data = json.load(file)
        
        return data
    except FileNotFoundError:
        print("Error: "+ filePath + " not found. Please ensure the file exists.")
        return None
    except json.JSONDecodeError:
        print("Error: Could not decode JSON "+ filePath + ". Check file content for valid JSON.")
        return None
    

def getGenerationsData(dirPath):
    entries = os.listdir(dirPath)

    genData = []
    for entry in entries:
        
        filePath = os.path.join(dirPath, entry)

        if filePath.startswith("constant"): continue 
        
        if os.path.isfile(filePath):
            genData.append(getConstantData(filePath))

