
files=("static/index.html")
target="static.h"

echo "" > $target

for file in "${files[@]}"
do
   xxd -i $file | \
      sed 's/unsigned char/const PROGMEM char/' | \
      sed 's/};/, 0x00 };/' >> $target
done
