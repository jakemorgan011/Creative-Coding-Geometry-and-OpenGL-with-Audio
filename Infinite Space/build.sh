#!/bin/bash

if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "win32" || "$OSTYPE" == "cygwin" ]]; then
  IS_WINDOWS=true
elif [[ "$(uname -s)" == "MINGW"* || "$(uname -s)" == "MSYS"* ]]; then
  IS_WINDOWS=true
else
  IS_WINDOWS=false
fi

echo "TERMITE - OS Found: $(if $IS_WINDOWS; then echo "Windows"; else echo "Unix Like"; fi)"

if [ -d "build" ]; then
  echo "TERMITE - build folder already exists."
else
  echo "TERMITE - build folder does not yet exist. Creating one now"
  mkdir build 
fi

if [ -d "JUCE" ]; then
  echo "TERMITE - JUCE submodules already exist."
  git submodule update --init --recursive
else
  echo "TERMITE - JUCE submodules do not yet exist. Grabbing from github."
  git submodule add https://github.com/juce-framework/JUCE.git JUCE
  git submodule update --init --recursive
fi



#build
cd build

# windows
if $IS_WINDOWS; then
  echo "TERMITE - config for windows"
  cmake -DCMAKE_BUILD_TYPE:STRING=Debug \
    -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE \
    --no-warn-unused-cli \
    -S "../" \
    -B "../build" \
    -G "Visual Studio 17 2022"

  if [ $? -ne 0]; then
    echo "TERMITE - No Visual Studio found, trying Ninja build"
    cmake -DCMAKE_BUILD_TYPE:STRING=Debug \
      -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE \
      --no-warn-unused-cli \
      -S "../" \
      -B "../build" \
      -G "Ninja"
  fi
# unix
else
  echo "TERMITE - config for Unix-like"
  cmake -DCMAKE_BUILD_TYPE:STRING=Debug \
    -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE \
    -DCMAKE_C_COMPILER:FILEPATH=/usr/bin/clang \
    -DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/clang++ \
    --no-warn-unused-cli -S "../" \
    -B "../build" \
    -G "Unix Makefiles"
fi



cd ..
cmake --build "build" --config Debug --target all -j 11 

# run
echo "TERMITE - beginning run of standalone plugin"
echo "                                                  
                                                  
                                                  
                                                  
                                   ⠿⠉             
                                  ⠯⣻              
                                 ⠁⡿⢯              
                                 ⣽⣯⠉              
                                ⡯⣯⢿               
                               ⠯⣟⣯⢯               
                              ⠁⡿⣯⣟⠛               
                              ⣽⣯⣯⢿                
                             ⡯⣯⣯⣯⡯                
                           ⠉⢯⣟⣯⣯⣟⠉                
                         ⠉⣾⡿⣯⣯⣯⣯⢿                 
                ⠁⠁⠁    ⠉⢯⡿⣯⣯⣯⣯⣯⣯⣽                 
               ⣾⡿⡿⡿⣻⢯⢯⣾⣻⣟⣯⣯⣯⣯⣯⣯⣯⢯                 
               ⠉⡿⣯⣯⣟⣻⣻⣻⣟⣯⣯⣯⣯⣯⣯⣯⣯⠿                 
                ⠯⣟⣯⢿⢯⣽⣟⣯⣯⣯⣯⣯⣯⣯⣯⣟⠛                 
                 ⠯⣽⠯⠛⢿⣯⣯⣯⣯⣯⣯⣯⣯⣯⡿⠉                 
                  ⠁⠁⢯⣯⣯⣯⣯⣯⣯⣯⣯⣯⣯⢿⠁                 
                   ⠛⡿⣯⣯⣯⣷⣷⣯⣯⣯⣯⣯⣻                  
                   ⣻⣯⣯⣷⣿⣷⣯⣯⣯⣯⣯⣯⣾                  
                  ⡯⣯⣯⣷⣿⣷⣯⣯⣯⣯⣯⣯⣟⢯                  
                 ⠯⣟⣯⣷⣿⣷⣯⣯⣯⣯⣯⣟⣯⣟⣽                  
                ⠉⡿⣷⣷⣷⣯⣯⣯⣯⣯⣟⢿⣻⣟⣯⣻                  
               ⠁⡿⣷⣷⣯⣯⣯⣯⣟⣻⣾⠯⣾⡿⣯⣯⡿⠉                 
              ⠁⡿⣿⣷⣯⣯⣟⣻⠿⠁  ⠁⣽⣯⣯⣯⣯⠯                 
             ⠁⡿⣿⣟⣻⡯⠯⠁      ⠁⢯⣯⣯⣯⣽                 
            ⠁⡿⣯⣾⠉            ⠯⡿⣯⣟⠉                
            ⣾⡯⠁               ⠁⢯⣯⢯                
                                ⠉⣾⠁               
                                 ⠁                
                                                  
                                                  
                                                  
                                                  
                                                  
                                                  "
./build/bin/Standalone/Infinite\ Space.app/Contents/MacOS/Infinite\ Space
