if [ $# -gt 0 ]
then
  echo "$1" | ./stringGranulator AnamnesisReadingShortMono.wav -c "granulatorDefault.conf"
else
  ./stringGranulator AnamnesisReadingShortMono.wav -t "stringToGranulate.txt" -c "granulatorDefault.conf"
fi
xdg-open out.wav
