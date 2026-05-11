#
#   Base game/graphics coded via AI (Claude Haiku 4.5 via Duck.ai) with user edits/adjustments made after
#
#

import pgzrun
import pygame
import random
from pgzero.rect import Rect

import serial
ser = serial.Serial('COM4')
print('Opening port:'+str(ser.name))

# ============================================================================
# CONSTANTS
# ============================================================================

# Game settings
SCREEN_WIDTH = 800
SCREEN_HEIGHT = 600
FPS = 60

# Player settings
PLAYER_WIDTH = 32
PLAYER_HEIGHT = 48
PLAYER_SPEED = 2
PLAYER_START_X = SCREEN_WIDTH // 2
PLAYER_START_Y = SCREEN_HEIGHT - 80

# Enemy settings
ENEMY_WIDTH = 32
ENEMY_HEIGHT = 48
ENEMY_SPEED = 3
ENEMY_SPAWN_RATE = 60  # frames between spawns

# Road settings
ROAD_WIDTH = 200
ROAD_X = (SCREEN_WIDTH - ROAD_WIDTH) // 2
ROAD_SCROLL_SPEED = 3

# Colors (8-bit style)
COLOR_ROAD = (50, 50, 50)
COLOR_GRASS = (34, 139, 34)
COLOR_PLAYER = (255, 200, 0)
COLOR_ENEMY = (255, 0, 0)
COLOR_LINE = (255, 255, 255)
COLOR_TEXT = (255, 255, 255)

# ============================================================================
# ROAD CLASS
# ============================================================================

class Road:
    def __init__(self):
        self.lane_scroll = 0
        self.line_positions = [i * 40 for i in range(SCREEN_HEIGHT // 40 + 2)]
    
    def update(self):
        """Scroll the road forward"""
        self.lane_scroll += ROAD_SCROLL_SPEED
        
        # Reset scroll at screen height to create infinite loop
        if self.lane_scroll >= 40:
            self.lane_scroll = 0
    
    def draw(self, screen):
        """Draw road and lane markings"""
        # Draw grass background
        screen.fill(COLOR_GRASS)
        
        # Draw road
        screen.draw.filled_rect(
            Rect(ROAD_X, 0, ROAD_WIDTH, SCREEN_HEIGHT),
            COLOR_ROAD
        )
        
        # Draw center lane markings
        center_x = SCREEN_WIDTH // 2
        for y in self.line_positions:
            adjusted_y = (y - self.lane_scroll) % SCREEN_HEIGHT
            screen.draw.line(
                (center_x, adjusted_y),
                (center_x, adjusted_y + 20),
                COLOR_LINE
                #width=10
            )

# ============================================================================
# PLAYER CLASS
# ============================================================================

class Player:
    def __init__(self):
        self.x = PLAYER_START_X
        self.y = PLAYER_START_Y
        self.width = PLAYER_WIDTH
        self.height = PLAYER_HEIGHT
        self.speed = 0
        self.score = 0
    
    def handle_input(self, keyboard):
        """Process keyboard input"""
        if keyboard.left and self.x > ROAD_X:
            self.x -= PLAYER_SPEED
        if keyboard.right and self.x + self.width < ROAD_X + ROAD_WIDTH:
            self.x += PLAYER_SPEED
        if keyboard.up and self.speed < 8:
            self.speed += 0.2
        if keyboard.down and self.speed > 0:
            self.speed -= 0.2
    
    # Pico input
    def handle_input_pico(self, picoInput):
        """Process keyboard input"""
        centerValue=37000
        turnSense=3000
        accelSense=15000
        if picoInput.steer > (centerValue+turnSense) and self.x > ROAD_X:
            self.x -= PLAYER_SPEED
            #sounds.car_tirescreech.stop()
            sounds.car_tirescreech.play()
        if picoInput.steer < (centerValue-turnSense) and self.x + self.width < ROAD_X + ROAD_WIDTH:
            self.x += PLAYER_SPEED
            #sounds.car_tirescreech.stop()
            sounds.car_tirescreech.play()
        if picoInput.accel < accelSense and self.speed < 16:
            self.speed += 0.2
            sounds.car_rev.play()
        if picoInput.accel > accelSense and self.speed > 0:
            self.speed -= 0.2
    
    def update(self):
        """Update player state"""
        # Gradual deceleration
        if self.speed > 0:
            self.speed *= 0.98
    
    def get_rect(self):
        """Return collision rectangle"""
        return Rect(self.x, self.y, self.width, self.height)
    
    def draw(self, screen):
        """Draw player vehicle (8-bit style)"""
        rect = self.get_rect()
        
        # Main body
        screen.draw.filled_rect(rect, COLOR_PLAYER)
        
        # Windshield
        screen.draw.filled_rect(
            Rect(self.x + 8, self.y + 4, 16, 12),
            (100, 150, 255)
        )
        
        # Speed indicator (exhaust)
        if self.speed > 2:
            for i in range(int(self.speed)):
                screen.draw.filled_rect(
                    Rect(self.x + 12, self.y + self.height + i * 4, 8, 3),
                    (200, 100, 50)
                )

# ============================================================================
# ENEMY CLASS
# ============================================================================

class Enemy:
    def __init__(self, x=None, y=-50):
        if x is None:
            # Random lane within road
            lane = random.choice([0.2, 0.5, 0.8])
            x = ROAD_X + (ROAD_WIDTH * lane) - PLAYER_WIDTH / 2
        
        self.x = x
        self.y = y
        self.width = PLAYER_WIDTH
        self.height = PLAYER_HEIGHT
        self.speed = ENEMY_SPEED + random.uniform(-0.5, 1.5)
    
    def update(self):
        """Move enemy down the screen"""
        self.y += self.speed
    
    def get_rect(self):
        """Return collision rectangle"""
        return Rect(self.x, self.y, self.width, self.height)
    
    def is_off_screen(self):
        """Check if enemy has left the screen"""
        return self.y > SCREEN_HEIGHT
    
    def draw(self, screen):
        """Draw enemy vehicle (8-bit style)"""
        rect = self.get_rect()
        
        # Main body
        screen.draw.filled_rect(rect, COLOR_ENEMY)
        
        # Windshield
        screen.draw.filled_rect(
            Rect(self.x + 8, self.y + 4, 16, 12),
            (100, 100, 100)
        )

# ============================================================================
# ENEMY MANAGER CLASS
# ============================================================================

class EnemyManager:
    def __init__(self):
        self.enemies = []
        self.spawn_timer = 0
    
    def update(self):
        """Update all enemies and spawn new ones"""
        self.spawn_timer += 1
        
        # Spawn new enemy
        if self.spawn_timer >= ENEMY_SPAWN_RATE:
            self.enemies.append(Enemy())
            self.spawn_timer = 0
        
        # Update and remove off-screen enemies
        for enemy in self.enemies[:]:
            enemy.update()
            if enemy.is_off_screen():
                self.enemies.remove(enemy)
    
    def draw(self, screen):
        """Draw all enemies"""
        for enemy in self.enemies:
            enemy.draw(screen)
    
    def get_all_rects(self):
        """Return collision rectangles for all enemies"""
        return [enemy.get_rect() for enemy in self.enemies]

# ============================================================================
# GAME STATE
# ============================================================================

player = Player()
enemy_manager = EnemyManager()
road = Road()
game_over = False
font_size = 32

# ============================================================================
# GAME FUNCTIONS
# ============================================================================

def update():
    """Update game state"""
    global game_over
    
    if game_over:
        if keyboard.space:
            reset_game()
        return
    
    n_bytes = ser.readline() # read all the letters available
    s = str(n_bytes) # turn them into a str
    result1 = s[s.find('(')+1:s.find(',')] # find everything beween ( and ,
    result2 = s[s.find(',')+1:s.find(')')] # find everything between , and )
    global n1_int
    n1_int = int(result1) # convert str to int
    global n2_int
    n2_int = int(result2)
       
    class picoInput:
        steer=n1_int
        accel=n2_int


    # Update systems
    player.handle_input(keyboard)
    player.handle_input_pico(picoInput)
    player.update()
    enemy_manager.update()
    road.update()
    
    # Increase score based on speed
    player.score += int(player.speed)
    
    # Collision detection
    player_rect = player.get_rect()
    for enemy_rect in enemy_manager.get_all_rects():
        if player_rect.colliderect(enemy_rect):
            game_over = True

def draw():
    """Render game"""
    # Draw road and background
    road.draw(screen)
    
    # Draw game objects
    enemy_manager.draw(screen)
    player.draw(screen)
    
    # Draw HUD
    score_text = f"Score: {player.score}"
    speed_text = f"Speed: {player.speed:.1f}"
    
    screen.draw.text(score_text, (10, 10), fontsize=font_size, color=COLOR_TEXT)
    screen.draw.text(speed_text, (10, 50), fontsize=font_size, color=COLOR_TEXT)
    #screen.draw.text('picoInput.steer='+pico,(10, 90), fontsize=font_size, color=COLOR_TEXT)
    #screen.draw.text('picoInput.accel='+str(accel),(10, 130), fontsize=font_size, color=COLOR_TEXT)
    
    # Draw game over screen
    if game_over:
        screen.draw.filled_rect(
            Rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT),
            (0, 0, 0)
        )
        screen.draw.text(
            "GAME OVER",
            (SCREEN_WIDTH // 2 - 120, SCREEN_HEIGHT // 2 - 60),
            fontsize=64,
            color=COLOR_TEXT
        )
        screen.draw.text(
            f"Final Score: {player.score}",
            (SCREEN_WIDTH // 2 - 150, SCREEN_HEIGHT // 2 + 20),
            fontsize=font_size,
            color=COLOR_TEXT
        )
        screen.draw.text(
            "Press SPACE to Restart",
            (SCREEN_WIDTH // 2 - 200, SCREEN_HEIGHT // 2 + 80),
            fontsize=font_size,
            color=COLOR_TEXT
        )

def reset_game():
    """Reset game state"""
    global game_over
    player.x = PLAYER_START_X
    player.y = PLAYER_START_Y
    player.speed = 0
    player.score = 0
    enemy_manager.enemies.clear()
    enemy_manager.spawn_timer = 0
    game_over = False

# ============================================================================
# PGZERO SETUP & RUN
# ============================================================================

WIDTH = SCREEN_WIDTH
HEIGHT = SCREEN_HEIGHT
FPS = FPS

pgzrun.go()
