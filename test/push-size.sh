#!/bin/bash -e

pyenv global 3.6
pip install gspread oauth2client pyyaml
git clone https://github.com/libpull/stats_ci.git

json=$(size libpull.a | awk -v date="$(date +"%Y-%m-%d %r")" '
BEGIN { ORS = ""; {printf " { \"timestamp\": \"%s\", ", date }; }
    /text/ {next}
    { printf "%s\"%s\": \"%s\"",
          separator, $6, $1,
      separator = ", "
    }
    END { print "}" }
')

echo $json

echo $json | stats_ci/stats-ci.py
