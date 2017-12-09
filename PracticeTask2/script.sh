#!/bin/bash
CC="gcc"
OBJDIR="./Objfiles"
TESTDIR="./Tests"
SW1="./stage1.c"
SOW1="$OBJDIR/s1.o"
PW1="$OBJDIR/s1"
SW2="./stage2.c"
SOW2="$OBJDIR/s2.o"
PW2="$OBJDIR/s2"
OPTIONS_CC="-O2 -Wall -Werror -pedantic-errors -Wno-pointer-sign -Wextra -fsanitize=undefined -std=gnu11 -ftrapv  -g -o"
CHECKMEM="valgrind"
OPTIONS_CHECKMEM="--leak-check=full --track-origins=yes --show-leak-kinds=all"
clean=0
if [ ! -z $1 ]
then         
    if [ $1 = "clean" ]
    then
        rm -rf $OBJDIR
        if [ -d lcov ]
        then
            rm -rf ./lcov
        fi
    fi
    clean=1
fi
if [ $clean = 0 ]
then
    if [ -e $SW ]
    then
        if [ ! -e $OBJDIR ]
        then
            mkdir $OBJDIR
        fi
        if [ $SW1 -nt $PW1 ]
        then 
            $CC $SW1 -c $OPTIONS_CC $SOW1 --coverage
            $CC $SOW1 $OPTIONS_CC $PW1 --coverage
        fi
        if [ $SW2 -nt $PW2 ]
        then 
            $CC $SW2 -c $OPTIONS_CC $SOW2 --coverage
            $CC $SOW2 $OPTIONS_CC $PW2 --coverage
        fi
        dir=0
    else
        echo You are in wrong direction!!!
        dir=1
    fi    
    if [ $dir = 0 ]
    then
        if [ ! -z $1 ]
        then         
            if [ $1 = "y" ]
            then
                echo Lets begin!
                $CHECKMEM $OPTIONS_CHECKMEM $PW2 $PW1 <$TESTDIR/correct 2>$OBJDIR/$CHECKMEM.txt 
                $CHECKMEM $OPTIONS_CHECKMEM $PW2 $PW1 <$TESTDIR/incorrect 2>$OBJDIR/$CHECKMEM.txt 
            if [ ! -z $2 ]
            then
            if [ $2 = "y" ]
                    then
                    if [ -d ./lcov ]
                    then
                            echo lcov was installed
                        else
                        echo installing lcov
                            git clone https://github.com/linux-test-project/lcov.git
                        fi
                        ./lcov/bin/lcov --capture --directory $OBJDIR/ --rc lcov_branch_coverage=1 --output-file $OBJDIR/coverage.info
                        ./lcov/bin/genhtml $OBJDIR/coverage.info --branch-coverage  -o $OBJDIR/
                        if [ ! -z $3 ]
                        then
                            if [ $3 = "y" ]
                            then
                                firefox $OBJDIR/index.html
                            fi
                        fi
                    fi
                fi
            fi
        fi
    fi
fi
