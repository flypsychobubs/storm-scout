#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GRID_W 11
#define GRID_H 9
#define CAMPAIGN_STORMS 5
#define MAX_PROBES 3

static PrintConsole conTop;
static PrintConsole conBottom;
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
    messageTimer = 180;
}

static void print_bar(int value) {
    int i;
    printf("[");
    for (i = 0; i < 10; ++i)
        printf("%c", value > i * 10 ? '#' : '.');
    printf("] %3d%%", value);
}

static void spawn_storm(void) {
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

static void draw_radar(void) {
    int x, y;
    printf("                    N\n");
    printf("          +---------------------+\n");
    for (y = 0; y < GRID_H; ++y) {
        printf("          |");
        for (x = 0; x < GRID_W; ++x) {
            char cell = '.';
            if (x == stormX && y == stormY) cell = 'X';
            if (x == playerX && y == playerY) cell = 'V';
            printf(" %c", cell);
        }
        printf(" |\n");
    }
    printf("          +---------------------+\n");
    printf("                    S\n");
    printf("       V CHASER     X STORM CORE     . CLEAR\n");
}

static void render_title(void) {
    consoleSelect(&conTop);
    consoleClear();
    printf("==================================================\n");
    printf("\n\n");
    printf("              .  *  .  *  .\n");
    printf("          STORM  /\\  SCOUT\n");
    printf("         FIELD OPERATIONS UNIT\n\n");
    printf("       TRACK THE CELL. SAMPLE THE STORM.\n");
    printf("       GET YOUR CREW HOME SAFE.\n\n");
    printf("            [ FIVE-STORM CHASE ]\n\n\n");
    printf("          PRESS START TO DEPLOY\n");
    printf("          PRESS SELECT TO QUIT\n\n");
    printf("==================================================\n");

    consoleSelect(&conBottom);
    consoleClear();
    printf("+-------------------------------------- +\n");
    printf("|          CHASE TEAM FIELD GUIDE       |\n");
    printf("+---------------------------------------+\n\n");
    printf("  D-PAD    Drive the chase vehicle\n\n");
    printf("  A        Capture storm data nearby\n\n");
    printf("  B        Deploy a probe for bonus data\n\n");
    printf("  SELECT   Exit the mission\n\n");
    printf("  STORM SAFETY\n");
    printf("  Avoid the core, conserve fuel, and\n");
    printf("  keep the vehicle in good condition.\n\n");
    printf("  Fictional game: never enter real\n");
    printf("  storms, floodwater, or closed roads.\n");
}

static void render_game(void) {
    int range = distance_to_storm();
    consoleSelect(&conTop);
    consoleClear();
    printf("==================================================\n");
    printf(" STORM SCOUT // LIVE RADAR       CASE %d / %d\n", stormsCaught + 1, CAMPAIGN_STORMS);
    printf(" SCORE %05d                     RANGE %02d\n", score, range);
    printf("--------------------------------------------------\n");
    draw_radar();
    printf("--------------------------------------------------\n");
    printf(" FUEL      ");
    print_bar(fuel);
    printf("\n VEHICLE   ");
    print_bar(condition);
    printf("\n PROBES %d/%d       SIGNAL: %s\n", probes, MAX_PROBES,
           range <= 2 ? "LOCKED" : "SEARCHING");
    printf("==================================================\n");

    consoleSelect(&conBottom);
    consoleClear();
    printf("+---------------------------------------+\n");
    printf("|              CHASE RADIO              |\n");
    printf("+---------------------------------------+\n\n");
    if (range == 0) {
        printf("  WARNING: CORE OVERHEAD\n");
        printf("  Move clear to protect your vehicle.\n\n");
    } else if (range <= 1) {
        printf("  TARGET IN SAMPLE RANGE\n");
        printf("  Press A to capture storm data.\n\n");
    } else if (range <= 3) {
        printf("  ROTATION DETECTED NEARBY\n");
        printf("  Close in carefully for a sample.\n\n");
    } else {
        printf("  SEARCHING FOR THE SUPERCELL\n");
        printf("  Follow the radar signal.\n\n");
    }
    printf("  FIELD STATUS\n");
    printf("  Probe: %s\n", probeDeployed ? "DEPLOYED - capture bonus ready" :
           (probes > 0 ? "ready for deployment" : "no probes remaining"));
    printf("  Samples secured: %d / %d\n\n", stormsCaught, CAMPAIGN_STORMS);
    printf("  DISPATCH\n");
    if (messageTimer > 0) printf("  %.37s\n", message);
    else printf("  Awaiting field report...\n");
    printf("\n  D-PAD DRIVE   A SAMPLE   B PROBE\n");
}

static void render_results(void) {
    consoleSelect(&conTop);
    consoleClear();
    printf("==================================================\n");
    printf("                  CHASE REPORT\n");
    printf("--------------------------------------------------\n\n");
    if (stormsCaught == CAMPAIGN_STORMS) {
        printf("          EXCEPTIONAL FIELD WORK!\n\n");
        printf("       All five storm samples secured.\n");
    } else {
        printf("              CHASE COMPLETE\n\n");
        printf("       The vehicle can go no farther.\n");
    }
    printf("\n       STORMS SAMPLED   %d / %d\n", stormsCaught, CAMPAIGN_STORMS);
    printf("       FINAL SCORE      %05d\n", score);
    printf("       FUEL REMAINING   %d%%\n", fuel);
    printf("       VEHICLE STATUS   %d%%\n\n", condition);
    printf("       START: redeploy   SELECT: quit\n");
    printf("==================================================\n");

    consoleSelect(&conBottom);
    consoleClear();
    printf("+---------------------------------------+\n");
    printf("|            MISSION DEBRIEF            |\n");
    printf("+---------------------------------------+\n\n");
    printf("  Chase team, report safely to base.\n\n");
    printf("  Storm Scout is a fictional game.\n");
    printf("  Never enter real storm or flood zones.\n\n\n");
    printf("  Press START to launch another chase.\n");
    printf("  Press SELECT to exit.\n");
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
        say(distance_to_storm() < oldDistance ? "Closing in on the storm." :
            "Driving. Keep an eye on fuel.");
    }
    fuel = clampi(fuel - 1, 0, 100);
}

static void capture_storm(void) {
    int range = distance_to_storm();
    int award;
    char text[72];
    if (range > 1) {
        say("Too far for a sample. Get within one radar tile.");
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
    snprintf(text, sizeof(text), "Sample secured! +%d points. New cell tracked.", award);
    say(text);
}

static void deploy_probe(void) {
    if (probes <= 0) {
        say("No probes left. Capture a storm to resupply.");
        return;
    }
    if (distance_to_storm() > 1) {
        say("Too far to deploy. Move within one tile.");
        return;
    }
    if (probeDeployed) {
        say("Probe is already deployed. Press A to capture.");
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
        u32 down;
        u32 held;
        hidScanInput();
        down = hidKeysDown();
        held = hidKeysHeld();

        if (down & KEY_SELECT) break;

        if (gameState == 0) {
            render_title();
            if (down & KEY_START) start_game();
        } else if (gameState == 1) {
            int dx = 0;
            int dy = 0;
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
                stormX = clampi(stormX + (rand() % 3) - 1, 0, GRID_W - 1);
                stormY = clampi(stormY + (rand() % 3) - 1, 0, GRID_H - 1);
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
    }

    gfxExit();
    return 0;
}
