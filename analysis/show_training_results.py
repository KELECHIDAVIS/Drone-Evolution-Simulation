# read from the simulation_data and visualize each stored generation  

# for each file prefixed with gen_ in simulation_data,
# read contents and replay its champ 

 # Example file showing a circle moving on screen
import pygame
import os
from read_data import getConstantData, getGenerationData

# pygame setup
pygame.init()
screen = pygame.display.set_mode((1600, 900))
clock = pygame.time.Clock()
dt = 0
running = True
dir_path = "../build-release/simulation_data"

#read in the constants file 
constants = getConstantData(dir_path + "/constants.json")
print(constants)


genData = getGenerationData(dir_path , 0 ) # returns a list of each generation data 
print( genData)


for i in range(0, len(os.listdir(dir_path))-1):
    
    genData = getGenerationData(dir_path , i) 
    for frame in genData['champReplayFrames']:
        if not running: break

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
        screen.fill("blue")
        print("frame: ", frame['frame'])
        pygame.display.flip()
        dt = clock.tick(60) / 1000
    
pygame.quit() 