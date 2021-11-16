# This script is intended to get executed via condor_run.sh, not alone.
mkdir -p ${1}
subfile=${1##*/}

cat > ${1}/$subfile.sub << EOF
Executable   = $2
Arguments    = $3 $4 $5 $6 $7 $8 $9
Initialdir   = ${1}
Log          = ${subfile}.log
Error        = ${subfile}.err
Output       = ${subfile}.out
#Input        = WHEN_YOU_HAVE_INPUT_FILES
Universe     = vanilla
GetEnv       = True
Request_cpus = 1
Request_memory = 1G
Notification = Always
Requirements = (machine != "kunpl-node03" && machine != "kunpl-node09")
Notify_user  = simpson9794@korea.ac.kr
Queue
EOF

condor_submit ${1}/$subfile.sub
rm ${1}/$subfile.sub
