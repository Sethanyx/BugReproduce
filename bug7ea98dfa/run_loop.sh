#!/bin/bash
echo "Starting stress test..."
count=0
while true; do
    count=$((count+1))
    echo -n "Run #$count: "

    ./repro &
    PID=$!

    sleep 2

    if ps -p $PID > /dev/null; then
        echo "!!! HANG DETECTED !!! Bug Reproduced!"
        echo "CPU 2 is stuck in idle. Process $PID is waiting for a lock that will never be released."
        wait $PID
        break
    else
        echo "Finished."
    fi
done
