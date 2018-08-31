#!/bin/bash

counter=1
while true;
do
  echo "message $counter" | nc -w1 -u 0.0.0.0 1234
  counter=$(($counter + 1))
  sleep 1
done

