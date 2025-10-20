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
    

def getGenerationData(dirPath, genNumber):
    """
    Lazily load a single generation file by generation number.
    Useful for on-demand loading when working with thousands of files.
    """
    filePath = os.path.join(dirPath, f"gen_{genNumber}.json")
    return getConstantData(filePath)

