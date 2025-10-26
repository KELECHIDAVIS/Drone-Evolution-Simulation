# visualizer.py
# Integrated Pygame + Matplotlib visualizer for saved simulation generations.
#
# Requirements: pygame, matplotlib, numpy
# Uses your existing read_data.getConstantData and read_data.getGenerationData

import os
import pygame
import matplotlib
matplotlib.use("Agg")  # render to buffer, no GUI
import matplotlib.pyplot as plt
from matplotlib.backends.backend_agg import FigureCanvasAgg as FigureCanvas
import numpy as np
from read_data import getConstantData, getGenerationData

# ---------- User options ----------
releaseMode = True
GEN_STEP = 20             # how you iterate files (your setup uses 20)
UPDATE_GRAPH_ON_GEN = True  # we update graphs only when generation changes
# ----------------------------------

print("Starting Sim Analysis in " + ("Release" if releaseMode else "Debug"))
dir_path = "../build-release/simulation_data" if releaseMode else "../build/simulation_data"

# read in the constants file
constants = getConstantData(os.path.join(dir_path, "constants.json"))
if not constants:
    raise SystemExit("Could not read constants.json from " + dir_path)

# pygame setup
pygame.init()
WINDOW_WIDTH = constants['ENV_WIDTH'] * 3
WINDOW_HEIGHT = constants['ENV_HEIGHT'] * 2
screen = pygame.display.set_mode((WINDOW_WIDTH, WINDOW_HEIGHT))
pygame.display.set_caption("Simulation Replay + Graphs")
font = pygame.font.SysFont("Space Mono", 20)
clock = pygame.time.Clock()

# layout: left = env (ENV_WIDTH * 0..ENV_WIDTH), center = stats (ENV_WIDTH .. 2*ENV_WIDTH), right = graphs (2*ENV_WIDTH .. 3*ENV_WIDTH)
env_x = 0
env_y = 0
env_w = constants['ENV_WIDTH']
env_h = constants['ENV_HEIGHT']

stats_x = env_w
stats_y = 0
stats_w = env_w
stats_h = env_h

graphs_x = 2 * env_w
graphs_y = 0
graphs_w = env_w
graphs_h = env_h

# bottom area (optional) — you can use for additional info if you want
bottom_x = 0
bottom_y = env_h
bottom_w = WINDOW_WIDTH
bottom_h = WINDOW_HEIGHT - env_h

# Playback state
current_frame = 0
current_gen = 0
genData = getGenerationData(dir_path, current_gen)
if not genData:
    raise SystemExit(f"No generation data found starting at {current_gen} in {dir_path}")

# History for graphs
gen_history = []
avg_fit_history = []
best_fit_history = []

# Initialize history with first loaded generation
def init_history_from_gen(genData):
    gen_history.clear()
    avg_fit_history.clear()
    best_fit_history.clear()
    if genData:
        gen_history.append(genData.get('generation', 0))
        avg_fit_history.append(genData.get('avgRawFit', 0.0))
        best_fit_history.append(genData.get('bestRawFit', 0.0))

init_history_from_gen(genData)

# Matplotlib figure for two stacked subplots
fig, (ax_avg, ax_best) = plt.subplots(2, 1, figsize=(6, 6), tight_layout=True)
canvas = FigureCanvas(fig)

def update_graph_history_with_gen(genData):
    """Store new generation stats if this generation hasn't been added yet."""
    if not genData:
        return

    gen = genData.get("generation", None)
    if gen is None:
        return
    
    # Only append once per generation
    if len(gen_history) == 0 or gen != gen_history[-1]:
        gen_history.append(gen)
        avg_fit_history.append(genData.get("avgRawFit", 0.0))
        best_fit_history.append(genData.get("bestRawFit", 0.0))


def render_graphs_to_surface(width, height):
    """Render matplotlib plots (avg & best fitness) into a Pygame surface."""
    if len(gen_history) == 0:
        return None

    dpi = 100
    fig = plt.figure(figsize=(width / dpi, height / dpi), dpi=dpi)
    canvas = FigureCanvas(fig)

    # Plot correct data
    plt.plot(gen_history, avg_fit_history, label="Avg Raw Fit")
    plt.plot(gen_history, best_fit_history, label="Best Raw Fit")
    plt.xlabel("Generation")
    plt.ylabel("Fitness")
    plt.title("Fitness Over Generations")
    plt.legend()
    plt.tight_layout()

    # Render and convert to RGBA
    canvas.draw()
    raw_data = canvas.buffer_rgba()  # ✅ Correct in modern Matplotlib (ARGB → RGBA format)

    # Convert to Pygame surface (RGBA)
    surface = pygame.image.frombuffer(raw_data, (width, height), "RGBA")
    plt.close(fig)
    return surface


def get_rocket_vertices(data, frame_index):
    """Return vertices in Pygame coordinates (flip y-axis)."""
    # guard
    frames = data.get('champReplayFrames', [])
    if frame_index < 0 or frame_index >= len(frames):
        return []
    f = frames[frame_index]
    return [
        (f['v1x'],  constants['ENV_HEIGHT'] - f['v1y']),
        (f['v2x'],  constants['ENV_HEIGHT'] - f['v2y']),
        (f['v3x'],  constants['ENV_HEIGHT'] - f['v3y']),
    ]

def draw_stats_panel(surface, genData):
    """Draw multiline textual stats in the center panel."""
    lines = [
        f"Generation: {genData.get('generation', 'N/A')}",
        f"Avg Raw Fit: {round(genData.get('avgRawFit', 'N/A'),3)}",
        f"Best Adjusted Fit: {round(genData.get('bestAdjFit', 'N/A'),3)}",
        f"Best Raw Fit: {round(genData.get('bestRawFit', 'N/A'),3)}",
        f"Best Species ID: {round(genData.get('bestSpecies', 'N/A'),3)}",
        f"Worst Adjusted Fit: {round(genData.get('worstAdjFit', 'N/A'),3)}",
        f"Worst Raw Fit: {round(genData.get('worstRawFit', 'N/A'),3)}",
        f"Worst Species ID: {genData.get('worstSpecies', 'N/A')}",
        f"Gens Since Innovation: {genData.get('gensSinceInnovation', 'N/A')}"
    ]
    line_h = font.get_linesize()
    # background for clarity
    stats_rect = pygame.Rect(stats_x, stats_y, stats_w, stats_h)
    pygame.draw.rect(surface, (10, 10, 10), stats_rect)  # subtle background
    for i, line in enumerate(lines):
        surf = font.render(line, True, (255, 255, 255))
        sx = stats_x + stats_w // 2 - surf.get_width() // 2
        sy = stats_y + 10 + i * line_h
        surface.blit(surf, (sx, sy))

def draw_environment(surface, genData, frame_idx):
    """Draw env, target, and champion rocket for the current frame."""
    frames = genData.get('champReplayFrames', [])
    if frame_idx < 0 or frame_idx >= len(frames):
        return
    f = frames[frame_idx]

    # convert y coordinates (your saved y is bottom-origin)
    rocket_x = f.get('rocketX', 0)
    rocket_y = constants['ENV_HEIGHT'] - f.get('rocketY', 0)
    target_x = f.get('targetX', 0)
    target_y = constants['ENV_HEIGHT'] - f.get('targetY', 0)

    # boundaries for env area
    env_rect = pygame.Rect(env_x, env_y, env_w, env_h)
    pygame.draw.rect(surface, (255, 255, 255), env_rect, 1)

    # draw target and rocket polygon
    pygame.draw.circle(surface, (255, 0, 0), (int(target_x), int(target_y)), constants.get('TARGET_RADIUS', 5))
    verts = get_rocket_vertices(genData, frame_idx)
    if verts:
        # convert to ints
        verts_int = [(int(x), int(y)) for (x, y) in verts]
        pygame.draw.polygon(surface, (0, 255, 255), verts_int, 0)

def draw_layout_borders(surface):
    """Draw the other vertical separators / borders for clarity."""
    # horizontal split (top env vs bottom area)
    pygame.draw.rect(surface, (255, 255, 255), (0, 0, WINDOW_WIDTH, WINDOW_HEIGHT // 2), 1)
    # vertical boundaries (env, stats, graphs)
    pygame.draw.rect(surface, (255, 255, 255), (0, 0, env_w, WINDOW_HEIGHT), 1)
    pygame.draw.rect(surface, (255, 255, 255), (env_w, 0, env_w, WINDOW_HEIGHT), 1)
    pygame.draw.rect(surface, (255, 255, 255), (2 * env_w, 0, env_w, WINDOW_HEIGHT), 1)

# Frame control
running = True
paused = False  # easy toggle if you want later
frame_delay = 60  # fps cap

# Mark we've already added the first generation to history
last_graph_gen = genData.get('generation', None)

# Main loop
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        elif event.type == pygame.KEYDOWN:
            # simple controls
            if event.key == pygame.K_ESCAPE:
                running = False
            elif event.key == pygame.K_SPACE:
                paused = not paused

    if not paused:
        # advance frames; if we exhausted frames for this generation, load next gen
        if current_frame >= len(genData.get("champReplayFrames", [])):
            # advance to the next generation file using your GEN_STEP
            current_gen += GEN_STEP
            next_data = getGenerationData(dir_path, current_gen)
            if not next_data:
                print(f"No more generation files after gen {current_gen - GEN_STEP}. Exiting.")
                running = False
                continue
            genData = next_data
            current_frame = 0

            # update graph history only when generation changes
            if UPDATE_GRAPH_ON_GEN:
                update_graph_history_with_gen(genData)
                last_graph_gen = genData.get('generation', last_graph_gen)
        else:
            current_frame += 1

    # drawing
    screen.fill((0, 0, 0))
    draw_layout_borders(screen)
    draw_environment(screen, genData, current_frame if current_frame < len(genData.get('champReplayFrames', [])) else -1)
    draw_stats_panel(screen, genData)

    # render graphs (only update figure when gen history changed - we keep this cheap)
    graph_surface = render_graphs_to_surface(graphs_w, graphs_h)
    screen.blit(graph_surface, (graphs_x, graphs_y))

    pygame.display.flip()
    clock.tick(frame_delay)

pygame.quit()
