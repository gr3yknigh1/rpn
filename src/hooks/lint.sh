#!/bin/bash

set -eo pipefail

echo " ====== RUNNING LINT githook ====="
echo 
make lint

echo " ====== SUCCESSFUL LINT      ====="
echo 

