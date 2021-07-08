#! /bin/bash


# Occasionally, students use UTF-8 characters in their source code (usually
# without knowing it).  We might as well support it.
LC_ALL="en_US.UTF-8"


# Java figures out the proper student .java files that are required - but we
# need a little help for C and assembly.  If you leave either of these blank,
# then this type of testing simply won't work.
C_SRC=
S_SRC=



if [[ $(which timeout 2>/dev/null) = "" ]]
then
  echo "ERROR: The command 'timeout' is not installed on this system, the grading script will not work." 1>&2
  exit 1
fi



JAVA_SRCS=$(ls -1 *.java 2>/dev/null | grep -v -i -E "^Test_" | grep -v -E "Base.java$")

if [[ $JAVA_SRCS != "" ]]
then
  echo "Compiling all of the Java sources - your code, plus all of the testcases..."

  # BUGFIX:
  #
  # It appears that Java does *NOT* remove old .class files when compilation
  # fails.  So we had a student who had an old, buggy implementation of the
  # code (which compiled).  When they changed it to a new (non-compiling)
  # version, javac (correctly) fails to compile the code but (maddeningly)
  # leaves the old .class files around.  So, when the code runs, the student
  # sees the old behavior, for reasons that they don't understand.
  rm *.class 2>/dev/null

  javac *.java > javac.out 2>&1
  if [[ $? != 0 || $(cat javac.out | grep -v "Some input files use unchecked" | grep -v "Recompile with") != "" ]]
  then
    echo "--- JAVA COMPILE FAILURE: ---"
    cat javac.out
    exit 1
  else
    rm javac.out
  fi

  # BUGFIX:
  #
  # When there is an error which breaks some (but not all) of the files, javac
  # will abort *all* of the builds.  So we had a student who lost *all* of his
  # testcase points because *some* testcases were broken.  So we'd like to
  # build all of the files individually.
  #
  # However, doing it that way (by default) is likely to be quite slow.  So
  # we will instead only fall back on that plan when the original compile
  # fails.
#  javac *.java || {
#    echo "--- JAVA COMPILE FAILURE ---"
#    echo "javac reported some errors while building your code.  (See the output above.)"
#    echo "This script will now re-run javac on each file, one at a time, in hopes of"
#    echo "succeeding in building some of the testcases."
#    echo
#    echo "In order to not clutter up the script output, this rebuild will *NOT* print"
#    echo "out any error messages; refer to the output above to see why javac failed."
#    echo
#    echo "This process is likely to be slow; if you want, you can use Ctrl-C to kill"
#    echo "this rebuild operation."
#    echo "----------------------------"
#
#    ls -1 *.java 2>&1 | xargs -r -n1 javac 1>/dev/null 2>&1
#  }
fi


attempts=0
passed=0

failList=""



echo
echo "************* Running the testcases *************"
echo


for SEED in 422277 385488 247148 780238 189451 738799 129123 645628 645962
do
  for ALGO in BubbleSort InsertionSort SelectionSort MergeSort QuickSort
  do
    # --- BASIC ---
    # run the algorithm, using the selected seed.
    attempts=$(( attempts+1 ))
    echo "TESTCASE: seed  $SEED algorithm $ALGO"

    OUT=output.$SEED.$ALGO
    timeout 15s java Proj01_Main $ALGO $SEED >$OUT 2>&1
    RC=$?

    if [[ $RC == 124 ]]
    then
      echo "  *** TESTCASE FAILED *** The sort timed out."
      continue
    elif [[ $RC != 0 ]]
    then
      echo "  *** TESTCASE FAILED *** The program terminated with a nonzero return code."
      continue
    fi

    if [[ $(tail -n1 $OUT) != "--- TESTCASE TERMINATED, NO ERRORS FOUND ---" ]]
    then
      echo "  *** TESTCASE FAILED *** The algorithm did not complete correctly.  (Check the file $OUT for details.)"
      continue
    fi

    # otherwise, the testcase passed.
    passed=$(( passed+1 ))
    rm $OUT



    # --- SORTED ---
    # try again, but sorted.  If the student doesn't implement Quicksort modes
    # properly, they will hit timeouts.

    # NOTE: We'll prefix the seed with an extra digit so that the input data
    #       is different than the previous runs.
    attempts=$(( attempts+1 ))
    echo "TESTCASE: seed 1$SEED algorithm $ALGO (sorted)"

    OUT=output.1$SEED.$ALGO.sorted
    timeout 15s java Proj01_Main $ALGO 1$SEED sorted >$OUT 2>&1
    RC=$?

    if [[ $RC == 124 ]]
    then
      echo "  *** TESTCASE FAILED *** The sort timed out."
      continue
    elif [[ $RC != 0 ]]
    then
      echo "  *** TESTCASE FAILED *** The program terminated with a nonzero return code."
      continue
    fi

    if [[ $(tail -n1 $OUT) != "--- TESTCASE TERMINATED, NO ERRORS FOUND ---" ]]
    then
      echo "  *** TESTCASE FAILED *** The algorithm did not complete correctly.  (Check the file $OUT for details.)"
      continue
    fi

    # otherwise, the testcase passed.
    passed=$(( passed+1 ))
    rm $OUT



    # --- MILLION ---
    # if this is QuickSort or Merge Sort, then we can do the 'millions'
    # version, and it should work just fine.  Otherwise, we need to skip it.

    if [[ $ALGO != "MergeSort" && $ALGO != "QuickSort" ]]
    then
      continue
    fi

    # NOTE: We'll prefix the seed with an extra digit so that the input data
    #       is different than the previous runs.
    attempts=$(( attempts+1 ))
    echo "TESTCASE: seed 2$SEED algorithm $ALGO (million)"

    OUT=output.2$SEED.$ALGO.sorted
    timeout 15s java Proj01_Main $ALGO 2$SEED million >$OUT 2>&1
    RC=$?

    if [[ $RC == 124 ]]
    then
      echo "  *** TESTCASE FAILED *** The sort timed out."
      continue
    elif [[ $RC != 0 ]]
    then
      echo "  *** TESTCASE FAILED *** The program terminated with a nonzero return code."
      continue
    fi

    if [[ $(tail -n1 $OUT) != "--- TESTCASE TERMINATED, NO ERRORS FOUND ---" ]]
    then
      echo "  *** TESTCASE FAILED *** The algorithm did not complete correctly.  (Check the file $OUT for details.)"
      continue
    fi

    # otherwise, the testcase passed.
    passed=$(( passed+1 ))
    rm $OUT
  done
done


MAX_AUTO_SCORE=50

echo
echo "*******************************************"
echo "*            OVERALL REPORT"
echo "* attempts: $attempts"
echo "* passed:   $passed"
echo "*"

echo "* score:    $(( MAX_AUTO_SCORE * passed / attempts ))"

echo "*******************************************"


