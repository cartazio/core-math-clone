#!/bin/bash

MAKE=make

KIND="$1"
shift

FUN="${!#}"
ARGS=("${@:1:$#-1}")

FILE="$(echo src/*/*/"$FUN".c)"
DIR="$(dirname "$FILE")"

if ! [ -d "$DIR" ]; then
    echo "Could not find $FUN"
    exit 1
fi

case "$KIND" in
    --exhaustive)
        "$MAKE" --quiet -C "$DIR" clean
        "$MAKE" --quiet -C "$DIR" check_exhaustive
        if [ "${#ARGS[@]}" -eq 0 ]; then
            MODES=("--rndn" "--rndz" "--rndu" "--rndd")
        else
            MODES=("${ARGS[@]}")
        fi
        for MODE in "${MODES[@]}"; do
            echo "Running exhaustive check in $MODE mode..."
            "$DIR/check_exhaustive" "$MODE"
        done
        ;;
    *)
        echo "Unrecognized command"
        exit 1
esac
