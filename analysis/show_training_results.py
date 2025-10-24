# read from the simulation_data and visualize each stored generation  

# for each file prefixed with gen_ in simulation_data,
# read contents and replay its champ 

 # Example file showing a circle moving on screen
import pygame
import os
from read_data import getConstantData, getGenerationData
import time 
# pygame setup
pygame.init()
screen = pygame.display.set_mode((1000, 800))
clock = pygame.time.Clock()
dt = 0
running = True
dir_path = "../build-release/simulation_data"

#read in the constants file 
constants = getConstantData(dir_path + "/constants.json")
print("Simulation Constants: ", constants)


'''
On Every Frame: 
- if gendata not loaded or end of champ replay frames reached:
    -load next generation data from file
    -start frame = 0 
-display current frame of champ replay with relevant generation data
'''
current_frame =0
current_gen =0
GEN_STEP =20
genData = getGenerationData(dir_path, current_gen)

def getRocketVertices(x, y, rot, base, height):
    pass 

while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    if current_frame >= len(genData["champReplayFrames"]):
        current_gen+=GEN_STEP 
        genData = getGenerationData(dir_path, current_gen)
        current_frame = 0
        
    
    #TODO: get current frame's x & y for target and rocket 
    rocket_x = round(genData["champReplayFrames"][current_frame]['rocketX'])
    rocket_y = round(genData["champReplayFrames"][current_frame]['rocketY'])
    rocket_rot = genData['champReplayFrames'][current_frame]["rotation"]
    target_x = round(genData["champReplayFrames"][current_frame]['targetX'])
    target_y = round(genData["champReplayFrames"][current_frame]['targetY'])

    # vertices = getRocketVertices (rocket_x, rocket_y, rocket_rot, constants['ROCKET_BASE'], constants['ROCKET_HEIGHT'])

    current_frame+=1 
    
    screen.fill("black")
    #draw
    pygame.draw.rect(screen, (255,255,255) ,(0,0, constants['ENV_WIDTH'], constants['ENV_HEIGHT'] ), 1 )
    
    pygame.draw.circle(screen, (255, 0,0), (target_x ,target_y), constants['TARGET_RADIUS'] )
    pygame.draw.circle(screen, (0, 255,0), (rocket_x ,rocket_y), constants['ROCKET_BASE'] )

    
    pygame.display.update()
    clock.tick(60)
    
    
pygame.quit() 