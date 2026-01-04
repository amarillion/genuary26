#!/bin/bash -e

# First build the project, this brings the build folder up-to-date

# rm -r build/DEBUG_EMSCRIPTEN/
./make-emscripten-buildenv.sh
cp index/*.* build/DEBUG_EMSCRIPTEN/

# test using `http-server build/DEBUG_EMSCRIPTEN/`
#  open http://localhost:8080/`

# Switch to gh-pages branch
git checkout gh-pages

## Remove previous version
# rm -r docs

# Remove previous version but keep README.txt
find docs ! -name 'README.txt' -type f -exec rm -f {} +

# move our dist folder to the github standard docs folder
cp build/DEBUG_EMSCRIPTEN/*.* docs/
git add docs
git commit -m "Update gh-pages with latest version"

git push origin gh-pages

# Switch back
git checkout main
