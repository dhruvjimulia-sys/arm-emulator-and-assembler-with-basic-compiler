PRINT "Rock, paper, scissors"
game:
PRINT "Choose 1 for rock, 2 for paper, 3 for scissors"
INPUT r
LET s = RND(3)
IF r+1-s=2 THEN PRINT "DRAW"
IF r+1-s=1 THEN PRINT "LOSS"
IF r+1-s=4 THEN PRINT "LOSS"
IF r+1-s=0 THEN PRINT "WIN"
IF r+1-s=3 THEN PRINT "WIN"
PRINT "Another game?"
INPUT a$
IF a$="yes" THEN GOTO game
END