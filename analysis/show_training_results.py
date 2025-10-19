# read from the simulation_data and visualize each stored generation  

# for each file prefixed with gen_ in simulation_data,
# read contents and replay its champ 

 # Example file showing a circle moving on screen
import pygame
from read_data import getConstantData, getGenerationsData

# pygame setup
pygame.init()
screen = pygame.display.set_mode((1600, 900))
clock = pygame.time.Clock()
running = True
dt = 0

#read in the constants file 
constants = getConstantData("../build-release/simulation_data/constants.json")
print(constants)
# read from file to get the list of champion 
generationsData = getGenerationsData("../build-release/simulation_data") # returns a list of each generation data 
print( generationsData)

while running:
        
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    


    pygame.display.flip()

    dt = clock.tick(60) / 1000
pygame.quit()