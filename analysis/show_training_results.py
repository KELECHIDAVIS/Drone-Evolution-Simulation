# read from the simulation_data and visualize each stored generation  

# for each file prefixed with gen_ in simulation_data,
# read contents and replay its champ 

 # Example file showing a circle moving on screen
import pygame
import os
from read_data import getConstantData, getGenerationData
import time 
releaseMode = True 
print("Starting Sim Analysis in "+ ("Release" if releaseMode else "Debug"))
dir_path = "../build-release/simulation_data" if releaseMode else "../build/simulation_data"

#read in the constants file 
constants = getConstantData(dir_path + "/constants.json")
#print("Simulation Constants: ", constants)

# pygame setup
pygame.init()
screen = pygame.display.set_mode((constants['ENV_WIDTH'] *3, constants['ENV_HEIGHT']*2))
font = pygame.font.SysFont(pygame.font.get_default_font(), 28)
clock = pygame.time.Clock()
dt = 0
running = True

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

def getRocketVertices(data, frame):
    vertices = [
        (data['champReplayFrames'][frame]['v1x'],  constants['ENV_HEIGHT'] - data['champReplayFrames'][frame]['v1y']),
        (data['champReplayFrames'][frame]['v2x'], constants['ENV_HEIGHT']- data['champReplayFrames'][frame]['v2y']),
        (data['champReplayFrames'][frame]['v3x'], constants['ENV_HEIGHT'] - data['champReplayFrames'][frame]['v3y']),
    ] 

    return vertices 
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    if current_frame >= len(genData["champReplayFrames"]):
        current_gen+=GEN_STEP 
        genData = getGenerationData(dir_path, current_gen)
        if not genData: break
        current_frame = 0
        
    
    #ON y's subtract from env height to draw correctly 
    rocket_x = genData["champReplayFrames"][current_frame]['rocketX']
    rocket_y = constants['ENV_HEIGHT']- genData["champReplayFrames"][current_frame]['rocketY']
    rocket_rot = genData['champReplayFrames'][current_frame]["rotation"]
    target_x = genData["champReplayFrames"][current_frame]['targetX']
    target_y = constants['ENV_HEIGHT'] - genData["champReplayFrames"][current_frame]['targetY']

    vertices= getRocketVertices(genData, current_frame); 
    
    current_frame+=1 
    
    screen.fill("black")
    #draw
    
    #draw boundaries 
    pygame.draw.rect(screen, (255,255,255) ,(0,0, screen.get_width(), screen.get_height()/2 ), 1 )
    pygame.draw.rect(screen, (255,255,255) ,(0,0, constants['ENV_WIDTH'], screen.get_height() ), 1 )
    pygame.draw.rect(screen, (255,255,255) ,(0,0, 2*constants['ENV_WIDTH'], screen.get_height() ), 1 )
    
    # Stats panel bounds
    stats_x = constants['ENV_WIDTH']
    stats_y = 0
    stats_width = constants['ENV_WIDTH']
    stats_height = constants['ENV_HEIGHT']

    # Build multiline gen stats text
    gen_stats = [
        f"Generation: {genData['generation']}",
        f"Avg Raw Fit: {genData['avgRawFit']}",
        f"Best Adjusted Fit: {genData['bestAdjFit']}",
        f"Best Raw Fit: {genData['bestRawFit']}",
        f"Best Species ID: {genData['bestSpecies']}",
        f"Worst Adjusted Fit: {genData['worstAdjFit']}",
        f"Worst Raw Fit: {genData['worstRawFit']}",
        f"Worst Species ID: {genData['worstSpecies']}",
        f"Gens Since Innovation: {genData['gensSinceInnovation']}"
    ]

    # Render each line
    line_height = font.get_linesize()
    for i, line in enumerate(gen_stats):
        text_surface = font.render(line, True, (255, 255, 255))
        screen.blit(
            text_surface,
            (stats_x + stats_width // 2 - text_surface.get_width() // 2, 10 + i * line_height)
        )

    pygame.draw.circle(screen, (255, 0,0), (target_x ,target_y), constants['TARGET_RADIUS'] )
    pygame.draw.polygon(screen, (0, 255,255), vertices, 0)

    
    pygame.display.update()
    clock.tick(60) #clock.tick(30)
    
    
pygame.quit() 