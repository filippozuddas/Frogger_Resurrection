#include <ncursesw/ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <locale.h>
#include <wchar.h>
#include <pthread.h>
#include <semaphore.h>

#include "globali.h"
#include "struct.h"
#include "game.h"
#include "crocodile.h"
#include "thread.h"
#include "colors.h"
#include "frog.h"
#include "map.h"
#include "menu.h"
#include "sprite.h"
#include "score.h"

