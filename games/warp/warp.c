char rcsid[] = "@(#)Header: warp.c,v 7.0.1.3 86/12/12 17:07:44 lwall Exp";

/*	warp -- a real-time space war program
 *	author: Larry Wall
 *	helpers: Jonathan and Mark Biggar, and Dan Faigin
 *	special thanks to my sweetie Gloria who suggested the Planet Crusher
 *
 * Copyright (c) 2020 The NetBSD Foundation, Inc.
 * All rights reserved.
 * 
 * This code is derived from software contributed to The NetBSD Foundation
 * by Larry Wall.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 * 
 * version 5.0  04/20/83
 *         5.1  05/05/83	various tidbits
 *	   5.2  05/12/83	VAX -> vax, ifdef'ed a SIGCONT
 *	   5.3  05/24/83	RCS
 *
 * Log:	warp.c,v
 * Revision 7.0.1.3  86/12/12  17:07:44  lwall
 * Baseline for net release.
 * 
 * Revision 7.0.1.2  86/10/20  12:08:00  lwall
 * Made all exits reset tty.
 * 
 * Revision 7.0.1.1  86/10/16  10:54:13  lwall
 * Added Damage.  Fixed random bugs.
 * 
 * Revision 7.0  86/10/08  15:14:47  lwall
 * Split into separate files.  Added amoebas and pirates.
 * 
 * Revision 6.4  83/12/16  13:11:45  lwall
 * Handled 15 bit random number generators.
 * 
 * Fixed array overflow bug on multiple zaps.
 * 
 * Multiple zaps now consolidated to minimize output.
 * 
 * Tholian jackpot games outlawed under difficulty 15.
 * 
 * Revision 6.3  83/08/24  11:17:49  lwall
 * Fixed array overflow bug on multiple zap.
 * 
 * Revision 6.2  83/08/23  18:06:37  lwall
 * Added zap command.
 * Warp -s should now work on dumb terminals
 * Specifying difficulty >= 40 now just makes it a special game.
 * SIGTTOU #ifdef'ed.
 * No-delay read provided as alternative to FIONREAD.
 * Warp won't report "-1 obsolete" when there are no Enterprises left.
 * Some high-difficulty tuning.
 * 
 * Revision 6.1  83/08/17  08:49:03  lwall
 * Fixed obscure bug in storing UP that caused a %. in CM to occasionally
 * foist garbage onto the screen.
 * 
 * Revision 6.0  83/08/08  17:09:26  lwall
 * New baseline version for net release.
 * 
 * Revision 5.5  83/08/01  10:59:56  lwall
 * Cloaking for the Enterprise.
 * Difficulty now goes to 99, and many activities depending on difficulty
 *     have been adjusted in frequency.
 * Simplified exit sequence, and reduced dependencies on control
 *     characters.  You needn't see the scoreboard if you don't want to.
 * Hitting i,w,c, or v switches to Enterprise.  Hitting p switches to Base.
 * Excessive use of q is not allowed.
 * Excessive use of D is not allowed.
 * Scoreboard may depend on either full name or login name.
 * Integrated scoreboard lister.  Login name now shows up on scoreboard.
 * "Hidden" startup options are now upper case.
 * Checks upon startup for no cursor movement, or screen too small.
 * Checks upon startup that WARPDIR is correctly protected, and that warp
 *     is running setuid.  As an additional bonus this prevents root from
 *     running warp, which mucks things up, UN*X be blessed.
 * All gets's turned into fgets's for safety.
 * Bonus Enterprises and Bases.
 * Escalating bonuses for saving Base and Enterprise.
 * Escalating Enterprise energy.
 * Turbolasers decrease with distance.
 * Really smart enemies can see through stars occasionally.
 * Occasional Tholian jackpot waves.  Tholians are a trifle nastier.
 * Choleric Gorns.
 * An O or o can miss seeing you.  Enemies can avoid a stationary O, o, or X.
 * Warp 3 enemies and other nastinesses are possible in massacre mode.
 * Enemies that decide to navigate when they see you can do other things than
 *     just come toward you.
 * Gorns occasionally launch a salvo for the fun of it.
 * Only star and enemy explosions can keep the round going now.
 * Bounces don't always go back to starting spot now.
 * Better full name processing.  USG quirks handled.  & substitution also
 *     handled now (whoever dreamed up that one must have been in the middle
 *     of the night before the morning after).
 * Catch ^D on fgets.
 * Version number printer.
 * Less signal catching during debugging.
 * 
 * Revision 5.4  83/06/24  09:28:38  lwall
 * 16 bit random number generators are now supported.
 * Made warp not blow up on a null save file.
 * Warp now prints E and B before the stars.
 * Fixed bug which caused torp count to get decremented even when no torp
 *     was launched because of an obstacle.
 * Put %<n>ld formats where appropriate.
 * Fixed E: 0  0 bug on refresh.
 * 
 * Revision 5.3  83/05/24  14:03:10  lwall
 * Starting RCS
 * 
 */

#include "INTERN.h"
#include "warp.h"
#include "EXTERN.h"
#include "bang.h"
#include "init.h"
#include "intrp.h"
#include "object.h"
#include "move.h"
#include "play.h"
#include "score.h"
#include "sig.h"
#include "term.h"
#include "them.h"
#include "us.h"
#include "util.h"
#include "version.h"
#include "weapon.h"

int
main(int argc, char *argv[])
{
    char tmp, *s, *tcbuf;

    FILE *savfil;

#ifdef lint  /* to suppress "defined but never used" */
# ifdef SIGTSTP
    (void)stop_catcher();
# endif
# ifdef SIGCONT
    (void)cont_catcher();
# endif
#endif

    while (--argc > 0 && (*++argv)[0] == '-')
	for (s = argv[0]+1; *s != '\0'; s++)
	    switch (*s) {
	    case '&':
		amoebaspec = true;
		beginner = true;
		break;
	    case 'A':
		apolspec = true;
		beginner = true;
		break;
	    case 'b':
		beginner = true;
		break;
	    case 'C':
		crushspec = true;
		beginner = true;
		break;
	    case 'D':
		debugging = true;
#ifdef DEBUGGING
		debug = atoi(++s);
#endif
		s += strlen(s)-1;
		break;
	    case 'd':
		s++;
		if (*s == '=') s++;
		ismarts = atoi(s);
		if (ismarts <= 0)
		    ismarts = 1;
		if (ismarts > 99)
		    ismarts = 99;
		if (ismarts > 40)
		    beginner = true;
		s += strlen(s)-1;
		break;
	    case 'E':
		klingspec = true;
		beginner = true;
		s++;
		if (*s == '=') s++;
		inumenemies = atoi(s);
		s += strlen(s)-1;
		break;
	    case 'F':
		friendspec = true;
		beginner = true;
		s++;
		if (*s == '=') s++;
		inumfriends = atoi(s);
		s += strlen(s)-1;
		break;
	    case 'G':
		gornspec = true;
		beginner = true;
		break;
	    case 'l':
		lowspeed = true;
		break;
	    case 'm':
		metakey = true;
		break;
	    case 'M':
		massacre = true;
		break;
	    case 'P':
		piratespec = true;
		beginner = true;
		s++;
		if (*s == '=') s++;
		inumpirates = atoi(s);
		s += strlen(s)-1;
		break;
	    case 'S':
		prespec = true;
		beginner = true;
		s++;
		if (*s == '=') s++;
		if (*s)
		    prescene = atoi(s);
		else
		    prescene = -1;
		s += strlen(s)-1;
		break;
	    case 'R':
		romspec = true;
		beginner = true;
		break;
	    case '*':
		starspec = true;
		beginner = true;
		s++;
		if (*s == '=') s++;
		inumstars = atoi(s);
		s += strlen(s)-1;
		break;
	    case 's':
		scorespec = true;
		break;
	    case 'T':
		tholspec = true;
		beginner = true;
		break;
	    case 'x':
		experimenting = true;
		break;
	    case 'v':
		version();
		exit(0);
		break;
	    default:
		fprintf(stderr,"warp: illegal option %c\n", *s);
		fprintf(stderr, "Usage: warp -dn -b -x -v -s\n");
		exit(1);
	    }
    if (argc != 0) {
	fprintf(stderr, "Usage: warp -dn -b -x -v -s\n");
	exit(1);
    }
    bang_init();
    move_init();
    object_init();
    play_init();
    them_init();
    us_init();
    util_init();
    weapon_init();

    tcbuf = malloc(1024);
    intrp_init(tcbuf);

    if (chdir(warplib) < 0)
	fprintf(stderr,NOCD,warplib);

    term_init();

    term_set(tcbuf);
    free(tcbuf);

    umask(022);     /* mustn't rely on incoming umask--could be 033 which */
		    /* would disable people from running wscore */

    score_init();

    sig_init();

    if (totalscore) {
	clear();
	mvaddstr(12,25,"*** restoring saved game ***");
	roundsleep(1);
    }

    srandom(getpid());

    do {
	for (keepgoing = true;;) {
	    if (!experimenting) {
		if ((savfil = fopen(savefilename,"w")) == NULL) {
		    resetty();
		    printf("Can't open savefile `%s'\r\n", savefilename);
		    finalize(1);
		}
		fprintf(savfil,
		    "%-8s %10ld, %2d,%5d,%2d,%2d,%3d %c%c%c%c%c%c%c%c%c\n",
		    logname, totalscore, smarts, cumsmarts,
		    numents, numbases, wave,
		    apolspec ? 'a' : ' ',
		    beginner   ? 'b' : ' ',
		    crushspec  ? 'c' : ' ',
		    gornspec   ? 'g' : ' ',
		    massacre   ? 'm' : ' ',
		    romspec    ? 'r' : ' ',
		    tholspec   ? 't' : ' ',
		    lowspeed   ? 'l' : ' ',
		    amoebaspec ? '&' : ' '
		);
		fprintf(savfil,"         running on %s, process #%d\n",
		    term+5,getpid());
		fclose(savfil);
	    }

	    lastscore = totalscore;
	    initialize();
	    play();
	    cumsmarts += smarts;
	    wavescore();
	    if (numents<=0 && numbases<=0)
		keepgoing = false;
	  if (!keepgoing) break;
	    do {
		if (experimenting) {
		    mvaddstr(23,15,
		      "      [Hit space to continue, 'q' to quit]       ");
		}
		else {
		    mvaddstr(23,15,
		      "[Hit space to continue, 's' to save, 'q' to quit]");
		}
		sleep(1);
		fflush(stdout);
		eat_typeahead();
		getcmd(&tmp);
		if (tmp == BREAKCH || tmp == INTRCH) {
		    mvaddstr(23,15,
		      "                                                 ");
		    mvaddstr(23,33,
		      "Really quit? ");
		    getcmd(&tmp);
		    if (tmp == 'y' || tmp == 'Y')
			tmp = 'q';
		    else
			tmp = 1;
		}
	    } while (tmp != INTRCH && tmp != BREAKCH && !strchr(" qQs",tmp));
	  if (tmp != ' ' && tmp != 's') break;
	    if (!beginner && smarts < 20)
		smarts += 4;
	    else if (!beginner && smarts < 35)
		smarts += 2;
	    else if (smarts < 99)
		smarts++;
	  if (tmp == 's') save_game();
	}
	score();

    } while (justonemoretime);

    if (!experimenting)
	unlink(savefilename);

    clear();
    resetty();
    exit(0);
}
