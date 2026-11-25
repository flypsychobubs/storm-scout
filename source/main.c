#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GRID_W 11
#define GRID_H 9
#define CAMPAIGN_STORMS 5
#define MAX_PROBES 3

static int playerX, playerY;
static int stormX, stormY;
static int fuel, condition, score, stormsCaught, probes;
static int probeDeployed;
static int gameState; /* 0: title, 1: chase, 2: results */
static int tickCount;
static int messageTimer;
static char message[72];

static int clampi(int value, int low, int high) {
    if (value < low) return low;
    if (value > high) return high;
    return value;
}

static int distance_to_storm(void) {
    int dx = abs(playerX - stormX);
    int dy = abs(playerY - stormY);
    return dx > dy ? dx : dy;
}

static void say(const char *text) {
    snprintf(message, sizeof(message), "%s", text);
    messageTimer = 150;
}

static void spawn_storm(void) {
    /* Keep storms away from the initial vehicle position. */
    do {
        stormX = 1 + rand() % (GRID_W - 2);
        stormY = 1 + rand() % (GRID_H - 2);
    } while (abs(stormX - playerX) + abs(stormY - playerY) < 4);
    probeDeployed = 0;
}

static void start_game(void) {
    playerX = 1;
    playerY = GRID_H - 2;
    fuel = 100;
    condition = 100;
    score = 0;
    stormsCaught = 0;
    probes = MAX_PROBES;
    tickCount = 0;
    messageTimer = 0;
    gameState = 1;
    spawn_storm();
    say("Dispatch: first supercell is on radar. Stay mobile!");
}

static void draw_map(void) {
    int x, y;
    printf("       STORM SCOUT RADAR   |   RANGE %dx%d\n", GRID_W, GRID_H);
    printf("       N ^       SENSORS: %d/5\n", stormsCaught);
    printf("      +-----------+\n");
    for (y = 0; y < GRID_H; ++y) {
        printf("      |");
        for (x = 0; x < GRID_W; ++x) {
            char cell = '.';
            if (x == stormX && y == stormY) cell = 'X';
            if (x == playerX && y == playerY) cell = 'V';
            printf("%c", cell);
        }
        printf("|\n");
    }
    printf("      +-----------+\n");
    printf("      V vehicle   X storm core   . open road\n\n");
}

static void render_title(void) {
    consoleSelect(&conTop);
    consoleClear();
    printf("\n\n\n");
    printf("          S T O R M   S C O U T\n\n");
    printf("       FIELD OPERATIONS / 3DS EDITION\n\n\n");
    printf("       Track the supercell. Sample the storm.\n");
    printf("       Bring your vehicle and crew home safely.\n\n\n");
    printf("       Five storms. One chase team.\n\n");
    printf("       Press START to begin\n");
    printf("       Press SELECT to exit\n");

    consoleSelect(&conBottom);
    consoleClear();
    printf("\n  FIELD GUIDE\n\n");
    printf("  D-pad  Drive across the radar grid\n\n");
    printf("  A      Capture data beside the storm\n\n");
    printf("  B      Deploy a probe near the storm\n\n");
    printf("  Storms drift as the chase continues.\n");
    printf("  Stay close for a better sample, but\n");
    printf("  avoid the dangerous storm core.\n");
}

static void render_game(void) {
    consoleSelect(&conTop);
    consoleClear();
    draw_map();
    printf("CHASER %d/5     SCORE %04d\n", stormsCaught + 1, score);
    printf("FUEL      [");
    for (int i = 0; i < 10; ++i) printf("%c", fuel > i * 10 ? '#' : '-');
    printf("] %3d%%\n", fuel);
    printf("VEHICLE   [");
    for (int i = 0; i < 10; ++i) printf("%c", condition > i * 10 ? '#' : '-');
    printf("] %3d%%\n", condition);
    printf("PROBES %d   STORM RANGE %d\n", probes, distance_to_storm());

    consoleSelect(&conBottom);
    consoleClear();
    printf("\n  CHASE RADIO\n\n");
    if (distance_to_storm() == 0)
        printf("  CORE OVERHEAD! Move clear\n  before the vehicle takes more damage.\n\n");
    else if (distance_to_storm() <= 2)
        printf("  Strong rotation nearby.\n  A capture is in range.\n\n");
    else
        printf("  Searching for the cell...\n  Follow the radar and close in.\n\n");
    printf("  %s\n\n", probeDeployed ? "Probe deployed - capture bonus ready." : "Probe status: ready to deploy.");
    if (messageTimer > 0) printf("  DISPATCH: %s\n", message);
    printf("\n  D-pad: drive   A: capture\n  B: deploy probe   SELECT: exit\n");
}

static void render_results(void) {
    consoleSelect(&conTop);
    consoleClear();
    printf("\n\n\n             CHASE REPORT\n\n\n");
    if (stormsCaught == CAMPAIGN_STORMS)
        printf("       OUTSTANDING FIELD WORK!\n\n       All five storms sampled.\n");
    else
        printf("       CHASE ENDED\n\n       The vehicle can go no farther.\n");
    printf("\n       STORMS SAMPLED: %d / %d\n", stormsCaught, CAMPAIGN_STORMS);
    printf("       FINAL SCORE:   %04d\n", score);
    printf("       FUEL LEFT:     %d%%\n", fuel);
    printf("       VEHICLE:       %d%%\n\n", condition);
    printf("       START: chase again   SELECT: quit\n");

    consoleSelect(&conBottom);
    consoleClear();
    printf("\n  Thanks for chasing responsibly.\n\n");
    printf("  Storm Scout is a fictional game.\n  Never enter a real storm or flood zone.\n");
}

static void move_vehicle(int dx, int dy) {
    int oldDistance;
    if (fuel <= 0) {
        say("Fuel exhausted. The chase is over.");
        return;
    }
    oldDistance = distance_to_storm();
    playerX = clampi(playerX + dx, 0, GRID_W - 1);
    playerY = clampi(playerY + dy, 0, GRID_H - 1);
    if (playerX == stormX && playerY == stormY) {
        condition = clampi(condition - 12, 0, 100);
        say("Core impact! Vehicle condition -12. Move out now!");
    } else {
        say(distance_to_storm() < oldDistance ? "Closing in on the storm." : "Driving. Keep an eye on fuel.");
    }
    fuel = clampi(fuel - 1, 0, 100);
}

static void capture_storm(void) {
    int range = distance_to_storm();
    int award;
    if (range > 1) {
        say("Too far for a safe sample. Get within one radar tile.");
        return;
    }
    award = range == 0 ? 150 : 250;
    if (probeDeployed) award += 150;
    score += award;
    stormsCaught++;
    fuel = clampi(fuel + 12, 0, 100);
    condition = clampi(condition + 8, 0, 100);
    probes = clampi(probes + 1, 0, MAX_PROBES);
    probeDeployed = 0;
    if (stormsCaught >= CAMPAIGN_STORMS) {
        gameState = 2;
        return;
    }
    spawn_storm();
    char text[72];
    snprintf(text, sizeof(text), "Sample secured! +%d points. Next cell on radar.", award);
    say(text);
}

static void deploy_probe(void) {
    if (probes <= 0) {
        say("No probes left. Capture a storm to resupply.");
        return;
    }
    if (distance_to_storm() > 1) {
        say("Too far to deploy. Move within one tile of the storm.");
        return;
    }
    if (probeDeployed) {
        say("Probe is already deployed. Press A to capture the storm.");
        return;
    }
    probes--;
    probeDeployed = 1;
    say("Probe deployed. Capture now for a bonus sample!");
}

int main(void) {
    gfxInitDefault();
    consoleInit(GFX_TOP, &conTop);
    consoleInit(GFX_BOTTOM, &conBottom);
    srand((unsigned int)osGetTime());
    gameState = 0;

    while (aptMainLoop()) {
        hidScanInput();
        u32 down = hidKeysDown();
        u32 held = hidKeysHeld();

        if (down & KEY_SELECT) break;

        if (gameState == 0) {
            render_title();
            if (down & KEY_START) start_game();
        } else if (gameState == 1) {
            int dx = 0, dy = 0;
            if (down & KEY_UP) dy = -1;
            else if (down & KEY_DOWN) dy = 1;
            else if (down & KEY_LEFT) dx = -1;
            else if (down & KEY_RIGHT) dx = 1;
            else if ((tickCount % 8) == 0) {
                if (held & KEY_UP) dy = -1;
                else if (held & KEY_DOWN) dy = 1;
                else if (held & KEY_LEFT) dx = -1;
                else if (held & KEY_RIGHT) dx = 1;
            }
            if (dx || dy) move_vehicle(dx, dy);
            if (down & KEY_A) capture_storm();
            if (down & KEY_B) deploy_probe();

            tickCount++;
            if (tickCount % 180 == 0 && gameState == 1) {
                int driftX = (rand() % 3) - 1;
                int driftY = (rand() % 3) - 1;
                stormX = clampi(stormX + driftX, 0, GRID_W - 1);
                stormY = clampi(stormY + driftY, 0, GRID_H - 1);
                say("Radar update: storm cell has shifted.");
            }
            if (messageTimer > 0) messageTimer--;
            if ((fuel <= 0 || condition <= 0) && gameState == 1) gameState = 2;
            render_game();
        } else {
            render_results();
            if (down & KEY_START) start_game();
        }

        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
        svcSleepThread(16 * 1000 * 1000LL);
    }

    gfxExit();
    return 0;
}
