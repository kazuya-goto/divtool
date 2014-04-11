#!/bin/sh
DIVTOOL=divtool
BIGFILE=bigfile

run_command() {
  OUT=""; APP=""
  if [ $1 = '-o' ]; then
    shift; OUT=$1; shift
  elif [ $1 = '-a' ]; then
    shift; APP=$1; shift
  fi
  cmd=$*
  rtc=1
  if [ x$OUT != "x" ]; then
    echo $cmd '>' $OUT
    $cmd > $OUT && rtc=0
  elif [ x$APP != "x" ]; then
    echo $cmd '>>' $APP
    $cmd >> $APP && rtc=0
  else
    echo $cmd
    $cmd && rtc=0
  fi
}

clean_all() {
  run_command rm -f $BIGFILE $BIGFILE.[0-9]* $BIGFILE.cat
}

run_command -o $BIGFILE head -c 5m /dev/urandom
run_command rm -f $BIGFILE.[0-9]*
run_command ./$DIVTOOL -c 1000000 $BIGFILE
numfiles=`ls $BIGFILE.[0-9]* | wc -w`
echo "Divided into $numfiles files"
run_command rm -f $BIGFILE.cat
i=0
while [ $i -lt $numfiles ]
do
  run_command -a $BIGFILE.cat cat $BIGFILE.$i
  i=$((i+1))
done
run_command cmp $BIGFILE $BIGFILE.cat
if [ $rtc -eq 0 ]
then
  echo 'Test Passed'
  clean_all
  exit 0
else
  echo 'Test Failed'
  clean_all
  exit 1
fi
